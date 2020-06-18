#ifndef WORLD_HPP_
#define WORLD_HPP_

#include <shape.hpp>
#include <circle.hpp>
#include <rectangle.hpp>
#include <reward_model.hpp>
#include <save.hpp>

/**
 * @brief World
 *
 * Continuous world.
 * Defined by its geometric elements and its size.
 */
class world {
public:
    double xsize; ///< Horizontal dimension of the environment
    double ysize; ///< Vertical dimension of the environment
    boost::ptr_vector<shape> walls; ///< Walls of the environment
    reward_model rwm; ///< Reward model of the environment

    /**
     * @brief Constructor
     *
     * Constructor wrt the given parameters.
     * @param {const parameters &} p; parameters
     */
    world(const parameters &p) {
        p.parse_world(xsize,ysize,walls,rwm);
    }
};

#endif // WORLD_HPP_
