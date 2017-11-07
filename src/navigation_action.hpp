#ifndef NAVIGATION_ACTION_HPP_
#define NAVIGATION_ACTION_HPP_

#include <action.hpp>

/**
 * @brief Navigation action class
 */
class navigation_action : action {
public:
    double fv; ///< absolute velocity variational factor
    double vmax; ///< maximum velocity
    double vmin; ///< minimum velocity
    double dtheta; ///< angle with x axis (rad) variation

    /**
     * @brief Default constructor
     */
    navigation_action(
        double _fv = 0.,
        double _vmax = 1.,
        double _vmin = 0.,
        double _dtheta = 0.) :
        fv(_fv),
        vmax(_vmax),
        vmin(_vmin),
        dtheta(_dtheta)
    {/* Nothing to do */}

    /**
     * @brief Apply action
     *
     * Modifiy the input state by applying the action.
     * @param {state &} s; modified state
     */
    void apply(state &s) override {
        s.x += s.v * cos(s.theta);
        s.y += s.v * sin(s.theta);
        s.v *= fv;
        if(is_greater_than(s.v,vmax)) { // above maximum velocity
            s.v = vmax;
        }
        if(is_less_than(s.v,vmin)) { // under minimum velocity
            s.v = vmin;
        }
        s.theta += dtheta;
    }

    /**
     * @brief Set to default
     *
     * Set the attributes to default values.
     */
    void set_to_default() override {
        fv = 0.;
        dtheta = 0.;
    }
};

#endif // NAVIGATION_ACTION_HPP_
