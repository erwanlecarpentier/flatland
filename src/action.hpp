#ifndef ACTION_HPP_
#define ACTION_HPP_

#include <state.hpp>
#include <utils.hpp>

/**
 * @brief Action class
 */
class action {
public:
    action() {}

    /**
     * @brief Apply action
     *
     * Modifiy the input state by applying the action.
     * @param {state &} s; modified state
     */
    virtual void apply(state &s) = 0;

    /**
     * @brief Set to default
     *
     * Set the attributes to default values.
     */
    virtual void set_to_default() = 0;
};

#endif // ACTION_HPP_
