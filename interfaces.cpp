/* System */
#include <cmath>
#include <cstdlib>
#include <iostream>

#include <starsh.h>
#include <starsh-randtlr.h>
#include <starsh-electrodynamics.h>
#include <starsh-spatial.h>

// Interfaces from https://github.com/bosilca/hicma-x-dev/blob/lei_tlr_release_based_on_sparse_branch/hicma_parsec/hicma_parsec.c

// kernel func args:
// 2 : 2D exp
int main(int argc, char **argv) {
  int N = atoi(argv[1]);
  int NB = atoi(argv[2]);
  int kernel_func = atoi(argv[3]);


  int ndim;
  STARSH_kernel *s_kernel;
  void *starsh_data;
  STARSH_int * starsh_index;

  double beta = 0.1;
  double nu = 0.5;     //in matern, nu=0.5 exp (half smooth), nu=inf sqexp (inifinetly smooth)
  double noise = 1.e-1;
  double sigma = 1.0;

  enum STARSH_PARTICLES_PLACEMENT place = STARSH_PARTICLES_UNIFORM;

  /* Set seed */
  srand(0);

  starsh_index = (STARSH_int*)malloc( N * sizeof(STARSH_int) );
  for(STARSH_int i = 0; i < N; ++i)
    starsh_index[i] = i;

  if (kernel_func == 2) {
    ndim = 2;
    s_kernel = starsh_ssdata_block_sqrexp_kernel_2d;
    starsh_ssdata_generate((STARSH_ssdata**)&starsh_data, N, ndim, beta,
                           nu, noise, place, sigma);
  }

  free(starsh_index);

}
