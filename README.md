# starsh-interfaces
Playing around with interfaces to Stars-H.

## `STARSH_ssdata` struct:

``` cpp
typedef struct starsh_ssdata
//! Structure for Spatial Statistics problems.
{
    STARSH_particles particles;
    //!< Particles.
    //char dtype;
    ////!< Precision of each matrix element (double, single etc).
    double beta;
    //!< Characteristical length of covariance.
    double nu;
    //!< Order of Mat&eacute;rn kernel.
    double noise;
    //!< Noise and regularization parameter.
    double sigma;
    //!< Variance.
    double nu2;
    //!< Order of Mat&eacute;rn kernel for the second variable (in the parsimonious bivariate case).
    double sigma2;
    //!< Variance for the second variable (in the parsimonious bivariate case).
   double corr;
    //!< spatial range parameter (define the correlation between the two variables in the parsimonious bivariate case).
} STARSH_ssdata;
```

Store data for spatial problems such as 2D and 3D exponential geometry.

## `STARSH_particles` struct:

``` cpp
typedef struct starsh_particles
//! Structure for general N-body problems.
{
    STARSH_int count;
    //!< Number of particles.
    int ndim;
    //!< Dimensionality of space.
    double *point;
    //!< Coordinates of particles.
} STARSH_particles;
```

This struct stores the co-ordinates of the particles. The co-ords are stored in the 1D array
`point`.
