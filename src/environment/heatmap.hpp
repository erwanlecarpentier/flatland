#ifndef HEATMAP_HPP_
#define HEATMAP_HPP_

#include <gaussian_reward_field.hpp>
#include <save.hpp>

/**
 * @brief Heatmap reward model
 */
class heatmap : public reward_model {
public:
    std::vector<gaussian_reward_field> rfield; ///< Reward field container
    std::vector<std::vector<std::vector<double>>> trajectories; ///< Trajectories
    std::vector<std::string> trajectories_output_paths;

    /**
     * @brief Constructor
     */
    heatmap(std::vector<gaussian_reward_field> _rfield) : rfield(_rfield) {
        trajectories.resize(rfield.size());
        trajectories_output_paths.reserve(rfield.size());
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
            elt.step(s);
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

    /**
     * @brief Reward backup
     *
     * If necessary, for reward backup.
     */
    void reward_backup() override {
        for(unsigned i = 0; i < rfield.size(); ++i) {
            trajectories[i].emplace_back(std::vector<double>{rfield[i].x,rfield[i].y});
        }
    }

    /**
     * @brief Save reward backup
     *
     * If necessary, for reward backup.
     */
    void save_reward_backup() const override {
        for(unsigned i = 0; i < rfield.size(); ++i) {
            std::string path = "data/rfield" + std::to_string(i) + ".csv";
            initialize_backup(std::vector<std::string>{"x","y"},path,",");
            save_matrix(trajectories[i],path,",",std::ofstream::app);
        }
    }
};

#endif // HEATMAP_HPP_
