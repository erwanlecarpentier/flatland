#ifndef PARAMETERS_HPP_
#define PARAMETERS_HPP_

#include <libconfig.h++>
#include <fstream>
#include <sstream>
#include <exceptions.hpp>

#include <circle.hpp>
#include <rectangle.hpp>
#include <shape.hpp>

/**
 * @brief Simulation parameters
 *
 * Contains all the simulations parameters.
 */
class parameters {
public:
    bool IS_WORLD_CONTINUOUS;
    double MISSTEP_PROBABILITY;
    std::string GRID_PATH;
    std::string CWORLD_PATH;
    std::string TRAJECTORY_OUTPUT_PATH;
    std::vector<double> INITIAL_STATE;
    unsigned POLICY_SELECTOR;
    unsigned DECISION_CRITERION_SELECTOR;
    std::vector<std::vector<double>> ACTION_SPACE;
    unsigned TREE_SEARCH_BUDGET;
    unsigned DEFAULT_POLICY_HORIZON;
    double UCT_CST;
    double DISCOUNT_FACTOR;

    /**
     * @brief Simulation parameters default constructor
     *
     * The parameters are set to the default values defined in this constructor.
     * @deprecated
     */
    parameters()
    {
        /* TODO: set some default parameters in this constructor */
    }

    /**
     * @brief Parse actions
     *
     * Parse the actions given as parameters
     * @param {libconfig::Config &} cfg; configuration
     */
    void parse_actions(libconfig::Config &cfg) {
        unsigned nbac = 0;
        assert(cfg.lookupValue("nb_actions",nbac));
        for(unsigned i=0; i<nbac; ++i) {
            std::string rname = "a" + std::to_string(i) + "0";
            std::string cname = "a" + std::to_string(i) + "1";
            double rval = 0., cval = 0.;
            if(cfg.lookupValue(rname,rval)
            && cfg.lookupValue(cname,cval)) {
                ACTION_SPACE.emplace_back(std::vector<double>{rval,cval});
            } else { // Error in action names syntax
                throw action_names_configuration_file_exception();
            }
        }
    }

    /**
     * @brief Parse grid
     *
     * Build the discrete world.
     * @param {std::vector<std::vector<int>> &} grid; grid world
     */
    void parse_grid(std::vector<std::vector<int>> &grid) {
        grid.clear();
        std::ifstream fi;
        fi.open(GRID_PATH);
        if(!fi.good()) { // throw exception if file not found
            throw wrong_world_configuration_path();
        }
        for(std::string line; std::getline(fi, line); ) {
            grid.emplace_back(std::vector<int>{});
            int read;
            for(std::stringstream iss(line); iss >> read; grid.back().push_back(read));
        }
    }

    /**
     * @brief Parse continuous world
     *
     * Build the continuous world attributes given as input.
     * @param {double &} xsize;
     * @param {double &} ysize;
     * @param {std::vector<std::unique_ptr<shape>> &} elements;
     * @param {circle &} goal;
     */
    void parse_cworld(
        double &xsize,
        double &ysize,
        std::vector<std::unique_ptr<shape>> &elements,
        circle &goal)
    {
        libconfig::Config cworld_cfg;
        cworld_cfg.readFile(CWORLD_PATH.c_str());
        if(cworld_cfg.lookupValue("xsize",xsize)
        && cworld_cfg.lookupValue("ysize",ysize)
        && cworld_cfg.lookupValue("xgoal",std::get<0>(goal.center))
        && cworld_cfg.lookupValue("ygoal",std::get<1>(goal.center))
        && cworld_cfg.lookupValue("rgoal",goal.radius)) {
            /* Nothing to do */
        } else {
            throw wrong_world_configuration_path();
        }
        unsigned nbr = 0, nbc = 0;
        assert(cworld_cfg.lookupValue("nb_rectangles",nbr));
        assert(cworld_cfg.lookupValue("nb_circles",nbc));
        elements.reserve(nbr + nbc);
        for(unsigned i=0; i<nbr; ++i) { // parse rectangles
            std::string c0name = "cr" + std::to_string(i) + "0";
            std::string c1name = "cr" + std::to_string(i) + "1";
            std::string d0name = "dr" + std::to_string(i) + "0";
            std::string d1name = "dr" + std::to_string(i) + "1";
            double c0 = 0., c1 = 0., d0 = 0., d1 = 0.;
            if(cworld_cfg.lookupValue(c0name,c0)
            && cworld_cfg.lookupValue(c1name,c1)
            && cworld_cfg.lookupValue(d0name,d0)
            && cworld_cfg.lookupValue(d1name,d1)) {
                elements.emplace_back(std::unique_ptr<shape>(new rectangle(std::tuple<double,double>{c0,c1},d0,d1)));
                //rectangle(std::tuple<double,double>{c0,c1},d0,d1)//TRM
            } else {
                throw wrong_world_configuration_path();
            }
        }
        for(unsigned i=0; i<nbc; ++i) { // parse circles
            std::string c0name = "cc" + std::to_string(i) + "0";
            std::string c1name = "cc" + std::to_string(i) + "1";
            std::string rdname = "r"  + std::to_string(i);
            double c0 = 0., c1 = 0., rd = 0.;
            if(cworld_cfg.lookupValue(c0name,c0)
            && cworld_cfg.lookupValue(c1name,c1)
            && cworld_cfg.lookupValue(rdname,rd)) {
                elements.emplace_back(std::unique_ptr<shape>(new circle(std::tuple<double,double>{c0,c1},rd)));
                //elements.emplace_back(circle(std::tuple<double,double>{c0,c1},rd));
            } else {
                throw wrong_world_configuration_path();
            }
        }
    }

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
        double sr = 0., sc = 0.;
        if(cfg.lookupValue("is_world_continuous",IS_WORLD_CONTINUOUS)
        && cfg.lookupValue("misstep_probability",MISSTEP_PROBABILITY)
        && cfg.lookupValue("initial_state_row",sr)
        && cfg.lookupValue("initial_state_col",sc)
        && cfg.lookupValue("policy_selector",POLICY_SELECTOR)
        && cfg.lookupValue("decision_criterion_selector",DECISION_CRITERION_SELECTOR)
        && cfg.lookupValue("tree_search_budget",TREE_SEARCH_BUDGET)
        && cfg.lookupValue("uct_cst",UCT_CST)
        && cfg.lookupValue("discount_factor",DISCOUNT_FACTOR)
        && cfg.lookupValue("default_policy_horizon",DEFAULT_POLICY_HORIZON)) {
            if(IS_WORLD_CONTINUOUS) { // take path of continuous world
                assert(cfg.lookupValue("cworld_path",CWORLD_PATH));
                assert(cfg.lookupValue("trajectory_output_path",TRAJECTORY_OUTPUT_PATH));
            } else { // take path of discrete world
                assert(cfg.lookupValue("grid_path",GRID_PATH));
            }
            INITIAL_STATE = std::vector<double> {sr,sc};
            parse_actions(cfg);
        }
        else { // Error in config file
            throw wrong_syntax_configuration_file_exception();
        }
    }
};

#endif // PARAMETERS_HPP_
