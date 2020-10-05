#include <cmath> //associated functions and stuff
#include <complex>
#include <iostream>

inline const double pi = 3.141592653589793;

using complexd_t = std::complex<double>;

// Returns the product of all integers 
// between start and end inclusive.
// If start > end it returns 1
int prod(int start, int end){
    int cumulative_product{1};
    for (int n = start; n <= end; n++){
        cumulative_product *= n;
    }
    return cumulative_product;
}

// Convention: theta is polar angle and phi is azimuthal. 
complexd_t Ylm(int l, int m, double theta, double phi){
    complexd_t y = std::sqrt((2 * l + 1) / (4 * pi * prod(l-m+1, l+m)))
        * std::polar(1.0, m * phi)
        * std::assoc_legendre(l, m, std::cos(phi));
    return y;
}

complexd_t Rnl(int n, int l, double r){
    double a = 0.529e-10;
    complexd_t c = std::sqrt(std::pow(2 / (n * a), 3)
            / (prod(n-l, n+l) * (2 * n)))
            * std::polar(1.0, -r / (n*a))
            * std::pow(2*r/(n*a), l)
            * std::assoc_laguerre(n-l-1, 2*l+1, 2*r/(n*a));
    return c;
}

complexd_t psi_nlm(int n, int l, int m, double r, double theta, double phi){
    return Rnl(n, l, r) * Ylm(l, m, theta, phi);
}

// Endpoint exclusive
// TODO doesn't work, returning memory address of destroyed object
void linspace(double start, double stop, int n, double* array){
    double step = (stop - start)/n;
    for (int i=0; i<n; i++){
        array[i] = start + i * step;
    }
}

struct Dims
{
    int r;
    int theta;
    int phi;
};


complexd_t* psi_arr(int n, int l, int m, Dims dims){

    double phi[dims.phi];
    double theta[dims.theta];
    double r[dims.r];
    linspace(0, 2*pi, dims.phi, phi);
    linspace(0, pi, dims.theta, theta);
    linspace(0, 1, dims.r, r);


    int size { dims.r * dims.theta * dims.phi };

    auto *psi { new complexd_t[size]{} };
    if (!psi) // handle case where new returned null
    {
        // Do error handling here
        std::cout << "Could not allocate memory";
    }

    complexd_t *iter {psi};
    for (int index {0}; index < size; index++){
        int i = index % dims.r;
        int j = (index / dims.r) % dims.theta;
        int k = index / (dims.r * dims.theta);
        *iter = psi_nlm(n, l, m, r[i], theta[j], phi[k]);
        iter++;
    }
        
    return psi;
}

double *abs_psi_sq(int n, int l, int m, Dims dims){
    complexd_t *psi { psi_arr(n, l, m, dims) };
    int size { dims.phi * dims.theta * dims.r };
    double *abs_psi_squared { new double[size] };

    complexd_t *iterpsi {psi};
    double *iterabs {abs_psi_squared};
    for (int i{0}; i < size; i++){
        *iterabs = std::pow(std::abs(*iterpsi), 2);
        iterabs++;
        iterpsi++;
    }
    delete[] psi;
    return abs_psi_squared;
}


int main(){

    double *aps { abs_psi_sq(3, 2, 1, Dims{100, 100, 100}) };

    delete[] aps;
    return 0;
}
