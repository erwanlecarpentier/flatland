#ifndef WAYPOINTS_HPP_
#define WAYPOINTS_HPP_

/**
 * @brief Waypoints reward model
 */
class waypoints : public reward_model {
public:
    std::vector<circle> wp; ///< Waypoints
    double wp_value; ///< Reward when reaching a waypoint

    /**
     * @brief Constructor
     */
    waypoints(std::vector<circle> _wp, double _wp_value) : wp(_wp), wp_value(_wp_value) {}

    /**
     * @brief Is waypoint reached
     *
     * Test if at least one waypoint is reached at the given state.
     * @param {const state &} s; given state
     * @return Return true if at least one waypoint is reached at the given state.
     */
    bool is_waypoint_reached(const state &s) const {
        for(auto &w : wp) {
            if(w.is_within(s.x,s.y)) {
                return true;
            }
        }
        return false;
    }

    reward_model * duplicate() const override DUPLICATE_DEFAULT_BODY

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
        const state &s_p) const
    {
        (void) a;
        (void) s_p;
        if(is_waypoint_reached(s)) {
            return wp_value;
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
        for(unsigned i=0; i<wp.size(); ++i) { // Goal checking
            if(wp[i].is_within(s.x, s.y)) {
                ++counter;
                indices_buffer.push_back(i);
            }
        }
        remove_elements(wp,indices_buffer);
        return counter;
    }

    /**
     * @brief Update reward model
     *
     * Update the reward model wrt current state of the agent e.g. remove a waypoint.
     * @param {const state &} s; current state of the agent
     */
    void update(const state &s) {
        if(is_waypoint_reached(s)) {
            remove_waypoints_at(s);
        }
    }

    /**
     * @brief Is terminal
     *
     * Test whether it is terminal wrt reward model.
     * @param {const state &} s; real state of the agent
     */
    bool is_terminal(const state &s) const {
        (void) s;
        if(wp.size() == 0) {
            return true;
        }
        return false;
    }

    /**
     * @brief Reward backup
     *
     * If necessary, for reward backup.
     */
    void reward_backup() override {
        // Nothing to backup
    }

    /**
     * @brief Save reward backup
     *
     * If necessary, for reward backup.
     */
    void save_reward_backup() const override {
        // Nothing to backup
    }
};

#endif // WAYPOINTS_HPP_
