#ifndef PARAMETERS_HPP_
#define PARAMETERS_HPP_

#include <libconfig.h++>
#include <fstream>
#include <sstream>

#include <action.hpp>
#include <cartesian_action.hpp>
#include <navigation_action.hpp>
#include <circle.hpp>
#include <environment.hpp>
#include <exceptions.hpp>
#include <rectangle.hpp>
#include <reward_model.hpp>
#include <waypoints.hpp>
#include <heatmap.hpp>
#include <shape.hpp>
#include <state.hpp>

constexpr double TO_RAD = 0.01745329251; ///< degrees to radians

/**
 * @brief Simulation parameters
 *
 * Contains all the simulations parameters.
 * The fact that the parameters are stored in the attributes of this object and then copied
 * into the parsed object allows to dynamically modify them inside the code so.
 * This is useful when one wants to run simulations on a range of different parameters.
 */
class parameters {
public:
    std::string MAIN_CFG_PATH;
    std::string GRID_PATH;
    std::string WORLD_PATH;
    std::string TRAJECTORY_OUTPUT_PATH;
    // Simulation parameters:
    unsigned SIMULATION_LIMIT_TIME;
    unsigned POLICY_SELECTOR;
    unsigned DEFAULT_POLICY_SELECTOR;
    unsigned ACTIONS_SELECTOR;
    // Environment parameters:
    bool IS_CRASH_TERMINAL;
    double MISSTEP_PROBABILITY;
    double STATE_GAUSSIAN_STDDEV;
    double VOID_REWARD;
    double WALL_REWARD;
    double GOAL_REWARD;
    // Policy parameters:
    bool IS_MODEL_DYNAMIC;
    unsigned TREE_SEARCH_BUDGET;
    unsigned DEFAULT_POLICY_HORIZON;
    unsigned MCTS_STRATEGY_SWITCH;
    double UCT_CST;
    double DISCOUNT_FACTOR;
    double SDM_RATIO;
    double SDV_THRESHOLD;
    double SDSD_THRESHOLD;
    double RDV_THRESHOLD;
    // Model parameters:
    double MODEL_MISSTEP_PROBABILITY;
    double MODEL_STATE_GAUSSIAN_STDDEV;

    /**
     * @brief Simulation parameters default constructor
     *
     * The parameters are set to the default values defined in this constructor.
     * @deprecated
     */
    parameters() {
        /* Set default parameters in this constructor */
    }

    /**
     * @brief Display libconfig ParseException
     *
     * @param {const libconfig::ParseException &} e; displayed exception
     */
    void display_libconfig_parse_exception(const libconfig::ParseException &e) const {
        std::cerr << "Error in parameters(const char *cfg_path): ParseException ";
        std::cerr << "in file " << e.getFile() << " ";
        std::cerr << "at line " << e.getLine() << ": ";
        std::cerr << e.getError() << std::endl;
    }

    /**
     * @brief Parse actions
     *
     * Parse the actions given as parameters
     * @param {std::vector<std::shared_ptr<action>> &} action_space; resulting action space
     */
    void parse_actions(std::vector<std::shared_ptr<action>> &action_space) const {
        libconfig::Config cfg;
        try {
            cfg.readFile(MAIN_CFG_PATH.c_str());
        }
        catch(const libconfig::ParseException &e) {
            display_libconfig_parse_exception(e);
        }
        switch(ACTIONS_SELECTOR) {
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
                assert(cfg.lookupValue("min_velocity",vmin));
                assert(cfg.lookupValue("max_velocity",vmax));
                for(unsigned i=0; i<nb_dtheta; ++i) {
                    for(unsigned j=0; j<nb_fvelocity; ++j) {
                        std::string dt_name = "dtheta" + std::to_string(i);
                        std::string fv_name = "fvelocity" + std::to_string(j);
                        double dt = 0., fv = 0.;
                        if(cfg.lookupValue(dt_name,dt)
                        && cfg.lookupValue(fv_name,fv)) {
                            action_space.emplace_back(
                                std::shared_ptr<action>(new navigation_action(fv,vmax,vmin,TO_RAD * dt))
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
     * @deprecated Grid world is no more implemented this way
     */
    void parse_grid(std::vector<std::vector<int>> &grid) const {
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
     * @brief Parse decision criterion
     *
     * Parse the decision criteria boolean.
     * @param {std::vector<bool> &} v; vector containing every boolean
     */
    void parse_decision_criterion(std::vector<bool> &v) const {
        libconfig::Config cfg;
        try {
            cfg.readFile(MAIN_CFG_PATH.c_str());
        }
        catch(const libconfig::ParseException &e) {
            display_libconfig_parse_exception(e);
        }
        v.reserve(5);
        bool b0, b1, b2, b3, b4;
        if(cfg.lookupValue("b0",b0)
        && cfg.lookupValue("b1",b1)
        && cfg.lookupValue("b2",b2)
        && cfg.lookupValue("b3",b3)
        && cfg.lookupValue("b4",b4)) {
            v.push_back(b0);
            v.push_back(b1);
            v.push_back(b2);
            v.push_back(b3);
            v.push_back(b4);
        } else {
            throw wrong_syntax_configuration_file_exception();
        }
    }

    /**
     * @brief Parse state
     *
     * Parse the initial state of the agent.
     * @param {state &} s; reference to the state of the agent
     */
    void parse_state(state &s) const {
        libconfig::Config cfg;
        try {
            cfg.readFile(WORLD_PATH.c_str());
        }
        catch(const libconfig::ParseException &e) {
            display_libconfig_parse_exception(e);
        }
        if(cfg.lookupValue("initial_state_x",s.x)
        && cfg.lookupValue("initial_state_y",s.y)
        && cfg.lookupValue("initial_state_v",s.v)
        && cfg.lookupValue("initial_state_theta",s.theta)) {
            s.waypoints_reached_counter = 0;
            s.theta *= TO_RAD;
        } else {
            throw wrong_syntax_configuration_file_exception();
        }
    }

    /**
     * @brief Parse world
     *
     * Build the continuous world attributes given as input.
     * @param {double &} xsize;
     * @param {double &} ysize;
     * @param {boost::ptr_vector<shape> &} walls;
     */
    void parse_world(
        double &xsize,
        double &ysize,
        boost::ptr_vector<shape> &elements) const
    {
        libconfig::Config world_cfg;
        try {
            world_cfg.readFile(WORLD_PATH.c_str());
        }
        catch(const libconfig::ParseException &e) {
            display_libconfig_parse_exception(e);
        }
        unsigned nbr = 0, nbc = 0;
        if(world_cfg.lookupValue("xsize",xsize)
        && world_cfg.lookupValue("ysize",ysize)
        && world_cfg.lookupValue("nb_rectangles",nbr)
        && world_cfg.lookupValue("nb_circles",nbc)) {
            elements.reserve(nbr + nbc);
        } else {
            throw wrong_syntax_configuration_file_exception();
        }
        for(unsigned i=0; i<nbr; ++i) { // parse rectangle walls
            std::string xn = "x_rect" + std::to_string(i);
            std::string yn = "y_rect" + std::to_string(i);
            std::string hn = "h_rect" + std::to_string(i);
            std::string wn = "w_rect" + std::to_string(i);
            double x = 0., y = 0., h = 0., w = 0.;
            if(world_cfg.lookupValue(xn,x)
            && world_cfg.lookupValue(yn,y)
            && world_cfg.lookupValue(hn,h)
            && world_cfg.lookupValue(wn,w)) {
                elements.push_back(new rectangle(std::tuple<double,double>{x,y},w,h));
            } else {
                throw wrong_syntax_configuration_file_exception();
            }
        }
        for(unsigned i=0; i<nbc; ++i) { // parse circle walls
            std::string xn = "x_circ" + std::to_string(i);
            std::string yn = "y_circ" + std::to_string(i);
            std::string rn = "r_circ"  + std::to_string(i);
            double x = 0., y = 0., r = 0.;
            if(world_cfg.lookupValue(xn,x)
            && world_cfg.lookupValue(yn,y)
            && world_cfg.lookupValue(rn,r)) {
                elements.push_back(new circle(std::tuple<double,double>{x,y},r));
            } else {
                throw wrong_syntax_configuration_file_exception();
            }
        }
    }

    /**
     * @brief Parse reward model
     *
     * @param {reward_model &} rmodel; reward model
     */
    void parse_reward_model(std::unique_ptr<reward_model> &rmodel) const {
        libconfig::Config world_cfg;
        try {
            world_cfg.readFile(WORLD_PATH.c_str());
        }
        catch(const libconfig::ParseException &e) {
            display_libconfig_parse_exception(e);
        }
        unsigned rmodel_selector = 0;
        if(!world_cfg.lookupValue("reward_model_selector",rmodel_selector)) {
            throw wrong_syntax_configuration_file_exception();
        }
        switch(rmodel_selector) {
            case 0: { // heatmap reward model
                unsigned nb = 0;
                std::vector<gaussian_reward_field> rfield;
                if(world_cfg.lookupValue("nb_rfield",nb)) {
                    rfield.reserve(nb);
                } else {
                    throw wrong_syntax_configuration_file_exception();
                }
                for(unsigned i=0; i<nb; ++i) { // parse reward fields
                    std::string xname = "x_rf" + std::to_string(i);
                    std::string yname = "y_rf" + std::to_string(i);
                    std::string vxname = "vx_rf" + std::to_string(i);
                    std::string vyname = "vy_rf" + std::to_string(i);
                    std::string sigmaname = "sigma_rf" + std::to_string(i);
                    std::string magnitudename = "magnitude_rf" + std::to_string(i);
                    double x = 0., y = 0., vx = 0., vy = 0., sigma = 0., magnitude = 0.;
                    if(world_cfg.lookupValue(xname,x)
                    && world_cfg.lookupValue(yname,y)
                    && world_cfg.lookupValue(vxname,vx)
                    && world_cfg.lookupValue(vyname,vy)
                    && world_cfg.lookupValue(sigmaname,sigma)
                    && world_cfg.lookupValue(magnitudename,magnitude)) {
                        rfield.emplace_back(gaussian_reward_field(x,y,vx,vy,sigma,magnitude));
                    } else {
                        throw wrong_syntax_configuration_file_exception();
                    }
                }
                rmodel = std::unique_ptr<reward_model>(new heatmap(rfield));
                break;
            }
            default: { // waypoints reward model
                unsigned nb = 0;
                double value = 1.;
                std::vector<circle> wp;
                if(world_cfg.lookupValue("nb_waypoints",nb)
                && world_cfg.lookupValue("waypoint_reward",value)) {
                    wp.reserve(nb);
                } else {
                    throw wrong_syntax_configuration_file_exception();
                }
                for(unsigned i=0; i<nb; ++i) { // parse waypoints
                    std::string xname = "x_wp" + std::to_string(i);
                    std::string yname = "y_wp" + std::to_string(i);
                    std::string rname = "r_wp" + std::to_string(i);
                    double x = 0., y = 0., r = 0.;
                    if(world_cfg.lookupValue(xname,x)
                    && world_cfg.lookupValue(yname,y)
                    && world_cfg.lookupValue(rname,r)) {
                        wp.emplace_back(circle(std::tuple<double,double>{x,y},r));
                    } else {
                        throw wrong_syntax_configuration_file_exception();
                    }
                }
                rmodel = std::unique_ptr<reward_model>(new waypoints(wp,value));
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
        try {
            cfg.readFile(MAIN_CFG_PATH.c_str());
        }
        catch(const libconfig::ParseException &e) {
            display_libconfig_parse_exception(e);
        }
        if(cfg.lookupValue("simulation_limit_time",SIMULATION_LIMIT_TIME)
        && cfg.lookupValue("world_path",WORLD_PATH)
        && cfg.lookupValue("trajectory_output_path",TRAJECTORY_OUTPUT_PATH)
        && cfg.lookupValue("is_crash_terminal",IS_CRASH_TERMINAL)
        && cfg.lookupValue("misstep_probability",MISSTEP_PROBABILITY)
        && cfg.lookupValue("state_gaussian_stddev",STATE_GAUSSIAN_STDDEV)
        && cfg.lookupValue("wall_reward",WALL_REWARD)
        && cfg.lookupValue("is_model_dynamic",IS_MODEL_DYNAMIC)
        && cfg.lookupValue("model_misstep_probability",MODEL_MISSTEP_PROBABILITY)
        && cfg.lookupValue("model_state_gaussian_stddev",MODEL_STATE_GAUSSIAN_STDDEV)
        && cfg.lookupValue("action_definition_selector",ACTIONS_SELECTOR)
        && cfg.lookupValue("policy_selector",POLICY_SELECTOR)
        && cfg.lookupValue("default_policy_selector",DEFAULT_POLICY_SELECTOR)
        && cfg.lookupValue("tree_search_budget",TREE_SEARCH_BUDGET)
        && cfg.lookupValue("default_policy_horizon",DEFAULT_POLICY_HORIZON)
        && cfg.lookupValue("mcts_strategy_switch",MCTS_STRATEGY_SWITCH)
        && cfg.lookupValue("uct_cst",UCT_CST)
        && cfg.lookupValue("discount_factor",DISCOUNT_FACTOR)
        && cfg.lookupValue("sdm_ratio",SDM_RATIO)
        && cfg.lookupValue("sdv_threshold",SDV_THRESHOLD)
        && cfg.lookupValue("sdsd_threshold",SDSD_THRESHOLD)
        && cfg.lookupValue("rdv_threshold",RDV_THRESHOLD)) {
            /* Nothing to do */
        }
        else { // Error in config file
            throw wrong_syntax_configuration_file_exception();
        }
    }
};

#endif // PARAMETERS_HPP_
