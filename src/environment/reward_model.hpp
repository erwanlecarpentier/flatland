#ifndef REWARD_MODEL_HPP_
#define REWARD_MODEL_HPP_

#include <type_traits>

#define DUPLICATE_DEFAULT_BODY {return new typename std::decay<decltype(*this)>::type(*this);}

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
     * @brief Duplicate
     *
     * Duplicate this object.
     */
    virtual reward_model * duplicate() const = 0;

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

    /**
     * @brief Reward backup
     *
     * If necessary, for reward backup.
     */
    virtual void reward_backup() = 0;

    /**
     * @brief Save reward backup
     *
     * If necessary, for reward backup.
     */
    virtual void save_reward_backup() const = 0;
};

#endif // REWARD_MODEL_HPP_
