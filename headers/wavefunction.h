#pragma once

#include <cmath> //associated functions and stuff
#include <complex>
#include <iostream>
#include <numeric>

using complexd_t = std::complex<double>;

struct Dims
{
    int r;
    int theta;
    int phi;
};

double fracfac(int i, int j);
int prod(int i, int j);
complexd_t Ylm(int l, int m, double theta, double phi);
complexd_t Rnl(int n, int l, double r);
complexd_t psi_nlm(int n, int l, int m, double r, double theta, double phi);
void linspace(double start, double stop, int n, double* array);
complexd_t* psi_arr(int n, int l, int m, Dims dims);
double *abs_psi_sq(int n, int l, int m, Dims dims);
void convert_to_basis(double v[3], double e1[3], double e2[3], double e3[3], double res[3]);
double *get_colors(int n, int l, int m, double phi_c, double theta_c, double xmin, double xmax, double ymin, double ymax, int n_x, int n_y);
double *get_colors2_electric_boogaloo(int n, int l, int m, double phi_c, double theta_c, double xmin, double xmax, double ymin, double ymax, double zmax, int n_x, int n_y, int n_z);
