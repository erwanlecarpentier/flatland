#ifndef CIRCLE_HPP_
#define CIRCLE_HPP_

#include <shape.hpp>

class circle : public shape {
public:
    std::tuple<double,double> center;
    double radius;

    /**
     * @brief Constructor
     *
     * Default values are arbitrary.
     */
    circle(
        std::tuple<double,double> _center = std::tuple<double,double>(0.,0.),
        double _radius = 0.) :
        center(_center),
        radius(_radius)
    {/* Nothing to do */}

    /**
     * @brief Test if position is within the shape
     *
     * Test if the given position belongs to the shape or not.
     * @param {double} x; x coordinate
     * @param {double} y; y coordinate
     * @return Return true if the given position is within the shape.
     */
    bool is_within(double x, double y) override {
        return is_less_than(
            pow(x - std::get<0>(center), 2.) + pow(y - std::get<1>(center), 2.),
            radius * radius
        );
    }
};

#endif // CIRCLE_HPP_
