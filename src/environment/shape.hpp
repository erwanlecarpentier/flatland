#ifndef SHAPE_HPP_
#define SHAPE_HPP_

#include <utils.hpp>

/**
 * @brief Virtual shape class
 */
class shape {
public:
    //std::tuple<double,double> center;//TRM

    /**
     * @brief Constructor
     */
    //shape() { /* No default */ }//TRM

    /**
     * @brief Test if a position is within the shape
     *
     * Test if the given position as input belongs to the shape or not.
     * @param {double} x; x coordinate
     * @param {double} y; y coordinate
     * @return Return true if the given position is within the shape.
     */
    virtual bool is_within(double x, double y) = 0;
};

#endif // SHAPE_HPP_
