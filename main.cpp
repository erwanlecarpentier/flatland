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
#include <display.hpp>
#include <random_policy.hpp>

template <class PLC>
void run(const parameters &p, bool print) {
    environment e(p);
    agent<PLC> a(p);
    if(print) {
        print_grid_with_agent<PLC>(e,a);
    }
    while(!e.is_terminal(a.state)) {
        a.apply_policy();
        e.transition<PLC>(a);
        a.process_reward();
        if(print) {
            print_grid_with_agent<PLC>(e,a);
        }
        a.step();
    }
}

void run_switch(const parameters &p, bool print) {
    switch(p.POLICY_SELECTOR) {
        case 0: { // TODO
            //run<todo_policy>(p,print);
            break;
        }
        default: { // random policy
            run<random_policy>(p,print);
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
        std::cerr << "Error in main(): standard exception caught: " << e.what() << std::endl;
    }
    catch(...) {
        std::cerr << "Error in main(): unknown exception caught" << std::endl;
    }
}
