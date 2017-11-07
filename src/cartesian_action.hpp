#ifndef CARTESIAN_ACTION_HPP_
#define CARTESIAN_ACTION_HPP_

#include <action.hpp>

/**
 * @brief Cartesian action class
 */
class cartesian_action : public action {
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
    void apply(state &s) override {
        s.x += dx;
        s.y += dy;
        s.v = _v; // not used
        s.theta = _theta; // not used
    }

    /**
     * @brief Set to default
     *
     * Set the attributes to default values.
     */
    void set_to_default() override {
        dx = 0.;
        dy = 0.;
        _v = 0.;
        _theta = 0.;
    }

    /**
     * @brief Print
     *
     * Print the attributes of the action
     */
    void print() override {
        std::cout << "Cartesian action, dx: ";
        std::cout << dx << " dy: " << dy << std::endl;
    }
};

#endif // CARTESIAN_ACTION_HPP_
