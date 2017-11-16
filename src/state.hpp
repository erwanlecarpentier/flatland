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
    unsigned waypoints_reached_counter; ///< Waypoints reached counter

    /**
     * @brief Default constructor
     */
    state(
        double _x = 0.,
        double _y = 0.,
        double _v = 1.,
        double _theta = 0.,
        unsigned _waypoints_reached_counter = 0) :
        x(_x),
        y(_y),
        v(_v),
        theta(_theta),
        waypoints_reached_counter(_waypoints_reached_counter)
    {}

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
        waypoints_reached_counter = 0;
    }

    /**
     * @brief Print
     *
     * Print the attributes of the state
     */
    void print() {
        std::cout << "State, ";
        std::cout << "x: " << x << " y: " << y;
        std::cout << " v: " << v << " theta: " << theta/0.01745329251 << " (deg)" << std::endl;
    }
};

#endif // STATE_HPP_
