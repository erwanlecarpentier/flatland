#ifndef HEATMAP_HPP_
#define HEATMAP_HPP_

#include<gaussian_reward_field.hpp>

/**
 * @brief Heatmap reward model
 */
class heatmap : public reward_model {
public:
    std::vector<gaussian_reward_field> rfield; ///< Reward field container

    /**
     * @brief Constructor
     */
    heatmap(std::vector<gaussian_reward_field> _rfield) : rfield(_rfield) {
        //
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
        const state &s_p) const
    {
        (void) a;
        (void) s_p;
        double value = 0.;
        for(auto &elt : rfield) {
            value += elt.get_value(s);
        }
        return value;
    }

    /**
     * @brief Update reward model
     *
     * Update the reward model wrt current state of the agent.
     * @param {const state &} s; current state of the agent
     */
    void update(const state &s) {
        (void) s;
        for(auto &elt : rfield) {
            elt.step();
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
        return false; // no termination criterion with this reward model
    }
};

#endif // HEATMAP_HPP_
