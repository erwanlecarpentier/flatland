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
#include <parameters.hpp>
#include <environment.hpp>
#include <agent.hpp>

void run() {
    parameters p("config/main.cfg"); // read parameters
    environment e(p); // create environment
    agent a(p); // create agent
    printv(a.state);
    e.print_grid();
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
