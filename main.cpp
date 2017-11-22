#include <algorithm>
#include <boost/ptr_container/ptr_vector.hpp>
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

#include <action.hpp>
#include <agent.hpp>
#include <world.hpp>
#include <environment.hpp>
#include <parameters.hpp>
#include <random.hpp>
#include <go_straight.hpp>
#include <uct.hpp>
#include <oluct.hpp>
#include <save.hpp>
#include <state.hpp>
#include <utils.hpp>

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
template <class PLC>
void single_run(
    const parameters &p,
    bool prnt,
    bool bckp,
    std::vector<std::vector<double>> &backup_vector)
{
    environment en(p);
    agent<PLC> ag(p,&en);
    unsigned t = 0; // time
	std::clock_t c_start = std::clock();
    for(t=0; t<p.SIMULATION_LIMIT_TIME; ++t) {
        ag.apply_policy();
        en.transition(ag.s,ag.a,ag.reward,ag.s_p);
        ag.process_reward();
        if(prnt) {
            std::cout << t << " ";
            ag.s.print();
            en.print(ag.s);
        }
        ag.step();
        if(en.is_waypoint_reached(ag.s)) {
            en.remove_waypoints_at(ag.s);
            ag.waypoint_reached();
        }
        if(en.is_terminal(ag.s)) { // terminal state reached
            break;
        }
    }
    std::clock_t c_end = std::clock();
    double time_elapsed_ms = 1000. * (c_end - c_start) / CLOCKS_PER_SEC;
    if(prnt) {
        std::cout << t << " ";
        ag.s.print();
        en.print(ag.s);
        en.save_trajectory();
        std::cout << "Finish\n";
    }
    if(bckp) {
        std::vector<double> simbackup = {
            (double) t, /* score */
            time_elapsed_ms /* computational cost */
        };
        std::vector<double> agbackup = ag.policy.get_backup(); // agent backup
        simbackup.insert(simbackup.end(),agbackup.begin(),agbackup.end());
        backup_vector.push_back(simbackup);
    }
}

/**
 * @brief Run switch
 *
 * Run a single simulation using the given parameters.
 * Applies the switch between the different policies.
 * @param {parameters &} p; parameters of the simulation
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
        case 0: { // Go-straight policy
            single_run<go_straight>(p,prnt,bckp,backup_vector);
            break;
        }
        case 1: { // UCT policy
            switch(p.DEFAULT_POLICY_SELECTOR) {
                case 0: { // Go-straight policy
                    single_run<uct<go_straight>>(p,prnt,bckp,backup_vector);
                    break;
                }
                default: { // Random policy
                    single_run<uct<random_policy>>(p,prnt,bckp,backup_vector);
                }
            }
            break;
        }
        case 2: { // OLUCT policy
            switch(p.DEFAULT_POLICY_SELECTOR) {
                case 0: { // Go-straight policy
                    single_run<oluct<go_straight>>(p,prnt,bckp,backup_vector);
                    break;
                }
                default: { // Random policy
                    single_run<oluct<random_policy>>(p,prnt,bckp,backup_vector);
                }
            }
            break;
        }
        default: { // random policy
            single_run<random_policy>(p,prnt,bckp,backup_vector);
        }
    }
}

/**
 * @brief Run
 *
 * Perform multiple simulation run.
 * All the parameters are fetched in this method.
 * @param {unsigned} nbsim; number of simulations
 * @param {const parameters &} p; parameters
 * @param {const char *} output_path; ouput path for backup
 * @param {bool} prnt; set to true if print
 * @param {bool} bckp; set to true if backup
 */
void run(
    unsigned nbsim,
    const parameters &p,
    const char *output_path,
    bool prnt = true,
    bool bckp = true)
{
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
    for(unsigned i=0; i<nbsim; ++i) { // run nbsim different simulations
        run_switch(p,prnt,bckp,backup_vector);
    }
    if(bckp) { // save all
        save_matrix(backup_vector,output_path,sep,std::ofstream::app);
    }
}

void test(char * n) {
    std::vector<double> mp_range = {.0, .05, .1, .15, .2, .25, .3, .35, .4, .45, .5};
    std::string name(n);
    unsigned nbsim = 100;
    for(auto &mp : mp_range) { // for every misstep probability
        std::string cfg_path = "config/backup/discrete/" + name + ".cfg";
        std::string bkp_path = "data/" + name + std::to_string((int)(mp*100.)) + ".csv";
        std::cout << "Output: " << bkp_path << std::endl;
        parameters p(cfg_path.c_str());
        p.MISSTEP_PROBABILITY = mp;
        p.MODEL_MISSTEP_PROBABILITY = mp;
        run(nbsim,p,bkp_path.c_str(),false,true);
    }
}

/**
 * @brief Main function
 */
int main(int argc, char* argv[]) {
    try {
        std::clock_t c_start = std::clock();
        srand(time(NULL));
        if(argc == 2) { // run test method with given path as input
            test(argv[1]);
        } else { // run single simulation
            parameters p("config/main.cfg");
            run(1,p,"data/test.csv",true,true);
        }
        std::clock_t c_end = std::clock();
        double time_elapsed_ms = 1000. * (c_end - c_start) / CLOCKS_PER_SEC;
        std::cout << "Program run in " << time_elapsed_ms << "ms" << std::endl;
    }
    catch(const std::exception &e) {
        std::cerr << "Error in main(): standard exception caught: " << e.what() << std::endl;
    }
    catch(...) {
        std::cerr << "Error in main(): unknown exception caught" << std::endl;
    }
}
