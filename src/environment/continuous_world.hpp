#ifndef CONTINUOUS_WORLD_HPP_
#define CONTINUOUS_WORLD_HPP_

#include <circle.hpp>
#include <rectangle.hpp>
#include <shape.hpp>

/**
 * @brief Continuous world
 *
 * Continuous world.
 * Defined by its geometric elements and its size.
 */
class continuous_world {
public:
    double xsize;
    double ysize;
    std::vector<shape> elements;

    /**
     * @brief Constructor
     *
     * Constructor wrt the given parameters.
     * @param {const parameters &} p; parameters
     */
    continuous_world(const parameters &p) {
        (void) p;
        xsize = 4.;
        ysize = 4.;
        elements.emplace_back(rectangle(std::tuple<double,double>{2.,2.},2.,1.));
        elements.emplace_back(circle(std::tuple<double,double>{0.,2.},2.));
    }

    /**
     * @brief World value
     *
     * Evaluate the world at the given state.
     * @param {const std::vector<double> &} s; given state
     * @return Return the value of the world.
     */
    int get_value_at(const std::vector<double> &s) {
        (void) s;
        //TODO
        return 42;
    }

    /**
     * @brief Print environment
     *
     * Print the environment including the agent's position.
     * @param {const std::vector<double> &} agent_position; position of the agent
     */
    void print(const std::vector<double> &agent_position) {
        std::cout << "s: " << agent_position.at(0) << " " << agent_position.at(1) << std::endl;
        //TODO
    }
};

#endif // CONTINUOUS_WORLD_HPP_
