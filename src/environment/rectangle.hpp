#ifndef RECTANGLE_HPP_
#define RECTANGLE_HPP_

#include <shape.hpp>

class rectangle : public shape {
private:
    shape * do_clone() const override final {
        return new rectangle(center,width,height);
    }

public:
    std::tuple<double,double> center;
    double width;
    double height;

    /**
     * @brief Constructor
     */
    rectangle(
        std::tuple<double,double> _center,
        double _width,
        double _height) :
        center(_center),
        width(_width),
        height(_height)
    { }

    /**
     * @brief Test if position is within the shape
     *
     * Test if the given position belongs to the shape or not.
     * @param {double} x; x coordinate
     * @param {double} y; y coordinate
     * @return Return true if the given position is within the shape.
     */
    bool is_within(double x, double y) const override {
        return is_less_than(
            fabs(x - std::get<0>(center)),
            width / 2.
        ) && is_less_than(
            fabs(y - std::get<1>(center)),
            height / 2.
        );
    }
};

#endif // RECTANGLE_HPP_
