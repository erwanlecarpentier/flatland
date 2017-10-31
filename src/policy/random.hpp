#ifndef RANDOM_POLICY_HPP_
#define RANDOM_POLICY_HPP_

#include <environment.hpp>

/**
 * @brief Random policy
 */
class random_policy {
public:
    environment * envt; ///< Pointer to an environment, used for action space reduction

    /**
     * @brief Constructor
     *
     * Constructu using the given parameters
     * @param {const parameters &} p; parameters
     * @param {environment *} en; pointer to the environment, used for action space reduction
     */
    random_policy(const parameters &p, environment *en) : envt(en) {
        (void) p; // No parameters in random policy
    }

    /**
     * @brief Policy operator
     *
     * Policy operator for the undertaken action at given state.
     * @param {const std::vector<double> &} s; given state
     * @return Return the undertaken action at s.
     */
	std::vector<double> operator()(const std::vector<double> &s) {
        (void) s;
        return rand_element(envt->get_action_space(s));
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
        (void) s;
        (void) a;
        (void) s_p;
    }
};

#endif // RANDOM_POLICY_HPP_
