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
void run(const parameters &p, bool print) {
    environment e(p);
    agent<PLC> a(p);
    if(print) {
        e.print_grid();
        printv(a.state);
    }
    while(!e.is_terminal(a.state)) {
        a.apply_policy();
        e.transition<PLC>(a);
        a.process_reward();
        a.step();
        if(print) {
            printv(a.state);
        }
    }
}

void run_switch(const parameters &p, bool print) {
    switch(p.POLICY_SELECTOR) {
        case 0: { // Random policy
            run<random_policy>(p,print);
            break;
        }
        default: {
            //TODO
        }
    }
}

/**
 * @brief Main function
 */
int main() {
    try {
        srand(time(NULL));
        run_switch(parameters("config/main.cfg"),true);
    }
    catch(const std::exception &e) {
        std::cerr<<"Error in main(): standard exception caught: "<<e.what()<<std::endl;
    }
    catch(...) {
        std::cerr<<"Error in main(): unknown exception caught"<<std::endl;
    }
}
