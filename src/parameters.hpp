#ifndef PARAMETERS_HPP_
#define PARAMETERS_HPP_

#include <libconfig.h++>
#include <fstream>
#include <sstream>

#include <action.hpp>
#include <cartesian_action.hpp>
#include <navigation_action.hpp>
#include <circle.hpp>
#include <exceptions.hpp>
#include <rectangle.hpp>
#include <shape.hpp>
#include <state.hpp>

constexpr double TO_RAD = 0.01745329251; ///< degrees to radians

/**
 * @brief Simulation parameters
 *
 * Contains all the simulations parameters.
 */
class parameters {
public:
    std::string MAIN_CFG_PATH;
    std::string GRID_PATH;
    std::string CWORLD_PATH;
    std::string TRAJECTORY_OUTPUT_PATH;
    bool IS_WORLD_CONTINUOUS;
    double MISSTEP_PROBABILITY;
    double STATE_GAUSSIAN_STDDEV;
    state INITIAL_STATE;
    unsigned POLICY_SELECTOR;
    unsigned DECISION_CRITERION_SELECTOR;
    std::vector<action> ACTION_SPACE;
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
     * @param {std::vector<std::shared_ptr<action>> &} action_space; resulting action space
     */
    void parse_actions(std::vector<std::shared_ptr<action>> &action_space) {
        libconfig::Config cfg;
        cfg.readFile(MAIN_CFG_PATH.c_str());
        unsigned selector = 0;
        assert(cfg.lookupValue("action_definition_selector",selector));
        switch(selector) {
            case 0: { // Polar actions, turned to cartesian actions
                unsigned nbac = 0;
                assert(cfg.lookupValue("nb_actions",nbac));
                for(unsigned i=0; i<nbac; ++i) {
                    std::string mname = "a" + std::to_string(i) + "m";
                    std::string aname = "a" + std::to_string(i) + "a";
                    double mgn = 0., ang = 0.;
                    if(cfg.lookupValue(mname,mgn)
                    && cfg.lookupValue(aname,ang)) {
                        action_space.emplace_back(
                            std::shared_ptr<action>(
                                new cartesian_action(
                                    mgn * cos(TO_RAD * ang),
                                    mgn * sin(TO_RAD * ang)
                                )
                            )
                        );
                    } else { // Error in action names syntax
                        throw action_names_configuration_file_exception();
                    }
                }
                break;
            }
            case 1: { // Navigation actions
                unsigned nb_dtheta = 0, nb_fvelocity = 0;
                double vmax = 1., vmin = 0.;
                assert(cfg.lookupValue("nb_dtheta",nb_dtheta));
                assert(cfg.lookupValue("nb_fvelocity",nb_fvelocity));
                assert(cfg.lookupValue("min_velocity",vmax));
                assert(cfg.lookupValue("max_velocity",vmin));
                for(unsigned i=0; i<nb_dtheta; ++i) {
                    for(unsigned j=0; j<nb_fvelocity; ++j) {
                        std::string dt_name = "dtheta" + std::to_string(i);
                        std::string fv_name = "fvelocity" + std::to_string(j);
                        double dt = 0., fv = 0.;
                        if(cfg.lookupValue(dt_name,dt)
                        && cfg.lookupValue(fv_name,fv)) {
                            action_space.emplace_back(
                                std::shared_ptr<action>(new navigation_action(fv,vmax,vmin,dt))
                            );
                        } else { // Error in action names syntax
                            throw action_names_configuration_file_exception();
                        }
                    }
                }
                break;
            }
            default: { // Cartesian actions
                unsigned nbac = 0;
                assert(cfg.lookupValue("nb_actions",nbac));
                for(unsigned i=0; i<nbac; ++i) {
                    std::string rname = "a" + std::to_string(i) + "x";
                    std::string cname = "a" + std::to_string(i) + "y";
                    double rval = 0., cval = 0.;
                    if(cfg.lookupValue(rname,rval)
                    && cfg.lookupValue(cname,cval)) {
                        action_space.emplace_back(
                            std::shared_ptr<action>(new cartesian_action(rval,cval))
                        );
                    } else { // Error in action names syntax
                        throw action_names_configuration_file_exception();
                    }
                }
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
     * @brief Parse state
     *
     * Build the state of the agent
     * @param {libconfig::Config &} cfg; configuration
     */
    void parse_state(libconfig::Config &cfg) {
        double x = 0., y = 0.;
        if(cfg.lookupValue("initial_state_x",x)
        && cfg.lookupValue("initial_state_y",y)) {
            /* Nothing to do */
        } else {
            throw wrong_world_configuration_path();
        }
        INITIAL_STATE.x = x;
        INITIAL_STATE.y = y;
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
            std::string c0name = "cr" + std::to_string(i) + "x";
            std::string c1name = "cr" + std::to_string(i) + "y";
            std::string d0name = "dr" + std::to_string(i) + "x";
            std::string d1name = "dr" + std::to_string(i) + "y";
            double c0 = 0., c1 = 0., d0 = 0., d1 = 0.;
            if(cworld_cfg.lookupValue(c0name,c0)
            && cworld_cfg.lookupValue(c1name,c1)
            && cworld_cfg.lookupValue(d0name,d0)
            && cworld_cfg.lookupValue(d1name,d1)) {
                elements.emplace_back(std::unique_ptr<shape>(new rectangle(std::tuple<double,double>{c0,c1},d0,d1)));
            } else {
                throw wrong_world_configuration_path();
            }
        }
        for(unsigned i=0; i<nbc; ++i) { // parse circles
            std::string c0name = "cc" + std::to_string(i) + "x";
            std::string c1name = "cc" + std::to_string(i) + "y";
            std::string rdname = "r"  + std::to_string(i);
            double c0 = 0., c1 = 0., rd = 0.;
            if(cworld_cfg.lookupValue(c0name,c0)
            && cworld_cfg.lookupValue(c1name,c1)
            && cworld_cfg.lookupValue(rdname,rd)) {
                elements.emplace_back(std::unique_ptr<shape>(new circle(std::tuple<double,double>{c0,c1},rd)));
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
    parameters(const char *cfg_path) : MAIN_CFG_PATH(cfg_path) {
        libconfig::Config cfg;
        cfg.readFile(MAIN_CFG_PATH.c_str());
        if(cfg.lookupValue("is_world_continuous",IS_WORLD_CONTINUOUS)
        && cfg.lookupValue("misstep_probability",MISSTEP_PROBABILITY)
        && cfg.lookupValue("state_gaussian_stddev",STATE_GAUSSIAN_STDDEV)
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
            parse_state(cfg);
        }
        else { // Error in config file
            throw wrong_syntax_configuration_file_exception();
        }
    }
};

#endif // PARAMETERS_HPP_
