#ifndef TODO_SHAPE_HPP_
#define TODO_SHAPE_HPP_

#include <shape.hpp>

class todo_shape : public shape {
private:
    shape * do_clone() const override final {
        return new todo_shape(
            /* TODO: Place constructor arguments here,use the attributes of this object */
        );
    }

public:
    /* TODO: Set attributes */

    /**
     * @brief Constructor
     */
    rectangle() {/* TODO: Implement constructor to do */}

    /**
     * @brief Test if position is within the shape
     *
     * Test if the given position belongs to the shape or not.
     * @param {double} x; x coordinate
     * @param {double} y; y coordinate
     * @return Return true if the given position is within the shape.
     */
    bool is_within(double x, double y) override {
        /* TODO: implement test for belonging of input position */
    }
};

#endif // TODO_SHAPE_HPP_
