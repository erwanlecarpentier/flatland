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
 * @brief Print (t,s,a,s_p)
 */
template <class PLC, class WRLD>
void print_tsasp(double t, const agent<PLC,WRLD> &ag) {
    std::cout << "t:" << t << " ";
    std::cout << "s:" << ag.state.at(0) << " " << ag.state.at(1) << " ";
    std::cout << "a:" << ag.action.at(0) << " " << ag.action.at(1) << " ";
    std::cout << "s_p:" << ag.state_p.at(0) << " " << ag.state_p.at(1) << std::endl;
}

/**
 * @brief Single run
 *
 * Run a single simulation using the given parameters.
 * @param {parameters &} p; parameters of the simulation
 * @param {bool} prnt; set to true for display
 * @param {bool} bckp; set to true for backup
 * @param {std::vector<std::vector<double>>} backup_vector; backup vector into which each
 * simulation records its backed up values
 */
template <class PLC, class WRLD>
void single_run(
    parameters &p,
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
            print_tsasp(t,ag);
            en.print(ag.state);
        }
        ag.step();
        ++t;
    }
    std::clock_t c_end = std::clock();
    double time_elapsed_ms = 1000. * (c_end - c_start) / CLOCKS_PER_SEC;
    if(prnt) {
        print_tsasp(t,ag);
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
 * @brief Run switch
 *
 * Run a single simulation using the given parameters.
 * Applies the switch between the different policies and environments.
 * @param {parameters &} p; parameters of the simulation
 * @param {bool} prnt; set to true for display
 * @param {bool} bckp; set to true for backup
 * @param {std::vector<std::vector<double>>} backup_vector; backup vector into which each
 * simulation records its backed up values
 */
void run_switch(
    parameters &p,
    bool prnt,
    bool bckp,
    std::vector<std::vector<double>> &backup_vector)
{
    switch(p.POLICY_SELECTOR) {
        case 0: { // UCT policy
            if(p.IS_WORLD_CONTINUOUS) {
                single_run<uct<continuous_world>,continuous_world>(p,prnt,bckp,backup_vector);
            } else {
                single_run<uct<discrete_world>,discrete_world>(p,prnt,bckp,backup_vector);
            }
            break;
        }
        case 1: { // OLUCT policy
            if(p.IS_WORLD_CONTINUOUS) {
                single_run<oluct<continuous_world>,continuous_world>(p,prnt,bckp,backup_vector);
            } else {
                single_run<oluct<discrete_world>,discrete_world>(p,prnt,bckp,backup_vector);
            }
            break;
        }
        default: { // random policy
            if(p.IS_WORLD_CONTINUOUS) {
                single_run<random_policy<continuous_world>,continuous_world>(p,prnt,bckp,backup_vector);
            } else {
                single_run<random_policy<discrete_world>,discrete_world>(p,prnt,bckp,backup_vector);
            }
        }
    }
}

/**
 * @brief Run
 *
 * Perform multiple simulation run.
 * All the parameters are fetched in this method.
 * @param {unsigned} nbsim; number of simulations
 * @param {const char *} config_path; configuration path
 * @param {const char *} output_path; ouput path for backup
 */
void run(
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
    std::cout << "test:\n";
    parameters p("config/main.cfg");
    environment<continuous_world> e(p);
    std::vector<double> s = {1.,1.};
    e.print(s);
}

/**
 * @brief Main function
 */
int main() {
    try {
        srand(time(NULL));
        run(1,"config/main.cfg","data/test.dat");
        //test();
    }
    catch(const std::exception &e) {
        std::cerr << "Error in main(): standard exception caught: " << e.what() << std::endl;
    }
    catch(...) {
        std::cerr << "Error in main(): unknown exception caught" << std::endl;
    }
}
