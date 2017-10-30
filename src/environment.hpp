#ifndef ENVIRONMENT_HPP_
#define ENVIRONMENT_HPP_

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
     * @brief Action validity test & next state computation
     *
     * Return true if the action is valid and compute the next state reached by this action.
     * @param {std::vector<double> &} s; state
     * @param {std::vector<double> &} a; action
     * @param {std::vector<double> &} s_p; next state
     * @return Return true if the action is valid at the given state.
     */
    bool is_action_valid_at(
        std::vector<double> &s,
        std::vector<double> &a,
        std::vector<double> &s_p)
    {
        s_p.push_back(s.at(0) + a.at(0));
        s_p.push_back(s.at(1) + a.at(1));
        assert(s_p.size() == 2);
        if(grid_world_value(s_p) == -1) {
            return false;
        } else {
            return true;
        }
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
        assert(s.size() == 2);
        assert(a.size() == 2);
        std::vector<double> s_p;
        if(is_action_valid_at(s,a,s_p)) { // admissible state
            return s_p;
        } else { // frontier
            return s;
        }
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
        (void) s;
        (void) a;
        if(grid_world_value(s_p) == 1) {
            return 1.;
        } else {
            return 0.;
        }
    }

    /**
     * @brief Transition operator
     *
     * Transition operator, compute the resulting state and reward wrt a state and an action.
     * @param {std::vector<double> &} s; state
     * @param {std::vector<double> &} a; action
     * @param {double} r; reward
     * @param {std::vector<double> &} s_p; next state
     */
    void transition(
        std::vector<double> &s,
        std::vector<double> &a,
        double & r,
        std::vector<double> &s_p)
    {
        s_p = state_transition(s,a);
        r = reward_function(s,a,s_p);
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
