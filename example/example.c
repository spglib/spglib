#include "spglib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static void test_spg_get_symmetry(void);
static void test_spg_get_symmetry_with_collinear_spin(void);
static void test_spg_get_multiplicity(void);
static void test_spg_find_primitive(void);
static void test_spg_get_international(void);
static void test_spg_get_schoenflies(void);
static void test_spg_refine_cell(void);
static void test_spg_get_dataset(void);
static void test_spg_get_ir_reciprocal_mesh(void);
static void test_spg_get_tetrahedra_relative_grid_address(void);
static int grid_address_to_index(int g[3], int mesh[3]);
static void mat_copy_matrix_d3(double a[3][3], double b[3][3]);
static double mat_get_determinant_d3(double a[3][3]);
static int mat_inverse_matrix_d3(double m[3][3],
				 double a[3][3],
				 const double precision);

int main(void)
{
  test_spg_find_primitive();
  test_spg_get_multiplicity();
  test_spg_get_symmetry();
  test_spg_get_symmetry_with_collinear_spin();
  test_spg_get_international();
  test_spg_get_schoenflies();
  test_spg_refine_cell();
  test_spg_get_dataset();
  test_spg_get_ir_reciprocal_mesh();
  /* test_spg_get_tetrahedra_relative_grid_address(); */

  return 0;
}

static void test_spg_find_primitive(void)
{
  double lattice[3][3] = { {4, 0, 0}, {0, 4, 0}, {0, 0, 4} };
  double position[][3] = {
    {0, 0, 0},
    {0.5, 0.5, 0.5}
  };
  int types[] = { 1, 1 };
  int i, num_atom = 2, num_primitive_atom;
  double symprec = 1e-5;
  
  /* lattice, position, and types are overwirtten. */
  printf("*** Example of spg_find_primitive (BCC unitcell --> primitive) ***:\n");
  num_primitive_atom = spg_find_primitive(lattice, position, types, num_atom, symprec);
  if ( num_primitive_atom == 0 ) {
    printf("Primitive cell was not found.\n");
  } else { 
    printf("Lattice parameter:\n");
    for (i = 0; i < 3; i++) {
      printf("%f %f %f\n", lattice[0][i], lattice[1][i], lattice[2][i]);
    }
    printf("Atomic positions:\n");
    for (i=0; i<num_primitive_atom; i++) {
      printf("%d: %f %f %f\n", types[i], position[i][0], position[i][1],
	     position[i][2]);
    }
  }
}

static void test_spg_refine_cell(void)
{
  double lattice[3][3] = { {0, 2, 2}, {2, 0, 2}, {2, 2, 0} };

  /* 4 times larger memory space must be prepared. */
  double position[4][3];
  int types[4];
  
  int i, num_atom_bravais, num_atom = 1;
  double symprec = 1e-5;

  position[0][0] = 0;
  position[0][1] = 0;
  position[0][2] = 0;
  types[0] = 1;
  
  /* lattice, position, and types are overwirtten. */
  printf("*** Example of spg_refine_cell ***:\n");
  num_atom_bravais = spg_refine_cell( lattice,
				      position,
				      types,
				      num_atom,
				      symprec );
  printf("Lattice parameter:\n");
  for ( i = 0; i < 3; i++ ) {
    printf("%f %f %f\n", lattice[0][i], lattice[1][i], lattice[2][i]);
  }
  printf("Atomic positions:\n");
  for ( i = 0; i<num_atom_bravais; i++ ) {
    printf("%d: %f %f %f\n", types[i], position[i][0], position[i][1],
	   position[i][2]);
  }
}

static void test_spg_get_international(void)
{
  double lattice[3][3] = {{4,0,0},{0,4,0},{0,0,3}};
  double position[][3] =
    {
      {0,0,0},
      {0.5,0.5,0.5},
      {0.3,0.3,0},
      {0.7,0.7,0},
      {0.2,0.8,0.5},
      {0.8,0.2,0.5},
    };
  int types[] = {1,1,2,2,2,2};
  int num_spg, num_atom = 6;
  char symbol[21];
  
  num_spg = spg_get_international(symbol, lattice, position, types, num_atom, 1e-5);
  printf("*** Example of spg_get_international ***:\n");
  if ( num_spg > 0 ) {
    printf("%s (%d)\n", symbol, num_spg);
  } else {
    printf("Space group could not be found.\n");
  }
}

static void test_spg_get_schoenflies(void)
{
  double lattice[3][3] = {{4,0,0},{0,4,0},{0,0,3}};
  double position[][3] =
    {
      {0,0,0},
      {0.5,0.5,0.5},
      {0.3,0.3,0},
      {0.7,0.7,0},
      {0.2,0.8,0.5},
      {0.8,0.2,0.5},
    };
  int types[] = {1,1,2,2,2,2};
  int num_atom = 6;
  char symbol[7];
  
  spg_get_schoenflies(symbol, lattice, position, types, num_atom, 1e-5);
  printf("*** Example of spg_get_schoenflies ***:\n");
  printf("Schoenflies: %s\n", symbol);
}

static void test_spg_get_multiplicity(void)
{
  double lattice[3][3] = { {4, 0, 0}, {0, 4, 0}, {0, 0, 4} };
  double position[][3] = {
    {0, 0, 0},
    {0.5, 0.5, 0.5}
  };
  int types[] = { 1, 2 };
  int num_atom = 2;
  int size;

  size = spg_get_multiplicity(lattice, position, types, num_atom, 1e-5);

  printf("*** Example of spg_get_multiplicity ***:\n");
  printf("Number of symmetry operations: %d\n", size);
}


static void test_spg_get_symmetry(void)
{
  double lattice[3][3] = {{4,0,0},{0,4,0},{0,0,3}};
  double position[][3] =
    {
      {0,0,0},
      {0.5,0.5,0.25},
      {0.3,0.3,0},
      {0.7,0.7,0},
      {0.2,0.8,0.25},
      {0.8,0.2,0.25},
      {0,0,0.5},
      {0.5,0.5,0.75},
      {0.3,0.3,0.5},
      {0.7,0.7,0.5},
      {0.2,0.8,0.75},
      {0.8,0.2,0.75}
    };
  int types[] = {1,1,2,2,2,2,1,1,2,2,2,2};
  int num_atom = 12;
  int max_size = 50;
  int i, j, size;
  int rotation[max_size][3][3];
  double translation[max_size][3];

  double origin_shift[3] = { 0.1, 0.1, 0 };
  for ( i = 0; i < num_atom; i++ ) {
    for ( j = 0; j < 3; j++ ) {
      position[i][j] += origin_shift[j];
    }
  }

  printf("*** Example of spg_get_symmetry (Rutile two unit cells) ***:\n");
  size = spg_get_symmetry( rotation,
			   translation,
			   max_size,
			   lattice,
			   position,
			   types,
			   num_atom,
			   1e-5 );
  for (i = 0; i < size; i++) {
    printf("--- %d ---\n", i + 1);
    for (j = 0; j < 3; j++)
      printf("%2d %2d %2d\n", rotation[i][j][0], rotation[i][j][1], rotation[i][j][2]);
    printf("%f %f %f\n", translation[i][0], translation[i][1],
	   translation[i][2]);
  }

}

static void test_spg_get_symmetry_with_collinear_spin(void) {
  double lattice[3][3] = {{4,0,0},{0,4,0},{0,0,4}};
  double position[][3] =
    {
      {0,0,0},
      {0.5,0.5,0.5}
    };	
  int types[] = { 1, 1 };
  double spins[2];
  int num_atom = 2;
  int max_size = 300;
  int i, j, size;
  int rotation[max_size][3][3];
  double translation[max_size][3];

  printf("*** Example of spg_get_symmetry_with_spin (BCC ferro) ***:\n");
  spins[0] = 1;
  spins[1] = 1;
  size = spg_get_symmetry_with_collinear_spin( rotation,
					       translation,
					       max_size,
					       lattice,
					       position,
					       types,
					       spins,
					       num_atom,
					       1e-5 );
  for (i = 0; i < size; i++) {
    printf("--- %d ---\n", i + 1);
    for (j = 0; j < 3; j++)
      printf("%2d %2d %2d\n", rotation[i][j][0], rotation[i][j][1], rotation[i][j][2]);
    printf("%f %f %f\n", translation[i][0], translation[i][1],
           translation[i][2]);
  }

  printf("*** Example of spg_get_symmetry_with_spin (BCC antiferro) ***:\n");
  spins[0] = 1;
  spins[1] = -1;
  size = spg_get_symmetry_with_collinear_spin( rotation,
					       translation,
					       max_size,
					       lattice,
					       position,
					       types,
					       spins,
					       num_atom,
					       1e-5 );
  for (i = 0; i < size; i++) {
    printf("--- %d ---\n", i + 1);
    for (j = 0; j < 3; j++)
      printf("%2d %2d %2d\n", rotation[i][j][0], rotation[i][j][1], rotation[i][j][2]);
    printf("%f %f %f\n", translation[i][0], translation[i][1],
           translation[i][2]);
  }

  printf("*** Example of spg_get_symmetry_with_spin (BCC broken spin) ***:\n");
  spins[0] = 1;
  spins[1] = 2;
  size = spg_get_symmetry_with_collinear_spin( rotation,
					       translation,
					       max_size,
					       lattice,
					       position,
					       types,
					       spins,
					       num_atom,
					       1e-5 );
  for (i = 0; i < size; i++) {
    printf("--- %d ---\n", i + 1);
    for (j = 0; j < 3; j++)
      printf("%2d %2d %2d\n", rotation[i][j][0], rotation[i][j][1], rotation[i][j][2]);
    printf("%f %f %f\n", translation[i][0], translation[i][1],
           translation[i][2]);
  }
}


static void test_spg_get_dataset(void)
{
  SpglibDataset *dataset;
  double lattice[3][3] = {{4,0,0},{0,4,0},{0,0,3}};
  double origin_shift[3] = { 0.1, 0.1, 0 };
  double position[][3] =
    {
      {0,0,0},
      {0.5,0.5,0.25},
      {0.3,0.3,0},
      {0.7,0.7,0},
      {0.2,0.8,0.25},
      {0.8,0.2,0.25},
      {0,0,0.5},
      {0.5,0.5,0.75},
      {0.3,0.3,0.5},
      {0.7,0.7,0.5},
      {0.2,0.8,0.75},
      {0.8,0.2,0.75}
    };
  int types[] = {1,1,2,2,2,2,1,1,2,2,2,2};
  int num_atom = 12;
  int i, j, size;
  const char *wl = "abcdefghijklmnopqrstuvwxyz";

  for ( i = 0; i < num_atom; i++ ) {
    for ( j = 0; j < 3; j++ ) {
      position[i][j] += origin_shift[j];
    }
  }

  printf("*** Example of spg_get_dataset (Rutile two unit cells) ***:\n");
  dataset = spg_get_dataset( lattice,
			     position,
			     types,
			     num_atom,
			     1e-5 );
  
  printf("International: %s (%d)\n", dataset->international_symbol, dataset->spacegroup_number );
  printf("Hall symbol:   %s\n", dataset->hall_symbol );
  printf("Transformation matrix:\n");
  for ( i = 0; i < 3; i++ ) {
    printf("%f %f %f\n",
	   dataset->transformation_matrix[i][0],
	   dataset->transformation_matrix[i][1],
	   dataset->transformation_matrix[i][2]);
  }
  printf("Wyckoff letters:\n");
  for ( i = 0; i < dataset->n_atoms; i++ ) {
    printf("%c ", wl[dataset->wyckoffs[i]]);
  }
  printf("\n");
  printf("Equivalent atoms:\n");
  for ( i = 0; i < dataset->n_atoms; i++ ) {
    printf("%d ", dataset->equivalent_atoms[i]);
  }
  printf("\n");
  
  for ( i = 0; i < dataset->n_operations; i++ ) {
    printf("--- %d ---\n", i + 1);
    for ( j = 0; j < 3; j++ ) {
      printf("%2d %2d %2d\n",
	     dataset->rotations[i][j][0],
	     dataset->rotations[i][j][1],
	     dataset->rotations[i][j][2]);
    }
    printf("%f %f %f\n",
	   dataset->translations[i][0],
	   dataset->translations[i][1],
	   dataset->translations[i][2]);
  }

  spg_free_dataset( dataset );

}

static void test_spg_get_ir_reciprocal_mesh(void)
{
  double lattice[3][3] = {{4,0,0},{0,4,0},{0,0,3}};
  double position[][3] =
    {
      {0,0,0},
      {0.5,0.5,0.5},
      {0.3,0.3,0},
      {0.7,0.7,0},
      {0.2,0.8,0.5},
      {0.8,0.2,0.5},
    };
  int types[] = {1,1,2,2,2,2};
  int num_atom = 6;
  int m = 100;
  int mesh[] = {m, m, m};
  int is_shift[] = {1, 1, 1};
  int grid_address[m * m * m][3];
  int grid_mapping_table[m * m * m];

  printf("*** Example of spg_get_ir_reciprocal_mesh of Rutile structure ***:\n");

  int num_ir = spg_get_ir_reciprocal_mesh(grid_address,
					  grid_mapping_table,
					  mesh,
					  is_shift,
					  1,
					  lattice,
					  position,
					  types,
					  num_atom,
					  1e-5);

  printf("Number of irreducible k-points of Rutile with\n");
  printf("100x100x100 Monkhorst-Pack mesh is %d.\n", num_ir);
}

/* frequency.dat is made to decompress frequency.dat.bz2. */
/* The values in this file are the phonon frequencies of NaCl */
/* with 80x80x80 mesh. Calculation was done with reducing */
/* k-points to the irreducible k-points. */
static void test_spg_get_tetrahedra_relative_grid_address(void)
{
  int i, j, k, l, q, r;

  /* NaCl 20x20x20 mesh */
  double lattice[3][3] = {
    {0.000000000000000, 2.845150738087836, 2.845150738087836},
    {2.845150738087836, 0.000000000000000, 2.845150738087836},
    {2.845150738087836, 2.845150738087836, 0.000000000000000}
  };
  double position[][3] =
    {{0, 0, 0},
     {0.5, 0.5, 0.5}};
  int types[] = {1, 2};
  int num_atom = 2;
  int m = 80;
  int mesh[] = {m, m, m};
  int is_shift[] = {0, 0, 0};
  int grid_address[m * m * m][3];
  int grid_mapping_table[m * m * m];
  int weights[m * m * m];
  int num_ir = spg_get_ir_reciprocal_mesh(grid_address,
					  grid_mapping_table,
					  mesh,
					  is_shift,
					  1,
					  lattice,
					  position,
					  types,
					  num_atom,
					  1e-5);
  int ir_gp[num_ir];
  int ir_weights[num_ir];
  int gp_ir_index[m * m * m];
  
  for (i = 0; i < m * m * m; i++) {
    weights[i] = 0;
  }

  for (i = 0; i < m * m * m; i++) {
    weights[grid_mapping_table[i]]++;
  }

  j = 0;
  for (i = 0; i < m * m * m; i++) {
    if (weights[i] != 0) {
      ir_gp[j] = i;
      ir_weights[j] = weights[i];
      gp_ir_index[i] = j;
      j++;
    } else {
      gp_ir_index[i] = gp_ir_index[grid_mapping_table[i]];
    }
  }

  int relative_grid_address[24][4][3];
  double rec_lat[3][3];

  printf("# Number of irreducible k-points: %d\n", num_ir);
  
  mat_inverse_matrix_d3(rec_lat, lattice, 1e-5);
  spg_get_tetrahedra_relative_grid_address(relative_grid_address, rec_lat);
					     
  /* for (i = 0; i < 24; i++) { */
  /*   for (j = 0; j < 4; j++) { */
  /*     printf("[%2d %2d %2d] ", */
  /* 	     relative_grid_address[i][j][0], */
  /* 	     relative_grid_address[i][j][1], */
  /* 	     relative_grid_address[i][j][2]); */
  /*   } */
  /*   printf("\n"); */
  /* } */

  FILE *fp;
  fp = fopen("frequency.dat", "r");
  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  double frequency[num_ir * num_atom * 3];

  for (i = 0; i < num_ir * num_atom * 3; i++) {
    read = getline(&line, &len, fp);
    if (read == -1) {
      break;
    }
    frequency[i] = strtod(line, NULL);
  }

  double max_f, min_f;
  max_f = frequency[0];
  min_f = frequency[0];
  for (i = 0; i < num_ir * num_atom * 3; i++) {
    if (max_f < frequency[i]) {
      max_f = frequency[i];
    }
    if (min_f > frequency[i]) {
      min_f = frequency[i];
    }
  }
  
  printf("# Number of frequencies: %d\n", i);
  
  double t_omegas[24][4];
  int g_addr[3];
  int gp;
  int num_freqs = 401;
  double dos[num_freqs];
  double omegas[num_freqs];
  double iw;

#pragma omp parallel for private(j, k, l, q, r, g_addr, gp, t_omegas, iw)
  for (i = 0; i < num_freqs; i++) {
    dos[i] = 0;
    omegas[i] = (max_f - min_f) / (num_freqs - 1) * i;
    for (j = 0; j < num_ir;  j++) {
      for (k = 0; k < num_atom * 3; k++) {
	for (l = 0; l < 24; l++) {
	  for (q = 0; q < 4; q++) {
	    for (r = 0; r < 3; r++) {
	      g_addr[r] = grid_address[ir_gp[j]][r] +
		relative_grid_address[l][q][r];
	    }
	    gp = grid_address_to_index(g_addr, mesh);
	    t_omegas[l][q] = frequency[gp_ir_index[gp] * num_atom * 3 + k];
	  }
	}
	iw = spg_get_tetrahedra_integration_weight(omegas[i], t_omegas);
	dos[i] += iw * ir_weights[j];
      }
    }
  }

  for (i = 0; i < num_freqs; i++) {
    printf("%f %f\n", omegas[i], dos[i] / m / m / m);
  }
    
}

static int grid_address_to_index(int g[3], int mesh[3])
{
  int i;
  int gm[3];

  for (i = 0; i < 3; i++) {
    gm[i] = g[i] % mesh[i];
    if (gm[i] < 0) {
      gm[i] += mesh[i];
    }
  }
  return (gm[0] + gm[1] * mesh[0] + gm[2] * mesh[0] * mesh[1]);
}

static void mat_copy_matrix_d3(double a[3][3], double b[3][3])
{
  a[0][0] = b[0][0];
  a[0][1] = b[0][1];
  a[0][2] = b[0][2];
  a[1][0] = b[1][0];
  a[1][1] = b[1][1];
  a[1][2] = b[1][2];
  a[2][0] = b[2][0];
  a[2][1] = b[2][1];
  a[2][2] = b[2][2];
}

static double mat_get_determinant_d3(double a[3][3])
{
  return a[0][0] * (a[1][1] * a[2][2] - a[1][2] * a[2][1])
    + a[0][1] * (a[1][2] * a[2][0] - a[1][0] * a[2][2])
    + a[0][2] * (a[1][0] * a[2][1] - a[1][1] * a[2][0]);
}

static int mat_inverse_matrix_d3(double m[3][3],
				 double a[3][3],
				 const double precision)
{
  double det;
  double c[3][3];
  det = mat_get_determinant_d3(a);

  c[0][0] = (a[1][1] * a[2][2] - a[1][2] * a[2][1]) / det;
  c[1][0] = (a[1][2] * a[2][0] - a[1][0] * a[2][2]) / det;
  c[2][0] = (a[1][0] * a[2][1] - a[1][1] * a[2][0]) / det;
  c[0][1] = (a[2][1] * a[0][2] - a[2][2] * a[0][1]) / det;
  c[1][1] = (a[2][2] * a[0][0] - a[2][0] * a[0][2]) / det;
  c[2][1] = (a[2][0] * a[0][1] - a[2][1] * a[0][0]) / det;
  c[0][2] = (a[0][1] * a[1][2] - a[0][2] * a[1][1]) / det;
  c[1][2] = (a[0][2] * a[1][0] - a[0][0] * a[1][2]) / det;
  c[2][2] = (a[0][0] * a[1][1] - a[0][1] * a[1][0]) / det;
  mat_copy_matrix_d3(m, c);
  return 1;
}
