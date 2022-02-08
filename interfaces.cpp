/* System */
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>

#include <starsh.h>
#include <starsh-randtlr.h>
#include <starsh-electrodynamics.h>
#include <starsh-spatial.h>

// Interfaces from https://github.com/bosilca/hicma-x-dev/blob/lei_tlr_release_based_on_sparse_branch/hicma_parsec/hicma_parsec.c

// kernel func args:
// 2 : 2D exp
int main(int argc, char **argv) {
  STARSH_int N = atol(argv[1]);
  int kernel_func = atoi(argv[2]);
  std::string kernel_name(argv[3]);

  int ndim;
  STARSH_kernel *s_kernel;
  // This array will contain physical data that will be put into in by the
  // _generate() functions. For a 2D function, this will be a 2D array.
  // Each ordered array of points will contain the co-ordinates of the
  // data at a given point.
  void *starsh_data;

  // This array contains the index of the data. Internally in stars-h, it is used for
  // for referring to the specific co-ordinate point for a given place in the array.
  // Typically, it can just be a (0..N) range.
  STARSH_int * starsh_index;

  double beta = 0.1;
  double nu = 0.5;     //in matern, nu=0.5 exp (half smooth), nu=inf sqexp (inifinetly smooth)
  double noise = 1.e-1;
  double sigma = 1.0;

  STARSH_int count;

  // Generate particles using the function starsh_particles_generate_uniform.
  enum STARSH_PARTICLES_PLACEMENT place = STARSH_PARTICLES_UNIFORM;

  /* Set seed */
  srand(0);

  double **coords;

  starsh_index = (STARSH_int*)malloc( N * sizeof(STARSH_int) );
  for(STARSH_int i = 0; i < N; ++i)
    starsh_index[i] = i;

  if (kernel_func == 1) {
    double wave_k = 50;           // default value from hicma_parsec.c
    double add_diag = 0.0;        // default value from hicma_parsec.c
    ndim = 2;
    coords = (double**)malloc(ndim * sizeof(double*));

    // Kernel from an electrodynamics application.
    s_kernel = starsh_eddata_block_sin_kernel_2d;
    starsh_eddata_generate((STARSH_eddata **)&starsh_data, N, ndim, wave_k,
                           add_diag, place);

    coords[0] = ((STARSH_eddata*)starsh_data)->particles.point;
    count = ((STARSH_eddata*)starsh_data)->particles.count;
  }

  if (kernel_func == 2) {
    ndim = 2;
    coords = (double**)malloc(ndim * sizeof(double*));

    s_kernel = starsh_ssdata_block_sqrexp_kernel_2d;

    // This function will generate a 2D spatial geometry. Grid.
    starsh_ssdata_generate((STARSH_ssdata**)&starsh_data, N, ndim, beta,
                           nu, noise, place, sigma);
    coords[0] = ((STARSH_ssdata*)starsh_data)->particles.point;

    // The generated data can be used from the starsh_data variable.
    count = ((STARSH_ssdata*)starsh_data)->particles.count;
  }

  if (kernel_func == 3) {
    ndim = 3;
    coords = (double**)malloc(ndim * sizeof(double*));

    // This function will generate a 3D spatial geometry. Grid.
    s_kernel = starsh_ssdata_block_sqrexp_kernel_3d;
    starsh_ssdata_generate((STARSH_ssdata **)&starsh_data, N, ndim,
                           beta, nu, noise, place, sigma);

    coords[0] = ((STARSH_ssdata*)starsh_data)->particles.point;

    // The generated data can be used from the starsh_data variable.
    count = ((STARSH_ssdata*)starsh_data)->particles.count;
  }

  // Store offsets of the co-ordinates of each dimension.
  for (int k = 1; k < ndim; ++k) {
    coords[k] = coords[0] + k * count;
  }

  std::ofstream file;
  file.open("stars_h_" + kernel_name + ".csv", std::ios::app | std::ios::out);


  file << "x,y,\n";
  for (int i =0; i < N; ++i) {
    for (int k = 0; k < ndim; ++k) {
      file << coords[k][starsh_index[i]] << ",";
    }
    file << std::endl;
  }

  file.close();

  free(starsh_index);
}
