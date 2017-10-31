#ifndef ENVIRONMENT_HPP_
#define ENVIRONMENT_HPP_

class environment {
public:
    bool is_continuous;
    std::vector<std::vector<int>> grid_world;
    double misstep_probability;
    std::vector<std::vector<double>> action_space; ///< Full action space

    /**
     * @brief Default constructor
     *
     * Default constructor initialising the parameters via a 'parameters' object.
     * @param {const parameters &} p; parameters
     */
    environment(const parameters &p) {
        is_continuous = p.IS_CONTINUOUS;
        misstep_probability = p.MISSTEP_PROBABILITY;
        action_space = p.ACTION_SPACE;
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
     * @brief Random action except the given one
     *
     * Get a random action which is different from the given one as input.
     * @param {const std::vector<double> &} a; input action
     * @return Return a random action different from the given one as input.
     */
    std::vector<double> random_action_except(const std::vector<double> &a) {
        std::vector<std::vector<double>> as;
        for(auto &elt : action_space) {
            if(!is_equal_to(elt.at(0), a.at(0))
            || !is_equal_to(elt.at(1), a.at(1))) {
                as.push_back(elt);
            }
        }
        return rand_element(as);
    }

    /**
     * @brief Resulting state
     *
     * Apply the transition taking into account the uncertainty of the environment.
     * @warning next state vector is cleared.
     * @param {const std::vector<double> &} s; state
     * @param {const std::vector<double> &} a; action
     * @return Return the resulting state
     */
    const std::vector<double> resulting_state(
        const std::vector<double> &s,
        const std::vector<double> &a)
    {
        std::vector<double> ac = a;
        if(uniform_double(0.,1.) < misstep_probability) { // misstep
            ac = random_action_except(ac); //TODO debug randomization
        }
        std::vector<double> s_p = s;
        s_p.at(0) += ac.at(0);
        s_p.at(1) += ac.at(1);
        return s_p;
    }

    /**
     * @brief State transition
     *
     * State transition function, compute the resulting state from the application of the
     * given action at the given state.
     * @warning next state vector is cleared.
     * @param {const std::vector<double> &} s; state
     * @param {const std::vector<double> &} a; action
     * @param {std::vector<double> &} s_p; next state
     * @return Return true if the action is valid at the given state.
     */
    bool state_transition(
        const std::vector<double> &s,
        const std::vector<double> &a,
        std::vector<double> &s_p)
    {
        assert(s.size() == 2 && a.size() == 2 && !is_continuous); // continuous version not implemented yet
        s_p = resulting_state(s,a);
        /* depcrecated
        s_p.clear();
        s_p.push_back(s.at(0) + a.at(0));
        s_p.push_back(s.at(1) + a.at(1));
        */
        if(grid_world_value(s_p) == -1) { // frontier
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
     * @param {const std::vector<double> &} s; state
     * @param {const std::vector<double> &} a; action
     * @param {const std::vector<double> &} s_p; next state
     * @return Return the resulting reward.
     */
    double reward_function(
        const std::vector<double> &s,
        const std::vector<double> &a,
        const std::vector<double> &s_p)
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
     * @param {const std::vector<double> &} s; state
     * @param {const std::vector<double> &} a; action
     * @param {double} r; reward
     * @param {std::vector<double> &} s_p; next state
     */
    void transition(
        const std::vector<double> &s,
        const std::vector<double> &a,
        double & r,
        std::vector<double> &s_p)
    {
        state_transition(s,a,s_p);
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
