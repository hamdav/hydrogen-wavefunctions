
#include "../headers/wavefunction.h"

inline const double pi = 3.141592653589793;


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

double *get_abs_psi_sq(int n, int l, int m, Dims dims){
    complexd_t *psi { psi_arr(n, l, m, dims) };
    int size { dims.phi * dims.theta * dims.r };
    double *abs_psi_sq{ new double[size] };

    complexd_t *iterpsi {psi};
    double *iterabs {abs_psi_sq};
    for (int i{0}; i < size; i++){
        *iterabs = std::pow(std::abs(*iterpsi), 2);
        iterabs++;
        iterpsi++;
    }
    delete[] psi;
    return abs_psi_sq;
}

// populates three long array with spherical coordinate
// representation of cartesian coordinates x, y, z
// Does this in the order r, theta, phi
// theta is polar angle and phi is azimuth.
void spherical_from_cart(double x, double y, double z, double *arr){
    using std::sqrt, std::pow, std::asin, std::atan2;
    double r = sqrt(pow(x,2) + pow(y,2) + pow(z,2));
    double rho = sqrt(pow(x,2) + pow(y,2));
    double theta = z > 0 ? asin(rho / r) : pi - asin(rho / r);
    double phi {atan2(x, y)};
    if (phi < 0)
        phi = phi + 2 * pi;
    
    *(arr) = r;
    *(arr+1) = theta;
    *(arr+2) = phi;
}


double *get_colors(int n, int l, int m, double phi_c, double theta_c, 
               double xmin, double xmax, double ymin, double ymax,
               int n_x, int n_y){
    // n, l, m are the arguments for the wave function
    // phi_c and theta_c are the azimuth and polar angle that the 
    // camera is pointing in
    using std::sin, std::cos, std::real, std::imag, std::abs;
    int size {n_x * n_y * 4};
    double deltax = (xmax - xmin)/n_x;
    double deltay = (ymax - ymin)/n_y;


    double *colors { new double[size] };
    double *itercol {colors};
    double maximum_psi{0};
    for (int i{0}; i < size/4; i++){
        // Calculate x and y
        double x_p = xmin + deltax * (int)(i / n_y);
        double y_p = ymin + deltay * (i % n_y);
        // Calculate r, theta and phi
        double x = x_p * sin(phi_c) - y_p * cos(phi_c) * cos(theta_c);
        double y = - x_p * cos(phi_c) - y_p * sin(phi_c) * cos(theta_c);
        double z = y_p * sin(theta_c);
        double sph_coord[3];
        spherical_from_cart(x, y, z, sph_coord);

        complexd_t psi = psi_nlm(n, l, m, sph_coord[0],
                                 sph_coord[1], sph_coord[2]);
        *(itercol++) = abs(real(psi));
        *(itercol++) = 0.0;
        *(itercol++) = abs(imag(psi));
        *(itercol++) = 1.0;

        if (abs(real(psi)) > maximum_psi)
            maximum_psi = abs(real(psi));
        if (abs(imag(psi)) > maximum_psi)
            maximum_psi = abs(imag(psi));
    }
    if (maximum_psi == 0)
        return colors;
    itercol = colors;
    for (int i{0}; i < size/4; i++){
        *(itercol++) /= maximum_psi;
        itercol++;
        *(itercol++) /= maximum_psi;
        itercol++;
    }

    return colors;    
}
