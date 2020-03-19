//! Full description of a crystallographic spacegroup

use spglib_sys as ffi;
use std::ffi::{CStr, CString};
use std::os::raw::c_int;

/// The full description of a given spacegroup
#[derive(Debug, Clone)]
pub struct Spacegroup {
    /// Space group number as defined in the International Tables for Crystallography
    pub number: i32,
    /// Abbreviated Hermann-Mauguin notation spacegroup symbol
    pub international_short: String,
    /// Full Hermann-Mauguin notation spacegroup symbol
    pub international_full: String,
    /// Full Hermann-Mauguin notation spacegroup symbol
    pub international: String,
    /// Schoenflies notation spacegroup symbol
    pub schoenflies: String,
    /// Hall symbol
    pub hall_symbol: String,
    /// Information on unique axis, settings, or cell choices
    pub choice: String,
    /// Full Hermann-Mauguin notation pointgroup symbol
    pub pointgroup_international: String,
    /// Schoenflies notation pointgroup symbol
    pub pointgroup_schoenflies: String,
    /// Arithmetic notation crystal class number
    pub arithmetic_crystal_class_number: i32,
    /// Arithmetic notation crystal class symbol
    pub arithmetic_crystal_class_symbol: String,
}

impl Spacegroup {
    /// Returns a Spacegroup initialized from a given hall number
    ///
    /// # Examples
    ///
    /// Get the spacegroup for a BCC cell (hall number 529)
    ///
    /// ```
    /// # use spglib::spacegroup::Spacegroup;
    /// let group = Spacegroup::new(529);
    /// assert_eq!(
    ///     group.international_short,
    ///     "Im-3m"
    /// );
    /// ```
    pub fn new(hall_number: i32) -> Spacegroup {
        let res = unsafe { ffi::spg_get_spacegroup_type(hall_number as c_int) };
        Spacegroup {
            number: res.number as i32,
            international_short: CString::from(unsafe {
                CStr::from_ptr(res.international_short.as_ptr())
            })
            .to_str()
            .unwrap()
            .to_owned(),
            international_full: CString::from(unsafe {
                CStr::from_ptr(res.international_full.as_ptr())
            })
            .to_str()
            .unwrap()
            .to_owned(),
            international: CString::from(unsafe { CStr::from_ptr(res.international.as_ptr()) })
                .to_str()
                .unwrap()
                .to_owned(),
            schoenflies: CString::from(unsafe { CStr::from_ptr(res.schoenflies.as_ptr()) })
                .to_str()
                .unwrap()
                .to_owned(),
            hall_symbol: CString::from(unsafe { CStr::from_ptr(res.hall_symbol.as_ptr()) })
                .to_str()
                .unwrap()
                .to_owned(),
            choice: CString::from(unsafe { CStr::from_ptr(res.choice.as_ptr()) })
                .to_str()
                .unwrap()
                .to_owned(),
            pointgroup_international: CString::from(unsafe {
                CStr::from_ptr(res.pointgroup_international.as_ptr())
            })
            .to_str()
            .unwrap()
            .to_owned(),
            pointgroup_schoenflies: CString::from(unsafe {
                CStr::from_ptr(res.pointgroup_schoenflies.as_ptr())
            })
            .to_str()
            .unwrap()
            .to_owned(),
            arithmetic_crystal_class_number: res.arithmetic_crystal_class_number,
            arithmetic_crystal_class_symbol: CString::from(unsafe {
                CStr::from_ptr(res.arithmetic_crystal_class_symbol.as_ptr())
            })
            .to_str()
            .unwrap()
            .to_owned(),
        }
    }
}

#[cfg(test)]
mod tests {
    use crate::spacegroup::Spacegroup;
    #[test]
    fn new_spacegroup() {
        let res = Spacegroup::new(446);
        assert_eq!(res.number, 156);
        assert_eq!(res.international_short.as_str(), "P3m1");
        assert_eq!(res.international_full.as_str(), "P 3 m 1");
        assert_eq!(res.international.as_str(), "P 3 m 1");
        assert_eq!(res.schoenflies.as_str(), "C3v^1");
        assert_eq!(res.hall_symbol.as_str(), "P 3 -2\"");
        assert_eq!(res.choice.as_str(), "");
        assert_eq!(res.pointgroup_international.as_str(), "3m");
        assert_eq!(res.pointgroup_schoenflies.as_str(), "C3v");
        assert_eq!(res.arithmetic_crystal_class_number, 45);
        assert_eq!(res.arithmetic_crystal_class_symbol.as_str(), "3m1P");
    }
}
