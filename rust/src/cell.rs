//! Data structures and methods which apply to or operate on a crystalline cell

use crate::{DelaunayReduceError, NiggliReduceError, StandardizeError};
use spglib_sys as ffi;
use std::os::raw::c_int;

/// Standardization operations which can be applied to a cell
#[derive(Clone, Debug)]
pub enum StandardizeOps {
    /// Find the ideal structure from one which may be slightly distorted
    Ideal,
    /// Find the primitive cell
    Primitive,
    /// Idealize and find primitive
    Both,
}

/// Representation of a crystalline cell
#[derive(Clone, Debug, Default)]
pub struct Cell {
    /// Lattice vectors
    pub lattice: [[f64; 3]; 3],
    /// Position of each atom
    pub positions: Vec<[f64; 3]>,
    /// Type of each atom
    pub types: Vec<i32>,
}

impl Cell {
    /// Returns a crystalline cell
    ///
    /// # Examples
    ///
    /// Generate a simple BCC cell
    ///
    /// ```
    /// # use spglib::cell::Cell;
    /// let lattice = [[4., 0., 0.], [0., 4., 0.], [0., 0., 4.]];
    /// let positions = &[[0., 0., 0.], [0.5, 0.5, 0.5]];
    /// let types = &[1, 1];
    /// let bcc_cell = Cell::new(lattice, positions, types);
    /// ```
    ///
    /// # Panics
    ///
    /// Panics if the lengths of `positions` and `types` are not equal
    pub fn new(lattice: [[f64; 3]; 3], positions: &[[f64; 3]], types: &[i32]) -> Cell {
        if positions.len() != types.len() {
            panic!("number of positions must match number of types")
        }
        Cell {
            lattice,
            positions: positions.to_owned(),
            types: types.to_owned(),
        }
    }

    /// Modifies the structure with one of the available standardization operations
    ///
    /// # Examples
    ///
    /// Find primitive representation of a BCC cell
    ///
    /// ```
    /// # use spglib::cell::{Cell, StandardizeOps};
    /// # let lattice = [[4., 0., 0.], [0., 4., 0.], [0., 0., 4.]];
    /// # let positions = &[[0., 0., 0.], [0.5, 0.5, 0.5]];
    /// # let types = &[1, 1];
    /// # let mut bcc_cell = Cell::new(lattice, positions, types);
    /// bcc_cell.standardize(StandardizeOps::Primitive, 1e-5).unwrap();
    /// assert_eq!(
    ///     bcc_cell.lattice,
    ///     [[-2.0, 2.0, 2.0], [2.0, -2.0, 2.0], [2.0, 2.0, -2.0]]
    /// );
    /// ```
    pub fn standardize(
        &mut self,
        std_ops: StandardizeOps,
        symprec: f64,
    ) -> Result<(), StandardizeError> {
        let (to_primitive, no_idealize) = match std_ops {
            StandardizeOps::Ideal => (0, 0),
            StandardizeOps::Primitive => (1, 1),
            StandardizeOps::Both => (1, 0),
        };
        let res = unsafe {
            ffi::spg_standardize_cell(
                self.lattice.as_ptr() as *mut [f64; 3],
                self.positions.as_ptr() as *mut [f64; 3],
                self.types.as_ptr() as *mut c_int,
                self.positions.len() as c_int,
                to_primitive,
                no_idealize,
                symprec,
            )
        };
        if res == 0 {
            return Err(StandardizeError);
        }
        Ok(())
    }

    /// Applies a Delaunay reduction
    pub fn delaunay_reduce(&mut self, symprec: f64) -> Result<(), DelaunayReduceError> {
        let res =
            unsafe { ffi::spg_delaunay_reduce(self.lattice.as_ptr() as *mut [f64; 3], symprec) };
        if res == 0 {
            return Err(DelaunayReduceError);
        }
        Ok(())
    }

    /// Applies a Niggli reduction
    pub fn niggli_reduce(&mut self, symprec: f64) -> Result<(), NiggliReduceError> {
        let res =
            unsafe { ffi::spg_niggli_reduce(self.lattice.as_ptr() as *mut [f64; 3], symprec) };
        if res == 0 {
            return Err(NiggliReduceError);
        }
        Ok(())
    }

    // TODO
    // pub fn ir_reciprocal_mesh(
    //     &mut self,
    //     mesh: [i32; 3],
    //     shift: [i32; 3],
    //     symprec: f64,
    // ) -> (Box<[[i32; 3]]>, Box<[i32]>) {
    //     let capacity = usize::try_from(mesh[0] * mesh[1] * mesh[2]).unwrap();
    //     let grid_address = Vec::<[i32; 3]>::with_capacity(capacity).as_mut_ptr();
    //     let ir_mapping_table = Vec::<i32>::with_capacity(capacity).as_mut_ptr();
    //     let mut lattice = self.lattice.clone();
    //     let mut positions = self.positions.clone();
    //     let types = self.types.clone();
    //     let n_atoms = positions.len();
    //     let n_kpoints = unsafe {
    //         ffi::spg_get_ir_reciprocal_mesh(
    //             grid_address,
    //             ir_mapping_table,
    //             mesh.as_ptr(),
    //             shift.as_ptr(),
    //             1,
    //             lattice.as_mut_ptr(),
    //             positions.as_mut_ptr(),
    //             types.as_ptr(),
    //             n_atoms as c_int,
    //             symprec,
    //         )
    //     } as usize;
    //     // TODO check n_kpoints
    //     let grid_address = unsafe { Vec::from_raw_parts(grid_address, n_kpoints, n_kpoints) };
    //     let ir_mapping_table =
    //         unsafe { Vec::from_raw_parts(ir_mapping_table, n_kpoints, n_kpoints) };
    //     (
    //         grid_address.into_boxed_slice(),
    //         ir_mapping_table.into_boxed_slice(),
    //     )
    // }
}

#[cfg(test)]
mod tests {
    use crate::cell::StandardizeOps;
    use crate::internal::{bcc_cell, corrundum_cell, distorted_bcc_cell, distorted_corrundum_cell};

    #[test]
    fn find_primitive() {
        let symprec = 1e-5;
        let mut cell = bcc_cell();
        cell.standardize(StandardizeOps::Primitive, symprec)
            .unwrap();
        let mut cell = corrundum_cell();
        cell.standardize(StandardizeOps::Primitive, symprec)
            .unwrap();
    }

    #[test]
    fn find_ideal() {
        let symprec = 1e-5;
        let mut cell = distorted_bcc_cell();
        cell.standardize(StandardizeOps::Ideal, symprec).unwrap();
        let mut cell = distorted_corrundum_cell();
        cell.standardize(StandardizeOps::Ideal, symprec).unwrap();
    }

    #[test]
    fn standardize() {
        let symprec = 1e-5;
        let mut cell = distorted_bcc_cell();
        cell.standardize(StandardizeOps::Both, symprec).unwrap();
        let mut cell = distorted_corrundum_cell();
        cell.standardize(StandardizeOps::Both, symprec).unwrap()
    }
}
