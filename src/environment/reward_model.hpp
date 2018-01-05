#ifndef REWARD_MODEL_HPP_
#define REWARD_MODEL_HPP_

/**
 * @brief Reward model
 */
class reward_model {
public:
    unsigned reward_model_selector;
    // Attributes for waypoints reward model
    std::vector<circle> waypoints; ///< Waypoints
    double waypoint_reward;
    // Attributes for heatmap reward model
    /* TODO */

    /**
     * @brief Is waypoint reached
     *
     * Test if at least one waypoint is reached at the given state.
     * @param {const state &} s; given state
     * @return Return true if at least one waypoint is reached at the given state.
     */
    bool is_waypoint_reached(const state &s) {
        assert(reward_model_selector != 0);
        for(auto &wp : waypoints) {
            if(wp.is_within(s.x,s.y)) {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Reward value
     *
     * Evaluate the reward value at the given state.
     * @param {state &} s; state
     * @param {const std::shared_ptr<action> &} a; action
     * @param {state &} s_p; next state
     * @return Return the value of the reward.
     */
    double get_reward_value_at(
        const state &s,
        const std::shared_ptr<action> &a,
        const state &s_p) {
        (void) a;
        (void) s_p;
        switch(reward_model_selector) {
            case 0: { // heatmap reward model
                //TODO
                break;
            }
            default: { // waypoints reward model
                if(is_waypoint_reached(s)) {
                    return waypoint_reward;
                }
            }
        }
        return 0.; // default
    }
};

#endif // REWARD_MODEL_HPP_
