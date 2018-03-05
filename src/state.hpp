#ifndef STATE_HPP_
#define STATE_HPP_

#include<utils.hpp>

/**
 * @brief State class
 */
class state {
public:
    unsigned t; ///< Time
    double x; ///< x coordinate
    double y; ///< y coordinate
    double v; ///< velocity
    double theta; ///< angle with x axis (rad)
    unsigned waypoints_reached_counter; ///< Waypoints reached counter

    /**
     * @brief Default constructor
     */
    state(
        double _t = 0,
        double _x = 0.,
        double _y = 0.,
        double _v = 1.,
        double _theta = 0.,
        unsigned _waypoints_reached_counter = 0) :
        t(_t),
        x(_x),
        y(_y),
        v(_v),
        theta(_theta),
        waypoints_reached_counter(_waypoints_reached_counter)
    {}

    /**
     * @brief Is terminal
     *
     * In the current implementation, both the environment and the state are queried for
     * termination criterion
     * @return Return true if the test is terminal, else false.
     */
    bool is_terminal() const {
        return false;
    }

    /**
     * @brief Set to default
     *
     * Set the attributes to default values.
     */
    void set_to_default() {
        t = 0;
        x = 0.;
        y = 0.;
        v = 0.;
        theta = 0.;
        waypoints_reached_counter = 0;
    }

    /**
     * @brief Equality operator
     *
     * @param {const state &} _s; state to which we compare
     * @return Return true if the states are equal.
     */
    bool is_equal_to(const state &_s) const {
        return (
            are_equal(t,_s.t)
        &&  are_equal(x,_s.x)
        &&  are_equal(y,_s.y)
        &&  are_equal(v,_s.v)
        &&  are_equal(theta,_s.theta)
        &&  waypoints_reached_counter == _s.waypoints_reached_counter
        );
    }

    /**
     * @brief Print
     *
     * Print the attributes of the state
     */
    void print() {
        std::cout << "State,";
        std::cout << " t: " << t;
        std::cout << " x: " << x;
        std::cout << " y: " << y;
        std::cout << " v: " << v;
        std::cout << " theta: " << theta/0.01745329251 << " (deg)";
        std::cout << std::endl;
    }
};

#endif // STATE_HPP_
