## Database generation for magnetic space group

### File description

Table data

- `magnetic_hall_symbols.yaml` List of Magnetic Hall symbols
- `msg_numbers.csb` conversion between BNS, OG, UNI, and Litvin numbers

Database generation scripts

- `make_hall_mapping.py`

  - `int magnetic_spacegroup_uni_mapping[][2]` and `int magnetic_spacegroup_hall_mapping[][2]`

- `make_mhall_db.py`

  - `int magnetic_symmetry_operations[]` and `int magnetic_spacegroup_operation_index[][18][2]`

- `make_msgtype_db.py`

  - `MagneticSpacegroupType magnetic_spacegroup_types[]`

- `make_alternative_settings.py`

  - `python make_alternative_settings.py` pre-computes transformations to standard descriptions and save results in conjugators.json
  - `python make_alternative_settings.py --dump` prints `int alternative_transformations[][18][7]` by reading conjugators.json
