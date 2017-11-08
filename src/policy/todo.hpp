#ifndef TODO_POLICY_HPP_
#define TODO_POLICY_HPP_

#include <environment.hpp>

/**
 * @brief TODO policy
 */
class todo_policy {
public:
    environment * envt; ///< Pointer to an environment, used for action space reduction

    /**
     * @brief Constructor
     *
     * Constructu using the given parameters
     * @param {const parameters &} p; parameters
     * @param {environment *} en; pointer to the environment, used for action space reduction
     */
    todo_policy(const parameters &p, environment * en) {
        /* TODO: set the parameters here */
    }

    /**
     * @brief Policy operator
     *
     * Policy operator for the undertaken action at given state.
     * @param {const state &} s; given state
     * @return Return the undertaken action at s.
     */
	std::shared_ptr<action> operator()(const state &s) {
        /* TODO: define the policy here */
	}

    /**
     * @brief Process reward
     *
     * Process the resulting reward from transition (s,a,s_p)
     * @param {state &} s; state
     * @param {std::shared_ptr<action> &} a; action
     * @param {state &} s_p; next state
     */
    void process_reward(
        const state & s,
        const std::shared_ptr<action> & a,
        const state & s_p)
    {
        /* TODO: define the reward use here */
    }

    /**
     * @brief Get backup
     *
     * Get the backed-up values.
     * @return Return a vector containing the values to be saved.
     */
    std::vector<double> get_backup() {
        return std::vector<double>{/* TODO: define the vales to backup here */};
    }
};

#endif // TODO_POLICY_HPP_
