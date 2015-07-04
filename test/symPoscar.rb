#!/usr/bin/env ruby
#   Copyright (C) 2008 Atsushi Togo
#   togo.atsushi@gmail.com
# 
#   This program is free software; you can redistribute it and/or
#   modify it under the terms of the GNU General Public License
#   as published by the Free Software Foundation; either version 2
#   of the License, or (at your option) any later version.
#   
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#   
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to
#   the Free Software Foundation, Inc., 51 Franklin Street,
#   Fifth Floor, Boston, MA 02110-1301, USA, or see
#   http://www.gnu.org/copyleft/gpl.html
#
# Usage: symPoscar.rb [OPTION] [structure]
# OPTION: -s, --symprec= : Symmetry check precision

require 'optparse'
require 'ostruct'
require 'getspg.so'
require 'poscar'
include Getspg

spg2hall = [0,
            1,   2,   3,   6,   9,  18,  21,  30,  39,  57,
            60,  63,  72,  81,  90, 108, 109, 112, 115, 116,
            119, 122, 123, 124, 125, 128, 134, 137, 143, 149,
            155, 161, 164, 170, 173, 176, 182, 185, 191, 197,
            203, 209, 212, 215, 218, 221, 227, 228, 230, 233,
            239, 245, 251, 257, 263, 266, 269, 275, 278, 284,
            290, 292, 298, 304, 310, 313, 316, 322, 334, 335,
            337, 338, 341, 343, 349, 350, 351, 352, 353, 354,
            355, 356, 357, 358, 359, 361, 363, 364, 366, 367,
            368, 369, 370, 371, 372, 373, 374, 375, 376, 377,
            378, 379, 380, 381, 382, 383, 384, 385, 386, 387,
            388, 389, 390, 391, 392, 393, 394, 395, 396, 397,
            398, 399, 400, 401, 402, 404, 406, 407, 408, 410,
            412, 413, 414, 416, 418, 419, 420, 422, 424, 425,
            426, 428, 430, 431, 432, 433, 435, 436, 438, 439,
            440, 441, 442, 443, 444, 446, 447, 448, 449, 450,
            452, 454, 455, 456, 457, 458, 460, 462, 463, 464,
            465, 466, 467, 468, 469, 470, 471, 472, 473, 474,
            475, 476, 477, 478, 479, 480, 481, 482, 483, 484,
            485, 486, 487, 488, 489, 490, 491, 492, 493, 494,
            495, 497, 498, 500, 501, 502, 503, 504, 505, 506,
            507, 508, 509, 510, 511, 512, 513, 514, 515, 516,
            517, 518, 520, 521, 523, 524, 525, 527, 529, 530,
            531]

def show_dataset(dataset, lattice, position, cell, nonewline, is_long_output, is_dataset, is_operations)
  spgnum, spg, hallnum, hall_symbol, setting, t_mat, o_shift,
  rotations, translations, wyckoffs,
  brv_lattice, brv_types, brv_positions = dataset
  ptg_symbol, ptg_num, trans_mat = getptg(rotations)

  if nonewline
    print "#{spg.strip} (#{spgnum})"
  else
    puts "#{spg.strip} (#{spgnum}) / #{ptg_symbol} / #{hall_symbol.strip} (#{hallnum}) / #{setting}"

    if is_long_output
      puts "----------- original -----------"
      lattice.each do |vec|
        printf("%10.5f %10.5f %10.5f\n", vec[0], vec[1], vec[2]);
      end

      puts "------------ final -------------"
      brv_lattice.each do |vec|
        printf("%10.5f %10.5f %10.5f\n", vec[0], vec[1], vec[2]);
      end

      brv_types.size.times do |i|
        printf("%d: %d  %10.5f %10.5f %10.5f\n", i+1, brv_types[i], 
               brv_positions[i][0], brv_positions[i][1], brv_positions[i][2]);
      end
    end

    if is_dataset
      puts "------ transformation matrix -----"
      t_mat.each do |row|
        printf("%10.5f %10.5f %10.5f\n", row[0], row[1], row[2]);
      end

      puts "---------- origin shift ----------"
      printf("%10.5f %10.5f %10.5f\n", o_shift[0], o_shift[1], o_shift[2]);
      
      puts "--------- Wyckoff position ----------"
      wl = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
      wyckoffs.each_with_index do |w, i|
        pos = []
        3.times do |j|
          pos.push(position[i][j] - position[i][j].floor)
        end
        printf("%4d %2s  %s %8.5f %8.5f %8.5f\n",
               i+1, cell.atoms[i].name, wl[w,1], pos[0], pos[1], pos[2])
      end
    end
  end

  if is_operations or is_dataset
    show_operations(rotations, translations)
  end
end

def show_operations(rotations, translations)
  rotations.size.times do |i|
    print "----", i+1, "----\n"
    rotations[i].each do |row|
      printf("%2d %2d %2d\n", row[0], row[1], row[2])
    end
    printf("%f %f %f\n", translations[i][0], translations[i][1], translations[i][2])
  end
end

def transform_to_c(lattice, position, types, symprec, filename='findspg.c')
  w = open(filename, "w")
  w.print <<HERE
#include <stdio.h>
#include "spglib.h"

static void get_international(void);

int main(void)
{
  get_international();
  return 0;
}

static void get_international(void)
{
  double lattice[3][3] = {{#{lattice[0][0]}, #{lattice[0][1]}, #{lattice[0][2]}},
                          {#{lattice[1][0]}, #{lattice[1][1]}, #{lattice[1][2]}},
                          {#{lattice[2][0]}, #{lattice[2][1]}, #{lattice[2][2]}}};
  double position[][3] =
    {
HERE

  position.each do |vec|
    w.puts "      {#{vec[0]}, #{vec[1]}, #{vec[2]}},"
  end
  w.puts "    };"
  w.print "  int types[] = {"
  types.each {|t| w.print "#{t}, "}
  w.puts "};"
  w.print <<HERE
  int num_spg, num_atom = #{types.size};
  char symbol[21];
  
  num_spg = spg_get_international(symbol, lattice, position, types, num_atom, #{symprec});
  if (num_spg > 0) {
    printf("%s (%d)\\n", symbol, num_spg);
  }
}
HERE
  w.close()
end

def parse_cell(cell, shift_string, pos_shift)
  if shift_string
    pos_shift = []
    shift_string.split.each do |val|
      pos_shift << val.to_f
    end
  end

  lattice = cell.axis.transpose
  names = (cell.atoms.collect {|atom| atom.name}).uniq
  position = []
  types = []
  names.each_with_index do |name, i|
    cell.atoms.each do |atom|
      if atom.name == name
        apos = atom.position
        position << [apos[0] + pos_shift[0],
                     apos[1] + pos_shift[1],
                     apos[2] + pos_shift[2]]
        types << i + 1
      end
    end
  end

  return lattice, position, types
end


options = OpenStruct.new
options.symprec = 1e-5
options.hall_number = 0
options.angle_tolerance = -1.0
options.nonewline = false
options.pos_shift = [0,0,0]
options.shift_string = false
options.show_primitive = false
options.is_long_output = false
options.is_operations = false
options.is_numerical_operations = false
options.is_dataset = false
options.is_check_settings = false
options.to_c = false

OptionParser.new do |opts|
  opts.on('-s', '--symprec VALUE', 'Symmetry check precision') do |v|
    options.symprec = v.to_f
  end

  opts.on('-a', '--angle_tolerance VALUE', 'Symmetry check precision for angle between lattice vectors in degrees') do |v|
    options.angle_tolerance = v.to_f
  end

  opts.on('--shift VALUE', 'uniform shift of internal atomic positions') do |v|
    options.shift_string = v
  end

  opts.on('-n', '--nonewline', 'Do not output the trailing newline') do 
    options.nonewline = true
  end

  opts.on('-l', '--long', 'Long output') do
    options.is_long_output = true
  end

  opts.on('-o', '--operations', 'Symmetry operations') do
    options.is_operations = true
  end

  opts.on('--numerical_operations', 'Numerical symmetry operations') do
    options.is_numerical_operations = true
  end

  opts.on('-d', '--dataset', 'Dataset') do
    options.is_dataset = true
  end

  opts.on('--settings', 'Check all settings') do
    options.is_check_settings = true
  end

  opts.on('--primitive', 'Show primitive cell') do
    options.show_primitive = true
  end

  opts.on('--to_c', 'Convert to C code') do
    options.to_c = true
  end

  opts.on('--hall VALUE', 'Hall symbol by the numbering') do |v|
    options.hall_number = v.to_i
  end
end.parse!

cell = Vasp::Poscar.new(ARGV.shift).cell
lattice, position, types = parse_cell(cell,
                                      options.shift_string,
                                      options.pos_shift)
if options.to_c
  transform_to_c(lattice, position, types, options.symprec)
end

dataset = get_dataset(lattice,
                      position,
                      types,
                      options.hall_number,
                      options.symprec,
                      options.angle_tolerance)

if not dataset.empty?
  if not options.is_check_settings
    show_dataset(dataset,
                 lattice,
                 position,
                 cell,
                 options.nonewline,
                 options.is_long_output,
                 options.is_dataset,
                 options.is_operations)
  else
    spgnum_orig = dataset[0]
    num_settings = spg2hall[spgnum_orig + 1] - spg2hall[spgnum_orig]
    puts "There are #{num_settings} settings:"
    num_settings.times {|i|
      dataset_with_hall = get_dataset(lattice,
                                      position,
                                      types,
                                      spg2hall[spgnum_orig] + i,
                                      options.symprec,
                                      options.angle_tolerance)
      spgnum, spg, hallnum, hall_symbol, setting, t_mat, o_shift,
      rotations, translations, wyckoffs = dataset_with_hall
      ptg_symbol, ptg_num, trans_mat = getptg(rotations)
      puts "#{i + 1}: #{spg.strip} (#{spgnum}) / #{ptg_symbol} / #{hall_symbol.strip} (#{hallnum}) / #{setting}"
    }
  end
end

if options.show_primitive
  lat_ary, pos_ary, typ_ary = find_primitive(lattice,
                                             position,
                                             types,
                                             options.symprec,
                                             options.angle_tolerance)

  atoms = []
  atomName = "ABCDEFGHIJKLMNOPQRSTUVWXYZ".split(//)
  typ_ary.size.times do |i|
    atoms.push(Vasp::Atom.new(pos_ary[i], name=atomName[typ_ary[i]]))
  end

  axis = [[lat_ary[0][0], lat_ary[1][0], lat_ary[2][0]],
          [lat_ary[0][1], lat_ary[1][1], lat_ary[2][1]],
          [lat_ary[0][2], lat_ary[1][2], lat_ary[2][2]]]
  cell = Crystal::Cell.new(axis, atoms, comment="")
  
  Vasp::CellToPoscar.new(cell).print
end

if options.is_numerical_operations
  rotations, translations = get_symmetry(lattice,
                                         position,
                                         types,
                                         options.symprec)
  show_operations(rotations, translations)
end
