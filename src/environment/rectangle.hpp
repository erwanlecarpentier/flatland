#ifndef RECTANGLE_HPP_
#define RECTANGLE_HPP_

#include <shape.hpp>

class rectangle : public shape {
public:
    std::tuple<double,double> center;
    double xsize;
    double ysize;

    /**
     * @brief Constructor
     */
    rectangle(
        std::tuple<double,double> _center,
        double _xsize,
        double _ysize) :
        center(_center),
        xsize(_xsize),
        ysize(_ysize)
    { /* Nothing to do */}

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
            x - std::get<0>(center),
            xsize
        ) && is_less_than(
            y - std::get<1>(center),
            ysize
        );
    }
};

#endif // RECTANGLE_HPP_
