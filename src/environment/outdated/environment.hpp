#ifndef ENVIRONMENT_HPP_
#define ENVIRONMENT_HPP_

#include <parameters.hpp>
#include <world.hpp>
#include <utils.hpp>

/**
 * @brief Environment
 *
 * Environment class.
 */
template <class AC>
class environment {
public:
    bool is_crash_terminal;
    world w; ///< world
    double misstep_probability; ///< Probability of misstep
    double state_gaussian_stddev; ///< Standard deviation of the Gaussian applied on the position
    double wall_reward;
    std::vector<AC> action_space; ///< Full space of the actions available in the environment

    /**
     * @brief Default constructor
     *
     * Default constructor initialising the parameters via a 'parameters' object.
     * @param {const parameters &} p; parameters
     */
    environment(const parameters &p) : w(p) {
        p.parse_actions<AC>(action_space);
        is_crash_terminal = p.IS_CRASH_TERMINAL;
        misstep_probability = p.MISSTEP_PROBABILITY;
        state_gaussian_stddev = p.STATE_GAUSSIAN_STDDEV;
        wall_reward = p.WALL_REWARD;
    }

    /**
     * @brief Print environment
     *
     * Print the environment including the agent's position.
     * @param {const state &} s; state of the agent
     */
    void print(const state &s) {
        w.print(s);
    }

    /**
     * @brief Is state valid
     *
     * Test if the agent is within a wall or not.
     * @param {const state &} s; given state
     * @return Return true if the world value is different than -1.
     */
    bool is_state_valid(const state &s) {
        return !w.is_wall_encountered_at(s);
    }

    /**
     * @brief Is action valid
     *
     * Test if the action is valid at the given state.
     * The action is considered valid if it does not lead to a wall.
     * @param {const state &} s; given state
     * @param {const AC &} a; tested action
     * @return Return true if the action is valid.
     */
    bool is_action_valid(const state &s, const AC &a) {
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
    bool will_crash(const state &s) {
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
    std::vector<AC> get_action_space(const state &s) {
        std::vector<AC> resulting_action_space;
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
     * @param {std::shared_ptr<action>} a; copy of the action
     * @param {state &} s_p; next state
     */
    void state_transition(
        const state &s,
        AC a,
        state &s_p)
    {
        s_p = s;
        if(is_less_than(uniform_double(0.,1.),misstep_probability)) { // misstep
            a = rand_element(get_action_space(s));
            a.apply(s_p);
            if(!is_state_valid(s_p)) { // misstep led to a wall, state is unchanged
                s_p = s;
            }
        } else { // no misstep
            a.apply(s_p);
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
        const AC &a,
        const state &s_p)
    {
        if(w.is_wall_encountered_at(s)) { //TODO maybe unify world and reward_model classes if no polymorphism
            return wall_reward;
        } else {
            return w.rwm.get_reward_value_at(s,a,s_p);
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
        const AC &a,
        double &r,
        state &s_p)
    {
        state_transition(s,a,s_p);
        r = reward_function(s,a,s_p);
    }


    /**
     * @brief Is waypoint reached
     *
     * Test if at least one waypoint is reached at the given state.
     * @param {const state &} s; given state
     * @return Return true if at least one waypoint is reached at the given state.
     */
    bool is_waypoint_reached(const state &s) { //TODO change this function
        if(!w.is_wall_encountered_at(s)
        &&  w.rwm.is_waypoint_reached(s)) {
            return true;
        } else {
            return false;
        }
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
    bool is_terminal(const state &s) {
        return (
            (w.is_wall_encountered_at(s) && is_crash_terminal) /* Wall */
            || w.rwm.is_terminal(s) /* Reward model says terminal eg waypoints reached*/
            || s.is_terminal() /* State is terminal */
        );
    }

    /**
     * @brief Step
     *
     * Go to the next state of the environment.
     * @param {const state &} s; state of the agent
     */
    void step(const state &s) {
        w.rwm.update_reward_model(s);
    }

    /**
     * @brief Save trajectory
     *
     * Save the trajectory for plotting purpose with continuous world.
     */
    void save_trajectory() {
        w.save_trajectory();
    }
};

#endif // ENVIRONMENT_HPP_
