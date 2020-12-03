#include "../headers/wavefunction.h"

inline const double pi = 3.141592653589793;


// Returns (i + j)!/(i - j)!
// assumes that i > abs(j)
double fracfac(int l, int m){
    int lo = m >= 0 ? l-m : l+m;
    int hi = m >= 0 ? l+m : l-m;
    double p {prod(lo+1, hi)};
    return m >= 0 ? p : 1/p;
}

// Returns the product i * (i+1) * (i+2) * ... * j
// If i > j returns 1
int prod(int i, int j){
    int cumProd{1};
    for (int n = i; n <= j; n++)
        cumProd *= n;
    return cumProd;
}

// Convention: theta is polar angle and phi is azimuthal. 
complexd_t Ylm(int l, int m, double theta, double phi){
    using std::sqrt, std::abs, std::polar, std::assoc_legendre, std::cos;
    complexd_t y = sqrt((2 * l + 1) / (4 * pi * fracfac(l, abs(m))))
            * polar(1.0, m * phi)
            * assoc_legendre(l, abs(m), cos(theta));
    return y;
}

complexd_t Rnl(int n, int l, double r){
    using std::sqrt, std::pow, std::exp, std::assoc_laguerre;
    double a = 0.529e-10;
    complexd_t c = sqrt(pow(2 / (n * a), 3)
            * fracfac(n, l) / ((n-l) * (2 * n)))
            * exp( -r / (n*a))
            * pow(2*r/(n*a), l)
            * assoc_laguerre(n-l-1, 2*l+1, 2*r/(n*a));
    return c;
}

complexd_t psi_nlm(int n, int l, int m, double r, double theta, double phi){
    return Rnl(n, l, r) * Ylm(l, m, theta, phi);
    //return Ylm(l, m, theta, phi);
}

// Endpoint exclusive
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
void spherical_from_cart(double cart[3], double *sph){
    using std::sqrt, std::pow, std::asin, std::atan2;
    double r = sqrt(pow(cart[0],2) + pow(cart[1],2) + pow(cart[2],2));
    double rho = sqrt(pow(cart[0],2) + pow(cart[1],2));
    double theta = cart[2] > 0 ? asin(rho / r) : pi - asin(rho / r);
    double phi {atan2(cart[0], cart[1])};
    if (phi < 0)
        phi = phi + 2 * pi;
    
    sph[0] = r;
    sph[1] = theta;
    sph[2] = phi;
}

void complex_to_color(complexd_t c, double *col_arr){
    using std::abs, std::arg, std::sin, std::cos, std::pow;
    col_arr[0] = abs(c) * pow(sin(arg(c)/2), 2);
    col_arr[1] = abs(c) * pow(sin(arg(c)/2 + pi/3), 2);
    col_arr[2] = abs(c) * pow(sin(arg(c)/2 + 2*pi/3), 2);
}



double *get_colors(int n, int l, int m, double phi_c, double theta_c, 
               double xmin, double xmax, double ymin, double ymax,
               int n_x, int n_y, double normalization_const){
    // n, l, m are the arguments for the wave function
    // phi_c and theta_c are the azimuth and polar angle that the 
    // camera is pointing in
    using std::sin, std::cos, std::real, std::imag, std::abs;
    int size {n_x * n_y * 4};
    double deltax = (xmax - xmin)/n_x;
    double deltay = (ymax - ymin)/n_y;

    // Basis vectors for the plane (in normal cartesian coords)
    double unit_xp[3] {cos(phi_c)*cos(theta_c),
                      sin(phi_c)*cos(theta_c), 
                     -sin(theta_c)};
    double unit_yp[3] {-sin(phi_c),
                      cos(phi_c),
                      0};
    double unit_zp[3] {cos(phi_c)*sin(theta_c),
                      sin(phi_c)*sin(theta_c),
                      cos(theta_c)};

    double *colors { new double[size] };
    double *itercol {colors};
    double maximum_psi{0};
    for (int i{0}; i < size/4; i++){
        // Calculate x and y
        double x_p = xmin + deltax * (int)(i / n_y);
        double y_p = ymin + deltay * (i % n_y);
        // Calculate r, theta and phi
        double p_coord[3] { x_p, y_p, 0 };
        double car_coord[3];
        convert_to_basis(p_coord, unit_xp, unit_yp, unit_zp, car_coord);
        double sph_coord[3];
        spherical_from_cart(car_coord, sph_coord);

        complexd_t psi = psi_nlm(n, l, m, sph_coord[0],
                                 sph_coord[1], sph_coord[2]);
        double col[3];
        complex_to_color(psi, col);
        *(itercol++) = col[0];
        *(itercol++) = col[1];
        *(itercol++) = col[2];
        *(itercol++) = 1.0;

        if (abs(psi) > maximum_psi)
            maximum_psi = abs(psi);
    }
    itercol = colors;
    for (int i{0}; i < size/4; i++){
        *(itercol++) /= normalization_const;
        *(itercol++) /= normalization_const;
        *(itercol++) /= normalization_const;
        itercol++;
    }

    return colors;    
}

// Adds v1 and v2 and puts result in v1
// v1 and v2 are assumed to be of length 3
void add(double v1[3], const double v2[3]){
    for (int i{0}; i < 3; i++){
        v1[i] = v1[i] + v2[i];
    }
}
// Adds v1 and v2 and puts result in res
// v1, v2, and res are assumed to be of length 3
void add(const double v1[3], const double v2[3], double res[3]){
    for (int i{0}; i < 3; i++){
        res[i] = v1[i] + v2[i];
    }
}
// Takes vector v in basis e, and basis vectors for e in another
// basis e' and puts representation of v in e' in res
void convert_to_basis(double v[3], double e1[3], double e2[3], double e3[3], double res[3]){
    res[0] = v[0] * e1[0] + v[1] * e2[0] + v[2] * e3[0];
    res[1] = v[0] * e1[1] + v[1] * e2[1] + v[2] * e3[1];
    res[2] = v[0] * e1[2] + v[1] * e2[2] + v[2] * e3[2];
}

double *get_colors2_electric_boogaloo(int n, int l, int m, double phi_c, double theta_c, 
               double xmin, double xmax, double ymin, double ymax, double zmax,
               int n_x, int n_y, int n_z){
    // n, l, m are the arguments for the wave function
    // phi_c and theta_c are the azimuth and polar angle that the 
    // camera is pointing in
    // xmin, xmax, ymin, ymax are the dimensions of the plane
    // that the camera should show
    // n_x, n_y are the number of points to sample in each direction
    // n_z is the number of points to take into account on either side of the plane
    // zmax is the length on either side taken into account
    //
    using std::sin, std::cos, std::real, std::imag, std::complex, std::abs, std::inner_product;
    int size {n_x * n_y * 4};
    double deltax = (xmax - xmin)/n_x;
    double deltay = (ymax - ymin)/n_y;
    double deltaz = 2 * zmax/n_z;

    // Basis vectors for the plane (in normal cartesian coords)
    double unit_xp[3] {cos(phi_c)*cos(theta_c),
                      sin(phi_c)*cos(theta_c), 
                     -sin(theta_c)};
    double unit_yp[3] {-sin(phi_c),
                      cos(phi_c),
                      0};
    double unit_zp[3] {cos(phi_c)*sin(theta_c),
                      sin(phi_c)*sin(theta_c),
                      cos(theta_c)};

    double *colors { new double[size] };
    double *itercol {colors};
    double maximum_psi{0};
    for (int i{0}; i < size/4; i++){
        complexd_t cum_psi{0};
        // Calculate x and y
        double x_p = xmin + deltax * (int)(i / n_y);
        double y_p = ymin + deltay * (i % n_y);
        for (int j{0}; j < n_z; j++){
            double z_p = - zmax + deltaz * j;
            
            // Calculate r, theta and phi
            double p_coord[3] { x_p, y_p, z_p };
            double car_coord[3];
            convert_to_basis(p_coord, unit_xp, unit_yp, unit_zp, car_coord);
            double sph_coord[3];
            spherical_from_cart(car_coord, sph_coord);

            cum_psi += psi_nlm(n, l, m, sph_coord[0], sph_coord[1], sph_coord[2]);
        }
        complexd_t avg_psi {cum_psi / complex<double>(n_z, 0)};
        *(itercol++) = abs(real(avg_psi));
        *(itercol++) = 0.0;
        *(itercol++) = abs(imag(avg_psi));
        *(itercol++) = 1.0;

        if (abs(real(avg_psi)) > maximum_psi)
            maximum_psi = abs(real(avg_psi));
        if (abs(imag(avg_psi)) > maximum_psi)
            maximum_psi = abs(imag(avg_psi));
    }
    if (maximum_psi == 0)
        return colors;
    itercol = colors;
    for (int i{0}; i < size/4; i++){
        *(itercol++) /= 5e12; //maximum_psi;
        itercol++;
        *(itercol++) /= 5e12; //maximum_psi;
        itercol++;
    }

    return colors;    
}
