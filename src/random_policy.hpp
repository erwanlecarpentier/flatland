#ifndef RANDOM_POLICY_HPP_
#define RANDOM_POLICY_HPP_

/**
 * @brief Random policy
 */
class random_policy {
public:
    std::vector<std::vector<double>> action_space;
    /**
     * @brief Policy operator
     *
     * Policy operator for the undertaken action at given state.
     * @param {std::vector<double> &} s; given state
     * @return Return the undertaken action at s.
     */
	std::vector<double> operator()(std::vector<double> &s) {
        (void) s;
        return rand_element(action_space);
	}

    /**
     * @brief Process reward
     *
     * Process the resulting reward from transition (s,a,s_p)
     * @param {std::vector<double> &} s; state
     * @param {std::vector<double> &} a; action
     * @param {std::vector<double> &} s_p; next state
     */
    void process_reward(
        std::vector<double> & s,
        std::vector<double> & a,
        std::vector<double> & s_p)
    {
        // Random policy does not learn.
    }

    /**
     * @brief Set action space
     *
     * Set the action space attribute as the input action space.
     * @param {std::vector<std::vector<double>>} as; given action space to be copied
     */
    void set_action_space(std::vector<std::vector<double>> as) {
        action_space = as;
    }
};

#endif // RANDOM_POLICY_HPP_
