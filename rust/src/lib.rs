#![deny(missing_docs)]
#![deny(missing_debug_implementations)]

//! The `spglib` crate provides a Rust-y wrapper to [spglib](https://atztogo.github.io/spglib/), a C library for finding and handling crystal symmetries.

pub mod cell;
pub mod dataset;
mod internal;
pub mod spacegroup;
use spglib_sys as ffi;
use std::error;
use std::fmt;
use std::os::raw::c_int;

/// Returns the major version of spglib
pub fn major_version() -> i32 {
    unsafe { ffi::spg_get_major_version() }
}

/// Returns the minor version of spglib
pub fn minor_version() -> i32 {
    unsafe { ffi::spg_get_minor_version() }
}

/// Returns the micro version of spglib
pub fn micro_version() -> i32 {
    unsafe { ffi::spg_get_micro_version() }
}

/// Returns the hall number for a given set of symmetry operations
///
/// # Examples
///
/// Get the hall number for a BCC cell in a roundabout way
///
/// ```
/// use spglib as spg;
/// # use spglib::cell::Cell;
/// # use spglib::dataset::Dataset;
/// # let lattice = [[4., 0., 0.], [0., 4., 0.], [0., 0., 4.]];
/// # let positions = &[[0., 0., 0.], [0.5, 0.5, 0.5]];
/// # let types = &[1, 1];
/// # let mut bcc_cell = Cell::new(lattice, positions, types);
/// let dataset = Dataset::new(&mut bcc_cell, 1e-5);
/// let mut rotations = dataset.rotations.clone();
/// let mut translations = dataset.translations.clone();
/// let hall_number =
///     spg::hall_number_from_symmetry(&mut rotations, &mut translations, 1e-5);
/// assert_eq!(hall_number, dataset.hall_number);
/// ```
pub fn hall_number_from_symmetry(
    rotations: &mut [[[i32; 3]; 3]],
    translations: &mut [[f64; 3]],
    symprec: f64,
) -> i32 {
    let n_operations = rotations.len();
    unsafe {
        ffi::spg_get_hall_number_from_symmetry(
            rotations.as_ptr() as *mut [[c_int; 3]; 3],
            translations.as_ptr() as *mut [f64; 3],
            n_operations as c_int,
            symprec,
        )
    }
}

#[derive(Debug, Clone)]
/// Error raised when a cell cannot be standardized
pub struct StandardizeError;

impl fmt::Display for StandardizeError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "failed to standardize the cell")
    }
}

impl error::Error for StandardizeError {
    fn source(&self) -> Option<&(dyn error::Error + 'static)> {
        None
    }
}

#[derive(Debug, Clone)]
/// Error raised when a cell cannot be reduced with the Delaunay reduction scheme
pub struct DelaunayReduceError;

impl fmt::Display for DelaunayReduceError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "failed to reduce the cell")
    }
}

impl error::Error for DelaunayReduceError {
    fn source(&self) -> Option<&(dyn error::Error + 'static)> {
        None
    }
}

#[derive(Debug, Clone)]
/// Error raised when a cell cannot be reduced with the Niggli reduction scheme
pub struct NiggliReduceError;

impl fmt::Display for NiggliReduceError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "failed to reduce the cell")
    }
}

impl error::Error for NiggliReduceError {
    fn source(&self) -> Option<&(dyn error::Error + 'static)> {
        None
    }
}

#[cfg(test)]
mod tests {
    use crate as spg;
    use crate::dataset::Dataset;
    use crate::internal::rutile_cell;
    #[test]
    fn hall_number_from_symmetry() {
        let mut cell = rutile_cell();
        let symprec = 1e-5;
        let dataset = Dataset::new(&mut cell, symprec);
        let mut rotations = dataset.rotations.clone();
        let mut translations = dataset.translations.clone();
        let hall_number =
            spg::hall_number_from_symmetry(&mut rotations, &mut translations, symprec);
        assert_eq!(hall_number, dataset.hall_number);
    }
}
