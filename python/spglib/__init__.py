"""Python bindings for C library for finding and handling crystal."""
# Copyright (C) 2015 Atsushi Togo
# All rights reserved.
#
# This file is part of spglib.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# * Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in
#   the documentation and/or other materials provided with the
#   distribution.
#
# * Neither the name of the spglib project nor the names of its
#   contributors may be used to endorse or promote products derived
#   from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

# fmt: off
from .spglib import (  # noqa: F401
    delaunay_reduce,
    find_primitive,
    get_BZ_grid_points_by_rotations,
    get_error_message,
    get_grid_point_from_address,
    get_grid_points_by_rotations,
    get_hall_number_from_symmetry,
    get_ir_reciprocal_mesh,
    get_layergroup,
    get_magnetic_spacegroup_type,
    get_magnetic_symmetry,
    get_magnetic_symmetry_dataset,
    get_magnetic_symmetry_from_database,
    get_pointgroup,
    get_spacegroup,
    get_spacegroup_type,
    get_spacegroup_type_from_symmetry,
    get_stabilized_reciprocal_mesh,
    get_symmetry,
    get_symmetry_dataset,
    get_symmetry_from_database,
    get_version,
    niggli_reduce,
    refine_cell,
    relocate_BZ_grid_address,
    standardize_cell,
)

# fmt: on

__version__ = "%d.%d.%d" % get_version()
