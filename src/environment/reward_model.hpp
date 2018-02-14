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

    /**
     * @brief Remove elements
     *
     * Remove the given elements of the input vector.
     * @param {std::vector<T> &} v; input vector
     * @param {std::vector<unsigned> &} indices; indices of the vectors to be removed
     */
    template <class T>
    void remove_elements(std::vector<T> &v, std::vector<unsigned> &indices) {
        for (unsigned i=0; i<indices.size(); ++i) {
            v.erase(v.begin() + indices[i]);
            for(unsigned j=i+1; j<indices.size(); ++j) {
                --indices[j];
            }
        }
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
                /* Nothing to do */
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
    bool is_terminal(const state &s) {
        (void) s;
        switch(reward_model_selector) {
            case 0: { // heatmap reward model
                return false;
            }
            default: { // waypoints reward model
                std::cout << waypoints.size() << std::endl;//TRM
                if(waypoints.size() == 0) {
                    return true;
                }
                return false;
            }
        }
    }
};

#endif // REWARD_MODEL_HPP_
