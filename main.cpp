#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <tuple>
#include <vector>

#include <agent.hpp>
#include <discrete_world.hpp>
#include <continuous_world.hpp>
#include <environment.hpp>
#include <parameters.hpp>
#include <random.hpp>
#include <uct.hpp>
#include <oluct.hpp>
#include <save.hpp>
#include <utils.hpp>

/**
 * @brief Run
 *
 * Run a single simulation using the given parameters.
 * @param {const parameters &} p; parameters of the simulation
 * @param {bool} prnt; set to true for display
 * @param {bool} bckp; set to true for backup
 * @param {std::vector<std::vector<double>>} backup_vector; backup vector into which each
 * simulation records its backed up values
 */
template <class PLC, class WRLD>
void run(
    const parameters &p,
    bool prnt,
    bool bckp,
    std::vector<std::vector<double>> &backup_vector)
{
    environment<WRLD> en(p);
    agent<PLC,WRLD> ag(p,&en);
    unsigned t = 0; // time
	std::clock_t c_start = std::clock();
    while(!en.is_terminal(ag.state)) {
        ag.apply_policy();
        en.transition(ag.state,ag.action,ag.reward,ag.state_p);
        ag.process_reward();
        if(prnt) {
            std::cout << "t:" << t << " ";
            std::cout << "s:" << ag.state.at(0) << " " << ag.state.at(1) << " ";
            std::cout << "a:" << ag.action.at(0) << " " << ag.action.at(1) << " ";
            std::cout << "s_p:" << ag.state_p.at(0) << " " << ag.state_p.at(1) << std::endl;
            en.print(ag.state);
        }
        ag.step();
        ++t;
    }
    std::clock_t c_end = std::clock();
    double time_elapsed_ms = 1000. * (c_end - c_start) / CLOCKS_PER_SEC;
    if(prnt) {
        std::cout << "t:" << t << " ";
        std::cout << "s:" << ag.state.at(0) << " " << ag.state.at(1) << " ";
        std::cout << "a:" << ag.action.at(0) << " " << ag.action.at(1) << " ";
        std::cout << "s_p:" << ag.state_p.at(0) << " " << ag.state_p.at(1) << std::endl;
        en.print(ag.state);
    }
    if(bckp) {
        std::vector<double> simbackup = {
            (double) t, //score
            time_elapsed_ms // computational cost
        };
        std::vector<double> agbackup = ag.policy.get_backup();
        simbackup.insert(simbackup.end(),agbackup.begin(),agbackup.end());
        backup_vector.push_back(simbackup);
    }
}

/**
 * @brief Run switch, for policy choice
 *
 * Run a single simulation using the given parameters.
 * Applies the switch between the different policies.
 * @param {const parameters &} p; parameters of the simulation
 * @param {bool} prnt; set to true for display
 * @param {bool} bckp; set to true for backup
 * @param {std::vector<std::vector<double>>} backup_vector; backup vector into which each
 * simulation records its backed up values
 */
void run_switch(
    const parameters &p,
    bool prnt,
    bool bckp,
    std::vector<std::vector<double>> &backup_vector)
{
    switch(p.POLICY_SELECTOR) {
        case 0: { // UCT policy
            run<uct,discrete_world>(p,prnt,bckp,backup_vector);
            break;
        }
        case 1: { // OLUCT policy
            //run<oluct,discrete_world>(p,prnt,bckp,backup_vector);
            break;
        }
        default: { // random policy
            //run<random_policy,discrete_world>(p,prnt,bckp,backup_vector);
        }
    }
}

/**
 * @brief Multi run
 *
 * Perform multiple simulation run.
 * All the parameters are fetched in this method.
 * @param {unsigned} nbsim; number of simulations
 * @param {const char *} config_path; configuration path
 * @param {const char *} output_path; ouput path for backup
 */
void multi_run(
    unsigned nbsim,
    const char *config_path,
    const char *output_path)
{
    parameters p(config_path); // load parameters
    bool prnt = true; // set to true if print
    bool bckp = true; // set to true if backup
    std::vector<std::vector<double>> backup_vector; // backup container
    std::vector<std::string> names = { // set backup variables names
        "score",
        "computational_cost",
        "nb_calls"
    };
    std::string sep = ","; // separator for backup file
    if(bckp) { // initialize backup names
        initialize_backup(names,output_path,sep);
    }
    for(unsigned i=0; i<nbsim; ++i) {
        run_switch(p,prnt,bckp,backup_vector);
    }
    if(bckp) { // save all
        save_matrix(backup_vector,output_path,sep,std::ofstream::app);
    }
}

void test() {
    parameters p("config/main.cfg");
    p.IS_WORLD_CONTINUOUS = true;
    environment<continuous_world> e(p);
}

/**
 * @brief Main function
 */
int main() {
    try {
        srand(time(NULL));
        //multi_run(1,"config/main.cfg","data/test.dat");
        test();
    }
    catch(const std::exception &e) {
        std::cerr << "Error in main(): standard exception caught: " << e.what() << std::endl;
    }
    catch(...) {
        std::cerr << "Error in main(): unknown exception caught" << std::endl;
    }
}
