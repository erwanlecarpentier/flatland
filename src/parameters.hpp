#ifndef PARAMETERS_HPP_
#define PARAMETERS_HPP_

#include <libconfig.h++>
#include <fstream>
#include <sstream>
#include <exceptions.hpp>

/**
 * @brief Simulation parameters
 *
 * Parameters container for a simulation
 */
class parameters {
public:
    bool IS_CONTINUOUS;
    std::vector<std::vector<int>> GRID_WORLD;
    std::vector<double> INITIAL_STATE;
    unsigned POLICY_SELECTOR;

    /**
     * @brief Simulation parameters 'default' constructor
     *
     * The parameters are set to the values defined in this constructor.
     * @deprecated
     */
    parameters(double is_continuous = false) :
        IS_CONTINUOUS(is_continuous)
    {}

    /**
     * @brief Simulation parameters 'from-configuration-file' constructor
     *
     * The parameters are taken into the config file whose path is given as an argument.
     * The used library for parsing is libconfig.
     * @param {const char *} cfg_path; path of the configuration file
     */
    parameters(const char *cfg_path) {
        libconfig::Config cfg;
        cfg.readFile(cfg_path);
        std::string grid_path;
        double sr = 0., sc = 0.;
        if(cfg.lookupValue("is_continuous",IS_CONTINUOUS)
        && cfg.lookupValue("grid_path",grid_path)
        && cfg.lookupValue("initial_state_row",sr)
        && cfg.lookupValue("initial_state_col",sc)
        && cfg.lookupValue("policy_selector",POLICY_SELECTOR)) {
            GRID_WORLD = parse_grid(grid_path);
            INITIAL_STATE = std::vector<double> {sr,sc};
        }
        else { // Error in config file
            throw wrong_syntax_configuration_file_exception();
        }
    }

    /**
     * @brief Parse grid
     *
     * Parse a grid and turn it into a 'std::vector<std::vector<int>>' object.
     * @warning Integer within the config file must be separated with a blanck.
     * @param {const std::string &} grid_path; path of the grid configuration file
     * @return Return a discrete grid world
     */
    std::vector<std::vector<int>> parse_grid(const std::string &grid_path) {
        std::vector<std::vector<int>> gw;
        std::ifstream fi;
        fi.open(grid_path);
        if(!fi.good()) { // throw exception if file not found
            throw wrong_grid_path();
        }
        for(std::string line; std::getline(fi, line); ) {
            gw.emplace_back(std::vector<int>{});
            int read;
            for(std::stringstream iss(line); iss >> read; gw.back().push_back(read));
        }
        return gw;
    }
};

#endif // PARAMETERS_HPP_
