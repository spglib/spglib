#include "spglib.h"
#include "tetrahedron_method.h"
#include <stdio.h>
#include <stdlib.h>

static void test_tetrahedron_method(void);
static void mat_copy_matrix_d3(double a[3][3], double b[3][3]);
static double mat_get_determinant_d3(double a[3][3]);
static int mat_inverse_matrix_d3(double m[3][3],
				 double a[3][3],
				 const double precision);

int main(void)
{
  test_tetrahedron_method();

  return 0;
}

/* frequency.dat is in the example directory. */
/* The values in this file are the phonon frequencies of NaCl */
/* with 20x20x20 mesh. Calculation was done with reducing */
/* k-points to the irreducible k-points using phonopy. */
/* (http://phonopy.sf.net/) */
static void test_tetrahedron_method(void)
{
  printf("*** Example of tetrahedron method of NaCl to calculate DOS ***:\n");
  printf("Read data from frequency.dat and write DOS to dos.dat.\n");

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
  int m = 20;
  int mesh[] = {m, m, m};
  int num_gp = mesh[0] * mesh[1] * mesh[2];
  int is_shift[] = {0, 0, 0};
  int grid_address[num_gp][3];
  int grid_mapping_table[num_gp];
  int weights[num_gp];
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
  int gp_ir_index[num_gp];
  int relative_grid_address[24][4][3];
  double rec_lat[3][3];
  FILE *fp;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  double frequency[num_ir * num_atom * 3];
  double max_f, min_f;
  double t_omegas[24][4];
  int g_addr[3];
  int gp;
  int num_freqs = 201;
  double dos[num_freqs];
  double integral_dos[num_freqs];
  double omegas[num_freqs];
  double iw;
  
  for (i = 0; i < num_gp; i++) {
    weights[i] = 0;
  }

  for (i = 0; i < num_gp; i++) {
    weights[grid_mapping_table[i]]++;
  }

  j = 0;
  for (i = 0; i < num_gp; i++) {
    if (weights[i] != 0) {
      ir_gp[j] = i;
      ir_weights[j] = weights[i];
      gp_ir_index[i] = j;
      j++;
    } else {
      gp_ir_index[i] = gp_ir_index[grid_mapping_table[i]];
    }
  }

  printf("Number of irreducible k-points: %d\n", num_ir);
  
  mat_inverse_matrix_d3(rec_lat, lattice, 1e-5);
  thm_get_relative_grid_address(relative_grid_address, rec_lat);

  /* for (i = 0; i < 24; i++) { */
  /*   for (j = 0; j < 4; j++) { */
  /*     printf("[%2d %2d %2d] ", */
  /* 	     relative_grid_address[i][j][0], */
  /* 	     relative_grid_address[i][j][1], */
  /* 	     relative_grid_address[i][j][2]); */
  /*   } */
  /*   printf("\n"); */
  /* } */

  fp = fopen("frequency.dat", "r");

  for (i = 0; i < num_ir * num_atom * 3; i++) {
    read = getline(&line, &len, fp);
    if (read == -1) {
      break;
    }
    frequency[i] = strtod(line, NULL);
  }

  fclose(fp);

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
  
  printf("Number of frequencies: %d\n", i);
  

#pragma omp parallel for private(j, k, l, q, r, g_addr, gp, t_omegas, iw)
  for (i = 0; i < num_freqs; i++) {
    dos[i] = 0;
    integral_dos[i] = 0;
    omegas[i] = min_f + (max_f - min_f) / (num_freqs - 1) * i;
    for (j = 0; j < num_ir;  j++) {
      for (k = 0; k < num_atom * 3; k++) {
	for (l = 0; l < 24; l++) {
	  for (q = 0; q < 4; q++) {
	    for (r = 0; r < 3; r++) {
	      g_addr[r] = grid_address[ir_gp[j]][r] +
		relative_grid_address[l][q][r];
	    }
	    gp = spg_get_grid_point_from_address(g_addr, mesh);
	    t_omegas[l][q] = frequency[gp_ir_index[gp] * num_atom * 3 + k];
	  }
	}
	iw = thm_get_integration_weight(omegas[i], t_omegas, 'J');
	dos[i] += iw * ir_weights[j];
	iw = thm_get_integration_weight(omegas[i], t_omegas, 'I');
	integral_dos[i] += iw * ir_weights[j];
      }
    }
  }

  fp = fopen("dos.dat", "w");

  for (i = 0; i < num_freqs; i++) {
    fprintf(fp, "%f %f\n", omegas[i], dos[i] / num_gp);
  }

  fprintf(fp, "\n\n");
  
  for (i = 0; i < num_freqs; i++) {
    fprintf(fp, "%f %f\n", omegas[i], integral_dos[i] / num_gp);
  }
    
  fclose(fp);
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

