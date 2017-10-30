#ifndef ENVIRONMENT_HPP_
#define ENVIRONMENT_HPP_

#include <agent.hpp>

class environment {
public:
    bool is_continuous;
    std::vector<std::vector<int>> grid_world;

    /**
     * @brief Default constructor
     *
     * Default constructor initialising the parameters via a 'parameters' object.
     * @param {const parameters &} p; parameters
     */
    environment(const parameters &p) {
        is_continuous = p.IS_CONTINUOUS;
        if(is_continuous) {
            std::cout << "TODO: implement continuous world" << std::endl;
        } else {
            grid_world = p.GRID_WORLD;
        }
    }

    /**
     * @brief Grid world value
     *
     * Evaluate the grid world at the given state.
     * @param {const std::vector<double> &} s; given state
     * @return Return the value of the grid.
     */
    int grid_world_value(const std::vector<double> &s) {
        assert(s.size() == 2);
        return grid_world.at(s.at(0)).at(s.at(1));
    }


    /**
     * @brief State transition
     *
     * State transition function, compute the resulting state from the application of the
     * given action at the given state.
     * @param {std::vector<double> &} s; state
     * @param {std::vector<double> &} a; action
     * @return Return the resulting state.
     */
    std::vector<double> state_transition(
        std::vector<double> &s,
        std::vector<double> &a)
    {
        //TODO
    }


    /**
     * @brief Reward function
     *
     * Reward function, compute the resulting reward from the transition (s,a,s_p).
     * @param {std::vector<double> &} s; state
     * @param {std::vector<double> &} a; action
     * @param {std::vector<double> &} s_p; next state
     * @return Return the resulting reward.
     */
    double reward_function(
        std::vector<double> &s,
        std::vector<double> &a,
        std::vector<double> &s_p)
    {
        //TODO
    }


    /**
     * @brief Transition operator
     *
     * Transition operator, apply a transition to an agent i.e. compute its resulting state
     * and reward based on its state-action attributes.
     * @param {agent &} ag; agent
     */
    template <class PLC>
    void transition(agent<PLC> &ag) {
        ag.state_p = state_transition(ag.state,ag.action);
        ag.reward = reward_function(ag.state,ag.action,ag.state_p);
    }

    /**
     * @brief Is terminal
     *
     * Test if the given state is terminal.
     * Convention for gridworld if 1 for a terminal grid cell.
     * @param {const std::vector<double> &} s; given state
     * @return Return true if the test is terminal, else false.
     */
    bool is_terminal(const std::vector<double> &s) {
        if(is_continuous) {
            std::cout << "TODO: implement continuous world" << std::endl;
            return true;
        } else {
            if(grid_world_value(s) == 1) {
                return true;
            } else {
                return false;
            }
        }
    }
};

#endif // ENVIRONMENT_HPP_
