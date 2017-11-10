#ifndef GO_STRAIGHT_POLICY_HPP_
#define GO_STRAIGHT_POLICY_HPP_

#include <environment.hpp>

/**
 * @brief Go-straight policy
 */
class go_straight {
public:
    environment * envt; ///< Pointer to an environment, used for action space reduction

    /**
     * @brief Constructor
     *
     * Constructu using the given parameters
     * @param {const parameters &} p; parameters
     * @param {environment *} en; pointer to the environment, used for action space reduction
     */
    go_straight(const parameters &p, environment * en) : envt(en) {
        (void) p;
    }

    /**
     * @brief Remove waypoint
     *
     * Remove the waypoints of the environment model at the position of s.
     * Nothing to do for go-straight policy
     * @param {const state &} s; state
     * @return Return the number of reached waypoints.
     */
    unsigned remove_waypoints_at(const state &s) {
        /* Nothing to do for go-straight policy */
        (void) s;
        return 0;
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
        return std::shared_ptr<action>(new navigation_action(1.,.2,0.,0.));
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
