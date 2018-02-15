#ifndef RANDOM_POLICY_HPP_
#define RANDOM_POLICY_HPP_

#include <environment.hpp>

/**
 * @brief Random policy
 */
class random_policy {
public:
    environment model; ///< Environment, used for action space reduction

    /**
     * @brief Constructor
     *
     * Construct wrt the given parameters.
     * @param {const parameters &} p; parameters
     */
    random_policy(const parameters &p) : model(p) {
        (void) p;
    }

    /**
     * @brief Policy operator
     *
     * Policy operator for the undertaken action at given state.
     * @param {const state &} s; given state
     * @return Return the undertaken action at s.
     */
	std::shared_ptr<action> operator()(const state &s) {
        return rand_element(model.get_action_space(s));
	}

    /**
     * @brief Process reward
     *
     * Process the resulting reward from transition (s,a,s_p).
     * Here nothing to do.
     * Random policy does not learn.
     * @param {state &} s; state
     * @param {std::shared_ptr<action> &} a; action
     * @param {state &} s_p; next state
     */
    void process_reward(
        const state & s,
        const std::shared_ptr<action> & a,
        const state & s_p)
    {
        (void) s;
        (void) a;
        (void) s_p;
    }

    /**
     * @brief Get backup
     *
     * Get the backed-up values.
     * @return Return a vector containing the values to be saved.
     */
    std::vector<double> get_backup() {
        return std::vector<double>{};
    }
};

#endif // RANDOM_POLICY_HPP_
