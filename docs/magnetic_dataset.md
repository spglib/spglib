(magnetic_spglib_dataset)=

# Magnetic Spglib dataset (Experimental)

**New at version 2.0**

The dataset is accessible through the C-structure given by

```c
typedef struct {
    /* Magnetic space-group type */
    int uni_number;
    int msg_type;
    int hall_number;
    int tensor_rank;
    /* Magnetic symmetry operations */
    int n_operations;
    int (*rotations)[3][3];
    double (*translations)[3];
    int *time_reversals;
    /* Equivalent atoms */
    int n_atoms;
    int *equivalent_atoms;
    /* Transformation to standardized setting */
    double transformation_matrix[3][3];
    double origin_shift[3];
    /* Standardized crystal structure */
    int n_std_atoms;
    double std_lattice[3][3];
    int *std_types;
    double (*std_positions)[3];
    double *std_tensors;
    double std_rotation_matrix[3][3];
    /* Intermediate datum in symmetry search */
    double primitive_lattice[3][3];
} SpglibMagneticDataset;
```

## Magnetic space-group type

### `uni_number`

The serial number from 1 to 1651 of UNI or BNS symbols.

### `msg_type`

Magnetic space groups (MSG) is classified by its family space group (FSG)
and maximal space subgroup (XSG).
FSG is a non-magnetic space group obtained by ignoring time-reversal term in MSG.
XSG is a space group obtained by picking out non time-reversal operations in MSG.

- `msg_type==1` (type-I): MSG, XSG, FSG are all isomorphic.
- `msg_type==2` (type-II): XSG and FSG are isomorphic, and MSG is generated
  from XSG and pure time reversal operations
- `msg_type==3` (type-III): XSG is a proper subgroup of MSG with isomorphic
  translational subgroups.
- `msg_type==4` (type-IV): XSG is a proper subgroup of MSG with isomorphic
  point group.

### `hall_number`

For type-I, II, III, {ref}`hall number <spglib_dataset_hall_number>` of FSG; for type-IV, that of XSG

### `tensor_rank`

0 for collinear spins, 1 for non-collinear spins

## Magnetic symmetry operations

### `n_operations`

Number of magnetic symmetry operations.

### `rotations`

Rotation (matrix) parts of symmetry operations

### `translations`

Translation (vector) parts of symmetry operations

### `time_reversals`

Time reversal part of magnetic symmetry operations.
1 indicates time reversal operation, and 0 indicates an ordinary operation.

## Symmetrically equivalent atoms

### `n_atoms` and `equivalent_atoms`

See {ref}`spglib_dataset_equivalent_atoms`.

## Transformation to standardized setting

### `transformation_matrix` and `origin_shift`

See {ref}`dataset_origin_shift_and_transformation`.

## Standardized crystal structure after idealization

### `n_std_atoms`, `std_lattice`, `std_types`, `std_positions`, and `std_rotation_matrix`

See {ref}`dataset_idealized_cell`.

### `std_tensors`

Site tensors of the standardized crystal structure {ref}`idealization <def_idealize_cell>`.

## Intermediate data in symmetry search

### `primitive_lattice`

See {ref}`spglib_dataset_primitive_lattice`.
`primitive_lattice` generates pure translations without time reversals.
Thus, for type-IV magnetic space groups, a unit cell spanned by `primitive_lattice` contains an anti-translation lattice point.
