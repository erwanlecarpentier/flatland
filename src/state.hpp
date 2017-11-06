#ifndef STATE_HPP_
#define STATE_HPP_

/**
 * @brief State class
 */
class state {
public:
    double x;
    double y;

    /**
     * @brief Default constructor
     */
    state(double _x = 0., double _y = 0.) : x(_x), y(_y) { /* Nothing to do */ }

    /**
     * @brief Set to default
     *
     * Set the attributes to default values.
     */
    void set_to_default() {
        x = 0.;
        y = 0.;
    }
};

#endif // STATE_HPP_
