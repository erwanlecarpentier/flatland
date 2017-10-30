#ifndef UCT_POLICY_HPP_
#define UCT_POLICY_HPP_

#include <environment.hpp>

/**
 * @brief UCT policy
 */
class uct_policy {
public:
    std::vector<std::vector<double>> action_space; ///< Full action space

    /**
     * @brief Reduced action space
     *
     * Compute the action space available at the given state.
     * @param {const std::vector<double &} s; state
     * @param {environment &} en; environment
     * @return Return the reduced action space.
     */
    std::vector<std::vector<double>> reduced_action_space(
        const std::vector<double> &s,
        environment &en)
    {
        std::vector<std::vector<double>> ras;
        for(auto &a : action_space) {
            std::vector<double> s_p;
            if(en.state_transition(s,a,s_p)) {
                ras.push_back(a);
            }
        }
        return ras;
    }

    /**
     * @brief Policy operator
     *
     * Policy operator for the undertaken action at given state.
     * @param {const std::vector<double> &} s; given state
     * @param {environment &} en; reference to the real environment for action space
     * reduction (function of the state)
     * @return Return the undertaken action at s.
     */
	std::vector<double> operator()(const std::vector<double> &s, environment &en) {
        //TODO
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
        const std::vector<double> & s,
        const std::vector<double> & a,
        const std::vector<double> & s_p)
    {
        // UCT policy does not learn.
        (void) s;
        (void) a;
        (void) s_p;
    }

    /**
     * @brief Set action space
     *
     * Set the action space attribute as the input action space.
     * @param {const std::vector<std::vector<double>>} as; given action space to be copied
     */
    void set_action_space(const std::vector<std::vector<double>> as) {
        action_space = as;
    }
};

#endif // UCT_POLICY_HPP_
