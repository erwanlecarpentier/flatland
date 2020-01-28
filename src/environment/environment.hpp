#ifndef ENVIRONMENT_HPP_
#define ENVIRONMENT_HPP_

#include <parameters.hpp>
#include <utils.hpp>
#include <save.hpp>

/**
 * @brief Environment
 *
 * Environment class
 */
class environment {
public:
    bool is_crash_terminal; ///< Set to true if you want the crashes to be terminal
    double xsize; ///< Horizontal dimension of the environment
    double ysize; ///< Vertical dimension of the environment
    boost::ptr_vector<shape> walls; ///< Walls of the environment
    std::unique_ptr<reward_model> rmodel; ///< Reward model of the environment
    double misstep_probability; ///< Probability of misstep
    double state_gaussian_stddev; ///< Standard deviation of the Gaussian noise applied on the position
    double wall_reward;
    std::vector<std::shared_ptr<action>> action_space; ///< Full space of the actions available in the environment
    std::vector<std::vector<double>> trajectory; ///< Matrix of the trajectory for backup
    std::string trajectory_output_path; ///< Output path for the trajectory

    /**
     * @brief Default constructor
     *
     * Default constructor initialising the parameters via a 'parameters' object.
     * @param {const parameters &} p; parameters
     */
    environment(const parameters &p) {
        p.parse_world(xsize,ysize,walls);
        p.parse_reward_model(rmodel);
        p.parse_actions(action_space);
        is_crash_terminal = p.IS_CRASH_TERMINAL;
        misstep_probability = p.MISSTEP_PROBABILITY;
        state_gaussian_stddev = p.STATE_GAUSSIAN_STDDEV;
        wall_reward = p.WALL_REWARD;
        trajectory_output_path = p.TRAJECTORY_OUTPUT_PATH;
    }

    /**
     * @brief Copy constructor
     *
     * Construct a copy of the environment pointed by the input pointer.
     * @note The saved trajectory and its output path are not copied.
     * @param {environment *} en; input environment pointer
     */
    environment(const environment & en) {
        is_crash_terminal = en.is_crash_terminal;
        xsize = en.xsize;
        ysize = en.ysize;
        walls = en.walls;
        rmodel = std::unique_ptr<reward_model>(en.rmodel->duplicate());
        misstep_probability = en.misstep_probability;
        state_gaussian_stddev = en.state_gaussian_stddev;
        wall_reward = en.wall_reward;
        action_space = en.action_space;
    }

    /**
     * @brief Get copy
     *
     * Get a copy of this environment.
     * @return Return a copy of the environment.
     */
    environment get_copy() {
        return environment(*this);
    }

    /**
     * @brief Is wall encountered
     *
     * Test whether a wall is encountered at the given position
     * @param {const state &} s; given state
     * @return Return true if a wall is encountered.
     */
    bool is_wall_encountered_at(const state &s) const {
        if(is_less_than(s.x,0.)
        || is_less_than(s.y,0.)
        || is_greater_than(s.x,xsize)
        || is_greater_than(s.y,ysize)) { // 1st: border checking
            return true;
        }
        for(unsigned i=0; i<walls.capacity(); ++i) { // 2nd: wall checking
            if(walls[i].is_within(s.x, s.y)) {
                return true;
            }
        }
        return false;
   }

    /**
     * @brief Is state valid
     *
     * Test if the agent is within a wall or not.
     * @param {const state &} s; given state
     * @return Return true if the world value is different than -1.
     */
    bool is_state_valid(const state &s) const {
        return !is_wall_encountered_at(s);
    }

    /**
     * @brief Is action valid
     *
     * Test if the action is valid at the given state.
     * The action is considered valid if it does not lead to a wall.
     * @param {const state &} s; given state
     * @param {const std::shared_ptr<action>} s; tested action
     * @return Return true if the action is valid.
     */
    bool is_action_valid(const state &s, const std::shared_ptr<action> &a) const {
        state s_p = s;
        a->apply(s_p);
        return is_state_valid(s_p);
    }

    /**
     * @brief Will crash
     *
     * Test if the agent will crash at the given state.
     * This means that at this state, every actions lead to crash.
     * @param {const state &} s; given state
     * @return Return true if the agent has crashed.
     */
    bool will_crash(const state &s) const {
        for(auto &a : action_space) {
            if(is_action_valid(s,a)) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief Get action space
     *
     * Get the action space at the given state.
     * @param {const state &} s; given state
     * @return Return space of the available actions at s.
     */
    std::vector<std::shared_ptr<action>> get_action_space(const state &s) const {
        std::vector<std::shared_ptr<action>> resulting_action_space;
        for(auto &a : action_space) {
            if(is_action_valid(s,a)) {
                resulting_action_space.push_back(a);
            }
        }
        if(resulting_action_space.size() == 0) { // Every action leads to a crash
            resulting_action_space.push_back(rand_element(action_space)); // Take a random action
        }
        return resulting_action_space;
    }

    /**
     * @brief Apply modulus on angle
     */
    void mod_angle(state &s) {
        if(is_less_than(s.theta,-M_PI)) {
            s.theta += 2. * M_PI;
        } else if(is_greater_than(s.theta,M_PI)) {
            s.theta -= 2. * M_PI;
        }
    }

    /**
     * @brief State transition
     *
     * State transition function, compute the resulting state from the application of the
     * given action at the given state.
     * @warning next state vector is cleared.
     * @param {const state &} s; state
     * @param {const std::shared_ptr<action> &} a; copy of the action
     * @param {state &} s_p; next state
     */
    void state_transition(
        const state &s,
        const std::shared_ptr<action> &a,
        state &s_p)
    {
        s_p = s;
        if(is_less_than(uniform_double(0.,1.),misstep_probability)) { // misstep
            rand_element(get_action_space(s))->apply(s_p);
            if(!is_state_valid(s_p)) { // misstep led to a wall, state is unchanged
                s_p = s;
            }
        } else { // no misstep
            a->apply(s_p);
            if(!is_state_valid(s_p) && !is_crash_terminal) { // action led to a wall, angle is reverted
                s_p.theta += M_PI;
            }
        }
        for(unsigned i=0; i<50; ++i) { // 50 trials for gaussian application - no gaussian if no valid result
            state _s_p = s_p;
            _s_p.x += normal_double(0.,state_gaussian_stddev);
            _s_p.y += normal_double(0.,state_gaussian_stddev);
            _s_p.v += normal_double(0.,state_gaussian_stddev);
            _s_p.theta += normal_double(0.,state_gaussian_stddev);
            if(is_state_valid(_s_p)) {
                s_p = _s_p;
                break;
            }
        }
        mod_angle(s_p);
    }

    /**
     * @brief Reward function
     *
     * Reward function, compute the resulting reward from the transition (s,a,s_p).
     * @param {state &} s; state
     * @param {const std::shared_ptr<action> &} a; action
     * @param {state &} s_p; next state
     * @return Return the resulting reward.
     */
    double reward_function(
        const state &s,
        const std::shared_ptr<action> &a,
        const state &s_p)
    {
        if(is_wall_encountered_at(s)) { //TODO maybe unify world and reward_model classes if no polymorphism
            return wall_reward;
        } else {
            return rmodel->get_reward_value_at(s,a,s_p);
        }
    }

    /**
     * @brief Transition operator
     *
     * Transition operator, compute the resulting state and reward wrt a state and an action.
     * @param {const state &} s; state
     * @param {const std::shared_ptr<action> &} a; action
     * @param {double &} r; reward
     * @param {state &} s_p; next state
     */
    void transition(
        const state &s,
        const std::shared_ptr<action> &a,
        double &r,
        state &s_p)
    {
        state_transition(s,a,s_p);
        r = reward_function(s,a,s_p);
    }

    /**
     * @brief Is terminal
     *
     * Test if the given state is terminal.
     * In the current implementation, both the environment and the state are queried for
     * termination criterion
     * @param {state &} s; given state
     * @return Return true if the test is terminal, else false.
     */
    bool is_terminal(const state &s) const {
        return (
            (is_wall_encountered_at(s) && is_crash_terminal) /* Crash */
            || rmodel->is_terminal(s) /* Reward model says terminal eg waypoints reached*/
            || s.is_terminal() /* State is terminal */
        );
    }

    /**
     * @brief Step
     *
     * Go to the next state of the environment considering the current state of the agent.
     * @param {const state &} s; state of the agent
     */
    void step(const state &s) {
        rmodel->update(s);
    }

    /**
     * @brief Print environment
     *
     * Print the environment including the agent's position.
     * @param {const state &} s; state of the agent
     */
    void trajectory_backup(const state &s) {
        trajectory.push_back(std::vector<double>{s.x,s.y});
    }

    /**
     * @brief Save trajectory
     *
     * Save the trajectory for plotting purpose.
     */
    void save_trajectory() const {
        initialize_backup(std::vector<std::string>{"x","y"},trajectory_output_path,",");
        save_matrix(trajectory,trajectory_output_path,",",std::ofstream::app);
    }
};

#endif // ENVIRONMENT_HPP_
