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
    double tbirth; ///< Birth date
    double tdeath; ///< Death date

    /**
     * @brief Constructor
     */
    gaussian_reward_field(
        double _x,
        double _y,
        double _vx,
        double _vy,
        double _sigma,
        double _magnitude,
        unsigned _tbirth,
        unsigned _tdeath
    ) : x(_x),
        y(_y),
        vx(_vx),
        vy(_vy),
        sigma(_sigma),
        magnitude(_magnitude),
        tbirth(_tbirth),
        tdeath(_tdeath)
    {
        //
    }

    /**
     * @brief Is alive
     *
     * Test if alive.
     * @param {unsigned} time; time
     * @return Return true if the Gaussian reward field is alive.
     */
    bool is_alive(unsigned time) const {
        return tbirth <= time && time <= tdeath;
    }

    /**
     * @brief Get value
     *
     * Get the value of the gaussian field at the position of the input state.
     * @param {const state &} s; input state
     */
    double get_value(const state &s) const {
        if(is_alive(s.t)) {
            return magnitude * exp(-.5 * (pow(x - s.x,2.) + pow(y - s.y,2.)) / pow(sigma,2.) );
        } else {
            return 0.;
        }
    }

    /**
     * @brief Step
     *
     * Forward evolution step.
     * @param {const state &} s; input state
     */
    void step(const state &s) {
        if(is_alive(s.t)) {
            x += vx;
            y += vy;
        }
    }
};

#endif // GAUSSIAN_REWARD_FIELD_HPP_
