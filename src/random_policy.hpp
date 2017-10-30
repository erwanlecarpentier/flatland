#ifndef RANDOM_POLICY_HPP_
#define RANDOM_POLICY_HPP_

/**
 * @brief Random policy
 */
class random_policy {
public:
    /**
     * @brief Policy operator
     *
     * Policy operator for the undertaken action at given state.
     * @param {std::vector<double> &} s; given state
     * @return Return the undertaken action at s.
     */
	std::vector<double> operator()(std::vector<double> &s) {
        (void) s;
        return std::vector<double> {1.,0.};
	}

    /**
     * @brief Process reward
     *
     * Process the resulting reward from transition (s,a,s_p)
     * @param {std::vector<double> &} s;
     * @param {std::vector<double> &} a;
     * @param {std::vector<double> &} s_p;
     */
    void process_reward(
        std::vector<double> & s,
        std::vector<double> & a,
        std::vector<double> & s_p)
    {
        // Random policy does not learn.
    }
};

#endif // RANDOM_POLICY_HPP_
