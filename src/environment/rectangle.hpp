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
    {
        //TODO
    }
};

#endif // RECTANGLE_HPP_
