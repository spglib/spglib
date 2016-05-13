#include <stdio.h>
#include "ruby.h"
#include "spglib.h"

VALUE Getspg = Qnil;
void Init_getspg(void);
VALUE method_getspg(VALUE self,
		    VALUE r_size,
		    VALUE r_lattice,
		    VALUE r_position,
		    VALUE r_types,
		    VALUE r_symprec,
		    VALUE r_angle_symprec);
VALUE method_getptg(VALUE self,
		    VALUE r_rotations);
VALUE method_find_primitive(VALUE self,
			    VALUE r_lattice,
			    VALUE r_position,
			    VALUE r_types,
			    VALUE r_symprec,
			    VALUE r_angle_symprec);
VALUE method_get_dataset(VALUE self,
			 VALUE r_lattice,
			 VALUE r_position,
			 VALUE r_types,
			 VALUE r_hall_num,
			 VALUE r_symprec,
			 VALUE r_angle_symprec);
VALUE method_get_symmetry(VALUE self,
			  VALUE r_lattice,
			  VALUE r_position,
			  VALUE r_types,
			  VALUE r_symprec,
			  VALUE r_angle_symprec);
VALUE method_standardize_cell(VALUE self,
			      VALUE r_lattice,
			      VALUE r_position,
			      VALUE r_types,
			      VALUE r_to_primitive,
			      VALUE r_leave_distrtion,
			      VALUE r_symprec,
			      VALUE r_angle_symprec);

void Init_getspg(void)
{
  Getspg = rb_define_module("Getspg");
  rb_define_method(Getspg, "getptg", method_getptg, 1);
  rb_define_method(Getspg, "find_primitive", method_find_primitive, 5);
  rb_define_method(Getspg, "get_dataset", method_get_dataset, 6);
  rb_define_method(Getspg, "get_symmetry", method_get_symmetry, 5);
}

VALUE method_getptg(VALUE self, VALUE r_rotations)
{
  int i, j, k, size, ptg_num;
  char symbol[6];
  int trans_mat[3][3];
  VALUE array, matrix, vector;

  size = RARRAY_LEN( r_rotations );
  int rotations[size][3][3];

  for ( i = 0; i < size; i++ ) {
    for ( j = 0; j < 3; j++ ) {
      for ( k = 0; k < 3; k++ ) {
	rotations[i][j][k] = NUM2INT( rb_ary_entry( rb_ary_entry( rb_ary_entry( r_rotations, i ), j ), k ) );
      }
    }				      
  }
  
  ptg_num = spg_get_pointgroup( symbol, trans_mat, rotations, size );
  array = rb_ary_new();
  rb_ary_push( array, rb_str_new2( symbol ) );
  rb_ary_push( array, INT2NUM( ptg_num ) );
  matrix = rb_ary_new();
  for ( i = 0; i < 3; i++) {
    vector = rb_ary_new();
    for ( j = 0; j < 3; j++ ) {
      rb_ary_push( vector, INT2NUM( trans_mat[i][j] ) );
    }
    rb_ary_push( matrix, vector);
  }
  rb_ary_push( array, matrix );
  
  return array;
}

VALUE method_find_primitive(VALUE self,
			    VALUE r_lattice,
			    VALUE r_position,
			    VALUE r_types,
			    VALUE r_symprec,
			    VALUE r_angle_symprec)
{
  int i, j, k, num_atom, num_prim_atom;
  double symprec, lattice[3][3];
  VALUE array, vector, lat_ary, pos_ary, typ_ary;
     
  num_atom = RARRAY_LEN(r_types);
     
  double position[num_atom][3];
  int types[num_atom];

  symprec = NUM2DBL(r_symprec);

  for (i = 0; i < num_atom; i++) {
    for (j = 0; j < 3; j++) {
      position[i][j] = NUM2DBL(rb_ary_entry(rb_ary_entry(r_position, i), j));
      types[i] = NUM2DBL(rb_ary_entry(r_types, i));
    }
  }

  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      lattice[i][j] = NUM2DBL(rb_ary_entry(rb_ary_entry(r_lattice, i), j));
    }
  }

  num_prim_atom = spgat_standardize_cell(lattice,
					 position,
					 types,
					 num_atom,
					 1,
					 0,
					 symprec,
					 NUM2DBL(r_angle_symprec));
  
  array = rb_ary_new();
  lat_ary = rb_ary_new();
  for (i = 0; i < 3 ; i++) {
    vector = rb_ary_new();
    for (j = 0; j < 3; j++) {
      rb_ary_push(vector, rb_float_new(lattice[i][j]));
    }
    rb_ary_push(lat_ary, vector);
  }
  rb_ary_push(array, lat_ary);

  pos_ary = rb_ary_new();
  typ_ary = rb_ary_new();
  for (i = 0; i < num_prim_atom; i++) {
    vector = rb_ary_new();
    rb_ary_push(typ_ary, INT2NUM(types[i]));
    for (j = 0; j < 3 ; j++) {
      rb_ary_push(vector, rb_float_new(position[i][j]));
    }
    rb_ary_push(pos_ary, vector);
  }

  rb_ary_push(array, pos_ary);
  rb_ary_push(array, typ_ary);

  return array;
}

VALUE method_get_dataset(VALUE self,
			 VALUE r_lattice,
			 VALUE r_position,
			 VALUE r_types,
			 VALUE r_hall_num,
			 VALUE r_symprec,
			 VALUE r_angle_symprec)
{
  int i, j, k, num_atom;
  double symprec, lattice[3][3];
  SpglibDataset *dataset;
  VALUE mat, vec, row, array, r_tmat, r_oshift, r_rot, r_trans, r_wyckoffs, r_std_lattice, r_std_positions, r_std_types;

  num_atom = RARRAY_LEN(r_types);

  double position[num_atom][3];
  int types[num_atom];

  symprec = NUM2DBL(r_symprec);

  for (i = 0; i < num_atom; i++) {
    for (j = 0; j < 3; j++) {
      position[i][j] = NUM2DBL(rb_ary_entry(rb_ary_entry(r_position, i), j));
      types[i] = NUM2DBL(rb_ary_entry(r_types, i));
    }
  }

  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      lattice[i][j] = NUM2DBL(rb_ary_entry(rb_ary_entry(r_lattice, i), j));
    }
  }

  dataset = spgat_get_dataset_with_hall_number(lattice,
					       position,
					       types,
					       num_atom,
					       NUM2INT(r_hall_num),
					       symprec,
					       NUM2DBL(r_angle_symprec));

  array = rb_ary_new();

  if (dataset == NULL) {
    goto err;
  }

  rb_ary_push(array, INT2NUM(dataset->spacegroup_number));
  rb_ary_push(array, rb_str_new2(dataset->international_symbol));
  rb_ary_push(array, INT2NUM(dataset->hall_number));
  rb_ary_push(array, rb_str_new2(dataset->hall_symbol));
  rb_ary_push(array, rb_str_new2(dataset->choice));
  
  /* Transformation_matrix */
  r_tmat = rb_ary_new();
  for (i = 0; i < 3; i++) {
    row = rb_ary_new();
    for (j = 0; j < 3; j++) {
      rb_ary_push(row,
		  rb_float_new(dataset->transformation_matrix[i][j]));
    }
    rb_ary_push(r_tmat, row);
  }
  rb_ary_push(array, r_tmat);

  /* Origin shift */
  r_oshift = rb_ary_new();
  for (i = 0; i < 3; i++) {
    rb_ary_push(r_oshift, rb_float_new(dataset->origin_shift[i]));
  }
  rb_ary_push(array, r_oshift);
 
  /* Rotations, translations */
  r_rot = rb_ary_new();
  r_trans = rb_ary_new();
  for (i = 0; i < dataset->n_operations; i++) {
    mat = rb_ary_new();
    vec = rb_ary_new();
    for (j = 0; j < 3; j++) {
      rb_ary_push(vec, rb_float_new(dataset->translations[i][j]));
      row = rb_ary_new();
      for (k = 0; k < 3; k++) {
	rb_ary_push(row, rb_float_new(dataset->rotations[i][j][k]));
      }
      rb_ary_push(mat, row);
    }
    rb_ary_push(r_trans, vec);
    rb_ary_push(r_rot, mat);
  }
  rb_ary_push(array, r_rot);
  rb_ary_push(array, r_trans);

  /* Wyckoff letters */
  r_wyckoffs = rb_ary_new();
  for (i = 0; i < dataset->n_atoms; i++) {
    r_wyckoffs = rb_ary_push(r_wyckoffs, INT2NUM(dataset->wyckoffs[i]));
  }
  rb_ary_push(array, r_wyckoffs);

  /* Bravais lattice */
  r_std_lattice = rb_ary_new();
  for (i = 0; i < 3; i++) {
    vec = rb_ary_new();
    for (j = 0; j < 3; j++) {
      rb_ary_push(vec, rb_float_new(dataset->std_lattice[i][j]));
    }
    rb_ary_push(r_std_lattice, vec);
  }
  rb_ary_push(array, r_std_lattice);
  
  r_std_positions = rb_ary_new();
  r_std_types = rb_ary_new();
  for (i = 0; i < dataset->n_std_atoms; i++) {
    vec = rb_ary_new();
    for (j = 0; j < 3; j++) {
      rb_ary_push(vec, rb_float_new(dataset->std_positions[i][j]));
    }
    rb_ary_push(r_std_positions, vec);
    rb_ary_push(r_std_types, INT2NUM(dataset->std_types[i]));
  }
  rb_ary_push(array, r_std_types);
  rb_ary_push(array, r_std_positions);
  
  spg_free_dataset(dataset);
  
 err:

  return array;
}

VALUE method_get_symmetry(VALUE self,
			  VALUE r_lattice,
			  VALUE r_position,
			  VALUE r_types,
			  VALUE r_symprec,
			  VALUE r_angle_symprec)
{
  int i, j, k, num_atom, num_sym;
  double symprec, lattice[3][3];
  VALUE mat, vec, row, array, r_rot, r_trans;

  num_atom = RARRAY_LEN(r_types);

  double position[num_atom][3];
  int types[num_atom];

  int max_num_sym = num_atom * 48;
  int rotations[max_num_sym][3][3];
  double translations[max_num_sym][3];

  symprec = NUM2DBL(r_symprec);

  for (i = 0; i < num_atom; i++) {
    for (j = 0; j < 3; j++) {
      position[i][j] = NUM2DBL(rb_ary_entry(rb_ary_entry(r_position, i), j));
      types[i] = NUM2DBL(rb_ary_entry(r_types, i));
    }
  }

  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      lattice[i][j] = NUM2DBL(rb_ary_entry(rb_ary_entry(r_lattice, i), j));
    }
  }

  num_sym = spgat_get_symmetry_numerical(rotations,
					 translations,
					 max_num_sym,
					 lattice,
					 position,
					 types,
					 num_atom,
					 symprec,
					 NUM2DBL(r_angle_symprec));

  array = rb_ary_new();

  /* Rotations, translations */
  r_rot = rb_ary_new();
  r_trans = rb_ary_new();
  for (i = 0; i < num_sym; i++) {
    mat = rb_ary_new();
    vec = rb_ary_new();
    for (j = 0; j < 3; j++) {
      rb_ary_push(vec, rb_float_new(translations[i][j]));
      row = rb_ary_new();
      for (k = 0; k < 3; k++) {
	rb_ary_push(row, rb_float_new(rotations[i][j][k]));
      }
      rb_ary_push(mat, row);
    }
    rb_ary_push(r_trans, vec);
    rb_ary_push(r_rot, mat);
  }
  rb_ary_push(array, r_rot);
  rb_ary_push(array, r_trans);

 err:

  return array;
}

VALUE method_standardize_cell(VALUE self,
			      VALUE r_lattice,
			      VALUE r_position,
			      VALUE r_types,
			      VALUE r_to_primitive,
			      VALUE r_leave_distrtion,
			      VALUE r_symprec,
			      VALUE r_angle_symprec)
{
}
