#ifndef CIRCLE_HPP_
#define CIRCLE_HPP_

#include <shape.hpp>

class circle : public shape {
public:
    std::tuple<double,double> center;
    double radius;

    /**
     * @brief Constructor
     */
    circle(
        std::tuple<double,double> _center,
        double _radius) :
        center(_center),
        radius(_radius)
    {
        //TODO
    }
};

#endif // CIRCLE_HPP_
