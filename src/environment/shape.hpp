#ifndef SHAPE_HPP_
#define SHAPE_HPP_

class shape {
public:
    std::tuple<double,double> center;

    /**
     * @brief Constructor
     */
    shape(std::tuple<double,double> _center) : center(_center) {
        //TODO
    }
};

#endif // SHAPE_HPP_
