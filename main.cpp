#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

#include <utils.hpp>

void run() {
    std::cout << "Hello world!\n";
}

/**
 * @brief Main function
 */
int main() {
    try {
        srand(time(NULL));
        run();
    }
    catch(const std::exception &e) {
        std::cerr<<"Error in main(): standard exception caught: "<<e.what()<<std::endl;
    }
    catch(...) {
        std::cerr<<"Error in main(): unknown exception caught"<<std::endl;
    }
}
