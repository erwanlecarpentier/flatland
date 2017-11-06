#ifndef ENVIRONMENT_HPP_
#define ENVIRONMENT_HPP_

#include <parameters.hpp>
#include <utils.hpp>

/**
 * @brief Environment
 *
 * Environment template class.
 */
template<class WRLD>
class environment {
public:
    typedef WRLD WRLD_type;
    bool is_continuous; ///< Is state space continuous
    WRLD world;
    double misstep_probability; ///< Probability of misstep
    std::vector<std::vector<double>> action_space; ///< Full space of the actions available in the environment

    /**
     * @brief Default constructor
     *
     * Default constructor initialising the parameters via a 'parameters' object.
     * @param {parameters &} p; parameters
     */
    environment(parameters &p) : world(p) {
        action_space = p.ACTION_SPACE;
        misstep_probability = p.MISSTEP_PROBABILITY;
    }

    /**
     * @brief Print environment
     *
     * Print the environment including the agent's position.
     * @param {const state &} s; state of the agent
     */
    void print(const state &s) {
        world.print(s);
    }

    /**
     * @brief World value
     *
     * Evaluate the world at the given state.
     * @param {const state &} s; given state
     * @return Return the value of the world.
     */
    int world_value_at(const state &s) {
        return world.get_value_at(s);
    }

    /**
     * @brief Is state valid
     *
     * Test if the state is valid (wall or not).
     * @param {const state &} s; given state
     * @return Return the value of the grid.
     */
    bool is_state_valid(const state &s) {
        return (world_value_at(s) != -1);
    }

    /**
     * @brief Get action space
     *
     * Get the action space at the given state.
     * @param {const state &} s; given state
     * @return Return space of the available actions at s.
     */
    std::vector<std::vector<double>> get_action_space(const state &s) {
        std::vector<std::vector<double>> resulting_as;
        for(auto &a : action_space) {
            state s_p = {s.x + a.at(0), s.x + a.at(1)};
            if(is_state_valid(s_p)) {
                resulting_as.push_back(a);
            }
        }
        return resulting_as;
    }

    /**
     * @brief State transition
     *
     * State transition function, compute the resulting state from the application of the
     * given action at the given state.
     * @warning next state vector is cleared.
     * @param {const state &} s; state
     * @param {std::vector<double>} a; copy of the action
     * @param {state &} s_p; next state
     * @return Return true if the action is valid at the given state.
     */
    bool state_transition(
        const state &s,
        std::vector<double> a,
        state &s_p)
    {
        assert(a.size() == 2);
        if(is_less_than(uniform_double(0.,1.),misstep_probability)) { // misstep
            a = rand_element(get_action_space(s));
        }
        s_p = s;
        s_p.x += a[0];
        s_p.y += a[1];
        if(!is_state_valid(s_p)) { // frontier
            s_p = s;
            return false;
        } else { // admissible state
            return true;
        }
    }

    /**
     * @brief Reward function
     *
     * Reward function, compute the resulting reward from the transition (s,a,s_p).
     * @param {state &} s; state
     * @param {const std::vector<double> &} a; action
     * @param {state &} s_p; next state
     * @return Return the resulting reward.
     */
    double reward_function(
        const state &s,
        const std::vector<double> &a,
        const state &s_p)
    {
        (void) s;
        (void) a;
        if(world_value_at(s_p) == 1) {
            return 1.;
        } else {
            return 0.;
        }
    }

    /**
     * @brief Transition operator
     *
     * Transition operator, compute the resulting state and reward wrt a state and an action.
     * @param {const state &} s; state
     * @param {const std::vector<double> &} a; action
     * @param {double &} r; reward
     * @param {state &} s_p; next state
     */
    void transition(
        const state &s,
        const std::vector<double> &a,
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
     * Convention for gridworld if 1 for a terminal grid cell.
     * @param {const state &} s; given state
     * @return Return true if the test is terminal, else false.
     */
    bool is_terminal(const state &s) {
        if(world_value_at(s) == 1) {
            return true;
        } else {
            return false;
        }
    }

    /**
     * @brief Save trajectory
     *
     * Save the trajectory for plotting purpose with continuous world.
     */
    void save_trajectory() {
        world.save_trajectory();
    }
};

#endif // ENVIRONMENT_HPP_
