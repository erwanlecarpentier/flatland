#ifndef CARTESIAN_ACTION_HPP_
#define CARTESIAN_ACTION_HPP_

#include <state.hpp>

/**
 * @brief Cartesian action class
 */
class cartesian_action {
public:
    double dx; ///< variation along x coordinate
    double dy; ///< variation along y coordinate
    double _v; ///< absolute velocity
    double _theta; ///< angle with x axis (rad)

    /**
     * @brief Default constructor
     */
    cartesian_action(
        double _dx = 0.,
        double _dy = 0.) :
        dx(_dx),
        dy(_dy)
    {
        _v = sqrt(dx * dx + dy * dy);
        _theta = atan(dy / dy);
    }

    /**
     * @brief Apply action
     *
     * Modifiy the input state by applying the action.
     * @param {state &} s; modified state
     */
    void apply(state &s) {
        s.x += dx;
        s.y += dy;
        s.v = _v;
        s.theta = _theta;
    }

    /**
     * @brief Set to default
     *
     * Set the attributes to default values.
     */
    void set_to_default() {
        dx = 0.;
        dy = 0.;
        _v = 0.;
        _theta = 0.;
    }
};

#endif // CARTESIAN_ACTION_HPP_
