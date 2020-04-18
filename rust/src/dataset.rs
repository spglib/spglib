//! Crystallographic dataset for a given structure

use crate::cell::Cell;
use spglib_sys as ffi;
use std::ffi::{CStr, CString};
use std::os::raw::c_int;

/// A database of crystallographic information for a given structure
#[derive(Debug, Clone)]
pub struct Dataset {
    /// The space group type number defined in International Tables for Crystallography
    pub spacegroup_number: i32,
    /// The serial number from 1 to 530
    pub hall_number: i32,
    /// The full Hermann-Mauguin notation
    pub international_symbol: String,
    /// The hall symbol
    pub hall_symbol: String,
    /// The information on unique axis, setting, or cell choices
    pub choice: String,
    /// The result of space-group-type matching under a set of unique axis, setting, and cell choices.
    /// For more detail: https://atztogo.github.io/spglib/definition.html#transformation-matrix-boldsymbol-p-and-origin-shift-boldsymbol-p
    pub transformation_matrix: [[f64; 3]; 3],
    /// The result of space-group-type matching under a set of unique axis, setting, and cell choices.
    /// For more detail: https://atztogo.github.io/spglib/definition.html#transformation-matrix-boldsymbol-p-and-origin-shift-boldsymbol-p
    pub origin_shift: [f64; 3],
    /// The number of symmetry operations
    pub n_operations: i32,
    /// The rotation symmetry operations
    pub rotations: Box<[[[i32; 3]; 3]]>,
    /// The translation symmetry operations
    pub translations: Box<[[f64; 3]]>,
    /// The number of atoms in the input cell
    pub n_atoms: i32,
    /// The wyckoff letters encoded as integer numbers
    pub wyckoffs: Box<[i32]>,
    /// Site symmetry symbols for a given space group type
    pub site_symmetry_symbols: Box<[String]>,
    /// The mapping table from the atomic indices of the input cell to the atomic indices of symmetrically independent atoms
    pub equivalent_atoms: Box<[i32]>,
    /// The atomic indices in the primitive cell of the input structure
    pub mapping_to_primitive: Box<[i32]>,
    /// Number of atoms in the standardized cell after idealization
    pub n_std_atoms: i32,
    /// Lattice of the standardized cell after idealization
    pub std_lattice: [[f64; 3]; 3],
    /// Types in the standardized cell after idealization
    pub std_types: Box<[i32]>,
    /// Positions in the standardized cell after idealization
    pub std_positions: Box<[[f64; 3]]>,
    /// Rotation matrix which rotates the cell structure from the pre idealization state to the post idealized state
    pub std_rotation_matrix: [[f64; 3]; 3],
    /// The atomic indices in the primitive cell of the standardized crystal structure where the same number represents the same atom in the primitive cell
    pub std_mapping_to_primitive: Box<[i32]>,
    /// The symbol of the crystallographic point group in Hermann-Mauguin notation
    pub pointgroup_symbol: String,
}

impl Dataset {
    /// Returns the dataset for a given cell
    ///
    /// # Examples
    ///
    /// Get the dataset for a BCC cell
    ///
    /// ```
    /// # use spglib::cell::Cell;
    /// # use spglib::dataset::Dataset;
    /// # let lattice = [[4., 0., 0.], [0., 4., 0.], [0., 0., 4.]];
    /// # let positions = &[[0., 0., 0.], [0.5, 0.5, 0.5]];
    /// # let types = &[1, 1];
    /// # let mut bcc_cell = Cell::new(lattice, positions, types);
    /// let dataset = Dataset::new(&mut bcc_cell, 1e-5);
    /// assert_eq!(
    ///     dataset.hall_number,
    ///     529
    /// );
    /// ```
    pub fn new(cell: &mut Cell, symprec: f64) -> Dataset {
        let res = unsafe {
            ffi::spg_get_dataset(
                cell.lattice.as_ptr() as *mut [f64; 3],
                cell.positions.as_ptr() as *mut [f64; 3],
                cell.types.as_ptr() as *const c_int,
                cell.positions.len() as c_int,
                symprec,
            )
        };
        raw_spglib_dataset_to_wrapper_dataset(res)
    }

    /// Returns the dataset for a given cell with a known hall number
    pub fn with_hall_number(cell: &mut Cell, hall_number: i32, symprec: f64) -> Dataset {
        let res = unsafe {
            ffi::spg_get_dataset_with_hall_number(
                cell.lattice.as_ptr() as *mut [f64; 3],
                cell.positions.as_ptr() as *mut [f64; 3],
                cell.types.as_ptr() as *const c_int,
                cell.positions.len() as c_int,
                hall_number as c_int,
                symprec,
            )
        };
        raw_spglib_dataset_to_wrapper_dataset(res)
    }
}

fn raw_spglib_dataset_to_wrapper_dataset(raw: *mut ffi::SpglibDataset) -> Dataset {
    // dereference the raw pointer
    let raw = unsafe { &mut *raw };
    // store the number of atoms for convenience
    let n_atoms = raw.n_atoms as usize;
    // store the number of operations for convenience
    let n_operations = raw.n_operations as usize;
    // store the number of standardized atoms for convenience
    let n_std_atoms = raw.n_std_atoms as usize;
    // construct and return the wrapper type
    Dataset {
        spacegroup_number: raw.spacegroup_number as i32,
        hall_number: raw.hall_number as i32,
        international_symbol: CString::from(unsafe {
            CStr::from_ptr(raw.international_symbol.as_ptr())
        })
        .to_str()
        .unwrap()
        .to_owned(),
        hall_symbol: CString::from(unsafe { CStr::from_ptr(raw.hall_symbol.as_ptr()) })
            .to_str()
            .unwrap()
            .to_owned(),
        choice: CString::from(unsafe { CStr::from_ptr(raw.choice.as_ptr()) })
            .to_str()
            .unwrap()
            .to_owned(),
        transformation_matrix: raw.transformation_matrix,
        origin_shift: raw.origin_shift,
        n_operations: raw.n_operations as i32,
        rotations: unsafe {
            Vec::from_raw_parts(raw.rotations, n_operations, n_operations).into_boxed_slice()
        },
        translations: unsafe {
            Vec::from_raw_parts(raw.translations, n_operations, n_operations).into_boxed_slice()
        },
        n_atoms: raw.n_atoms as i32,
        wyckoffs: unsafe { Vec::from_raw_parts(raw.wyckoffs, n_atoms, n_atoms).into_boxed_slice() },
        site_symmetry_symbols: Vec::new().into_boxed_slice(), // TODO
        equivalent_atoms: unsafe {
            Vec::from_raw_parts(raw.equivalent_atoms, n_atoms, n_atoms).into_boxed_slice()
        },
        mapping_to_primitive: unsafe {
            Vec::from_raw_parts(raw.mapping_to_primitive, n_atoms, n_atoms).into_boxed_slice()
        },
        n_std_atoms: raw.n_std_atoms as i32,
        std_lattice: raw.std_lattice,
        std_types: unsafe {
            Vec::from_raw_parts(raw.std_types, n_std_atoms, n_std_atoms).into_boxed_slice()
        },
        std_positions: unsafe {
            Vec::from_raw_parts(raw.std_positions, n_std_atoms, n_std_atoms).into_boxed_slice()
        },
        std_rotation_matrix: raw.std_rotation_matrix,
        std_mapping_to_primitive: unsafe {
            Vec::from_raw_parts(raw.std_mapping_to_primitive, n_std_atoms, n_std_atoms)
                .into_boxed_slice()
        },
        pointgroup_symbol: CString::from(unsafe { CStr::from_ptr(raw.pointgroup_symbol.as_ptr()) })
            .to_str()
            .unwrap()
            .to_owned(),
    }
}

#[cfg(test)]
mod tests {
    use crate::dataset::Dataset;
    use crate::internal::{rutile_cell, validate_rutile_cell_dataset};

    #[test]
    fn new_dataset() {
        let mut cell = rutile_cell();
        let res = Dataset::new(&mut cell, 1e-5);
        validate_rutile_cell_dataset(&res);
    }

    #[test]
    fn new_dataset_with_hall_number() {
        let mut cell = rutile_cell();
        let hall_number = 419;
        let res = Dataset::with_hall_number(&mut cell, hall_number, 1e-5);
        validate_rutile_cell_dataset(&res)
    }
}
