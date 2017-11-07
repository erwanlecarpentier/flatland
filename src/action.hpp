#ifndef ACTION_HPP_
#define ACTION_HPP_

/**
 * @brief Action class
 */
class action {
public:
    double dx;
    double dy;

    /**
     * @brief Default constructor
     */
    action(double _dx = 0., double _dy = 0.) : dx(_dx), dy(_dy) { /* Nothing to do */ }

    /**
     * @brief Set to default
     *
     * Set the attributes to default values.
     */
    void set_to_default() {
        dx = 0.;
        dy = 0.;
    }
};

#endif // ACTION_HPP_
