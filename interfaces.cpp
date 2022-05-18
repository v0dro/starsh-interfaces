/* System */
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>

extern "C" {
#include <starsh.h>
#include <starsh-randtlr.h>
#include <starsh-electrodynamics.h>
#include <starsh-spatial.h>
#include <starsh-fugaku_gc.h>
}
// Interfaces from https://github.com/bosilca/hicma-x-dev/blob/lei_tlr_release_based_on_sparse_branch/hicma_parsec/hicma_parsec.c

// kernel func args:
// 2 : 2D exp
double frob_norm(double* matrix, int nrows, int ncols, int row_start, int col_start, int stride) {
  double norm = 0.0;

  for (int i = 0; i < nrows; ++i) {
    for (int j = 0; j < ncols; ++j) {
      norm += pow(matrix[(row_start + i) + (col_start + j) * stride], 2);
    }
  }

  return std::sqrt(norm);
}

int main(int argc, char **argv) {
  STARSH_int N = atol(argv[1]);
  int kernel_func = atoi(argv[2]);
  std::string kernel_name(argv[3]);
  STARSH_int nb = atoi(argv[4]);

  STARSH_int ndim;
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

  if (kernel_func == 4) {
    ndim = 3;
    coords = (double**)malloc(ndim * sizeof(double*));

    // 3D spatial geometry for exponential kernel.
    s_kernel = starsh_ssdata_block_exp_kernel_3d;
    starsh_ssdata_generate((STARSH_ssdata **)&starsh_data, N, ndim,
                           beta, nu, noise, place, sigma);

    coords[0] = ((STARSH_ssdata*)starsh_data)->particles.point;

    // The generated data can be used from the starsh_data variable.
    count = ((STARSH_ssdata*)starsh_data)->particles.count;
  }

  if (kernel_func == 5) {
    ndim = 3;
    coords =  (double**)malloc(ndim * sizeof(double*));
    s_kernel = starsh_laplace_block_kernel;
    starsh_laplace_grid_generate((STARSH_laplace**)&starsh_data,
                                 N, ndim, 1e-8, place);
    coords[0] = ((STARSH_ssdata*)starsh_data)->particles.point;

    // The generated data can be used from the starsh_data variable.
    count = ((STARSH_ssdata*)starsh_data)->particles.count;
  }

  if (kernel_func == 6) {
    ndim = 3;
    coords =  (double**)malloc(ndim * sizeof(double*));
    STARSH_laplace* data = (STARSH_laplace*)malloc(sizeof(STARSH_laplace));

    data->N = N;
    data->PV = 1e-8;
    data->ndim = ndim;
    s_kernel = starsh_laplace_block_kernel;
    starsh_file_grid_read_kmeans("/groups/gca50014/md_data/57126x1.dat",
                                 &data->particles,
                                 N, ndim);
    coords[0] = ((STARSH_ssdata*)starsh_data)->particles.point;

    // The generated data can be used from the starsh_data variable.
    count = ((STARSH_ssdata*)starsh_data)->particles.count;
  }



  // Store offsets of the co-ordinates of each dimension.
  for (int k = 1; k < ndim; ++k) {
    coords[k] = coords[0] + k * count;
  }

  std::ofstream file;
  file.open("starsh.dat", std::ios::out);


  file << "x,y,z\n";
  // file << N << ndim;
  for (int i =0; i < N; ++i) {
    std::string comma("");
    for (int k = 0; k < ndim; ++k) {
      file << comma << coords[k][starsh_index[i]];
      // file << coords[k][starsh_index[i]];
      comma = ",";
    }
    file << std::endl;
  }

  file.close();

  // generate the matrix
  double *matrix = (double*)malloc(sizeof(double) * N * N);
  int nt = floor(N / 1024);
  int mt = floor(N / 1024);
  for (int i = 0; i < nt; ++i) {
    int mb = i != mt-1 ? 1024 : (N - mt * 1024);
    for (int j = 0; j < mt; ++j) {
      int nb = j != nt-1 ? 1024 : (N - nt * 1024);

      printf("i: %d mb: %d -- j: %d nb: %d\n", i, mb, j, nb);
      s_kernel(mb, nb, starsh_index + mb * i, starsh_index + nb * j,
               starsh_data, starsh_data, matrix, N);
    }


  }


  // int nblocks = N / nb;
  // for (int i = 0; i < nblocks; ++i) {
  //   for (int j = 0; j < nblocks; ++j) {
  //     std::cout << "<" << i << "," << j << "> :: " << frob_norm(matrix, nb, nb, nb * i, nb * j, N) << std::endl;
  //   }
  // }

  free(starsh_index);
}
