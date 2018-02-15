#ifndef REWARD_MODEL_HPP_
#define REWARD_MODEL_HPP_

/**
 * @brief Reward model
 */
class reward_model {
public:
    /**
     * @brief Reward value
     *
     * Evaluate the reward value at the given state.
     * @param {state &} s; state
     * @param {const std::shared_ptr<action> &} a; action
     * @param {state &} s_p; next state
     * @return Return the value of the reward.
     */
    virtual double get_reward_value_at(
        const state &s,
        const std::shared_ptr<action> &a,
        const state &s_p
    ) const = 0;

    /**
     * @brief Update reward model
     *
     * Update the reward model wrt current state of the agent e.g. remove a waypoint.
     * @param {const state &} s; current state of the agent
     */
    virtual void update(const state &s) = 0;

    /**
     * @brief Is terminal
     *
     * Test whether it is terminal wrt state or reward model.
     * @param {const state &} s; real state of the agent
     */
    virtual bool is_terminal(const state &s) const = 0;
};






class reward_model_deprecated {
public:
    unsigned reward_model_selector;
    // Attributes for waypoints reward model
    std::vector<circle> waypoints; ///< Waypoints
    double waypoint_reward;
    // Attributes for heatmap reward model
    //std::vector<gaussian_rewards> reward_field;

    /**
     * @brief Is waypoint reached
     *
     * Test if at least one waypoint is reached at the given state.
     * @param {const state &} s; given state
     * @return Return true if at least one waypoint is reached at the given state.
     */
    bool is_waypoint_reached(const state &s) const {
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
        const state &s_p) const {
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

    /**
     * @brief Remove waypoints at given position
     *
     * Remove all the waypoints at the position of the input state.
     * @param {const state &} s; input state
     * @return Return the number of removed waypoints.
     */
    unsigned remove_waypoints_at(const state &s) {
        std::vector<unsigned> indices_buffer;
        unsigned counter = 0;
        for(unsigned i=0; i<waypoints.size(); ++i) { // Goal checking
            if(waypoints[i].is_within(s.x, s.y)) {
                ++counter;
                indices_buffer.push_back(i);
            }
        }
        remove_elements(waypoints,indices_buffer);
        return counter;
    }

    /**
     * @brief Update reward model
     *
     * Update the reward model wrt real position of the agent.
     * E.g. remove a waypoint
     * @param {const state &} s; real state of the agent
     */
    void update_reward_model(const state &s) {
        switch(reward_model_selector) {
            case 0: { // heatmap reward model
                /* Todo */
                break;
            }
            default: { // waypoints reward model
                if(is_waypoint_reached(s)) { // Waypoints case
                    remove_waypoints_at(s);
                }
            }
        }
    }

    /**
     * @brief Is terminal
     *
     * Test whether it is terminal wrt state or reward model.
     * @param {const state &} s; real state of the agent
     */
    bool is_terminal(const state &s) const {
        (void) s;
        switch(reward_model_selector) {
            case 0: { // heatmap reward model
                return false;
            }
            default: { // waypoints reward model
                if(waypoints.size() == 0) {
                    return true;
                }
                return false;
            }
        }
    }
};

#endif // REWARD_MODEL_HPP_
