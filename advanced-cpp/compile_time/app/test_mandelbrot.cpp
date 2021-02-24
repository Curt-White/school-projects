#include <iostream>
#include "ra/mandelbrot.hpp"
#include <fstream>

int main() {
    constexpr auto s = ra::fractal::mandelbrot<10, 10>;

    std::ofstream myfile("/Users/curtwhite/Desktop/out.pbm");
    if (myfile.is_open()) {
        myfile << s.begin();
        myfile.flush();
        myfile.close();
    } else {
        std::cerr << "didn't write" << std::endl;
    }

    // std::cout << ra::cexpr::num_digits(15) << std::endl;
    // std::cout << ra::cexpr::num_digits(50);


    return 0;
}