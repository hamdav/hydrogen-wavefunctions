#pragma once

#include <cmath> //associated functions and stuff
#include <complex>
#include <iostream>

using complexd_t = std::complex<double>;

struct Dims
{
    int r;
    int theta;
    int phi;
};

int prod(int start, int end);
complexd_t Ylm(int l, int m, double theta, double phi);
complexd_t Rnl(int n, int l, double r);
complexd_t psi_nlm(int n, int l, int m, double r, double theta, double phi);
void linspace(double start, double stop, int n, double* array);
complexd_t* psi_arr(int n, int l, int m, Dims dims);
double *abs_psi_sq(int n, int l, int m, Dims dims);
double *get_colors(int n, int l, int m, double phi_c, double theta_c, double xmin, double xmax, double ymin, double ymax, int n_x, int n_y);
