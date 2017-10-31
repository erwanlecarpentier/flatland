#ifndef RANDOM_POLICY_HPP_
#define RANDOM_POLICY_HPP_

#include <environment.hpp>

/**
 * @brief Random policy
 */
class random_policy {
public:
    std::vector<std::vector<double>> action_space; ///< Full action space
    environment * envt; ///< Pointer to an environment, used for action space reduction

    /**
     * @brief Constructor
     *
     * Constructu using the given parameters
     * @param {const parameters &} p; parameters
     */
    random_policy(const parameters &p) {
        action_space = p.ACTION_SPACE;
    }

    /**
     * @brief Set model
     *
     * Take an environment pointer and set it as a model.
     * Used for action space reduction.
     * @param {environment *} en; input environment
     */
    void set_model(environment * en) {
        envt = en;
    }

    /**
     * @brief Reduced action space
     *
     * Compute the action space available at the given state.
     * @param {const std::vector<double &} s; state
     * @return Return the reduced action space.
     */
    std::vector<std::vector<double>> reduced_action_space(const std::vector<double> &s) {
        std::vector<std::vector<double>> ras;
        for(auto &a : action_space) {
            std::vector<double> s_p;
            if(envt->state_transition(s,a,s_p)) {
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
     * @return Return the undertaken action at s.
     */
	std::vector<double> operator()(const std::vector<double> &s) {
        (void) s;
        return rand_element(reduced_action_space(s));
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
        // Random policy does not learn.
        (void) s;
        (void) a;
        (void) s_p;
    }
};

#endif // RANDOM_POLICY_HPP_
