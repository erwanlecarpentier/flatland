#ifndef STATE_HPP_
#define STATE_HPP_

/**
 * @brief State class
 */
class state {
public:
    double x; ///< x coordinate
    double y; ///< y coordinate
    double v; ///< velocity
    double theta; ///< angle with x axis (rad)
    unsigned goal_reached_counter; ///< Goal reached counter

    /**
     * @brief Default constructor
     */
    state(
        double _x = 0.,
        double _y = 0.,
        double _v = 1.,
        double _theta = 0.) :
        x(_x),
        y(_y),
        v(_v),
        theta(_theta)
    { /* Nothing to do */ }

    /**
     * @brief Set to default
     *
     * Set the attributes to default values.
     */
    void set_to_default() {
        x = 0.;
        y = 0.;
        v = 0.;
        theta = 0.;
    }

    /**
     * @brief Print
     *
     * Print the attributes of the state
     */
    void print() {
        std::cout << "State, ";
        std::cout << "x: " << x << " y: " << y;
        std::cout << " v: " << v << " theta: " << theta << std::endl;
    }
};

#endif // STATE_HPP_
