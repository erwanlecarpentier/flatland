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
#include <random_policy.hpp>

template <class PLC>
void run(bool print, const parameters &p) {
    environment e(p);
    agent<PLC> a(p);
    if(print) {
        e.print_grid();
        printv(a.state);
    }
    while(!e.is_terminal(a.state)) {
        //TODO
        //a.apply_policy();
    }
}

void run_switch(const parameters &p)
{
    bool print = true;
    switch(p.POLICY_SELECTOR)
    {
        case 0: run< random_policy >(print, p); break;
    }
}

/**
 * @brief Main function
 */
int main() {
    try {
        srand(time(NULL));
        run_switch(parameters("config/main.cfg"));
    }
    catch(const std::exception &e) {
        std::cerr<<"Error in main(): standard exception caught: "<<e.what()<<std::endl;
    }
    catch(...) {
        std::cerr<<"Error in main(): unknown exception caught"<<std::endl;
    }
}
