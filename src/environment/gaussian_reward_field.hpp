#ifndef GAUSSIAN_REWARD_FIELD_HPP_
#define GAUSSIAN_REWARD_FIELD_HPP_

/**
 * @brief Gaussian reward field
 */
class gaussian_reward_field {
public:
    double x; ///< Position on x axis
    double y; ///< Position on y axis
    double vx; ///< Velocity on x axis
    double vy; ///< Velocity on y axis
    double sigma; ///< Standard deviation
    double magnitude; ///< Magnitude

    /**
     * @brief Constructor
     */
    gaussian_reward_field(
        double _x,
        double _y,
        double _vx,
        double _vy,
        double _sigma,
        double _magnitude
    ) : x(_x),
        y(_y),
        vx(_vx),
        vy(_vy),
        sigma(_sigma),
        magnitude(_magnitude)
    {
        //
    }

    /**
     * @brief Get value
     *
     * Get the value of the gaussian field at the position of the input state.
     * @param {const state &} s; input state
     */
    double get_value(const state &s) const {
        return magnitude * exp(-.5 * (pow(x - s.x,2.) + pow(y - s.y,2.)) / pow(sigma,2.) );
    }

    /**
     * @brief Step
     *
     * Forward evolution step.
     */
    void step() {
        x += vx;
        y += vy;
        /*
        double mag = .01;
        x += uniform_double(-mag,mag);
        y += uniform_double(-mag,mag);
        */
    }
};

#endif // GAUSSIAN_REWARD_FIELD_HPP_
