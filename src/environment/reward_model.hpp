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
    // Attributes for heatmap reward model
    /* TODO */
};

#endif // REWARD_MODEL_HPP_
