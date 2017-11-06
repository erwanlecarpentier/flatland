#ifndef SHAPE_HPP_
#define SHAPE_HPP_

#include <utils.hpp>

class shape {
public:
    std::tuple<double,double> center;

    /**
     * @brief Constructor
     */
    shape() { /* No default */ }

    /**
     * @brief Test if a position is within the shape
     *
     * Test if the given position as input belongs to the shape or not.
     * @todo Handle this with pure virtual method.
     * @param {double} x; x coordinate
     * @param {double} y; y coordinate
     * @return Return true if the given position is within the shape.
     */
    virtual bool is_within(double x, double y) {
        (void) x;
        (void) y;
        throw call_to_generic_shape_exception();
        return false;
    }
};

#endif // SHAPE_HPP_
