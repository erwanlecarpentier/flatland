#ifndef GO_STRAIGHT_POLICY_HPP_
#define GO_STRAIGHT_POLICY_HPP_

#include <environment.hpp>

/**
 * @brief Go-straight policy
 */
class go_straight {
public:
    /**
     * @brief Constructor
     *
     * Construct wrt the given parameters.
     * @param {const parameters &} p; parameters
     */
    go_straight(const parameters &p) {
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
        (void) s;
        return std::shared_ptr<action>(new navigation_action(1.,100.,0.,0.));
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

#endif // GO_STRAIGHT_POLICY_HPP_
