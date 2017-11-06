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
    double xsize; ///< Horizontal dimension of the environment
    double ysize; ///< Vertical dimension of the environment
    std::vector<shape> elements; ///< Container of the wall elements of the world
    std::vector<std::vector<double>> trajectory; ///< Matrix of the trajectory for backup

    /**
     * @brief Constructor
     *
     * Constructor wrt the given parameters.
     * @param {parameters &} p; parameters
     */
    continuous_world(parameters &p) {
        p.parse_cworld(xsize,ysize,elements);
    }

    /**
     * @brief World value
     *
     * Evaluate the world at the given state.
     * @param {const std::vector<double> &} s; given state
     * @return Return the value of the world.
     */
    int get_value_at(const std::vector<double> &s) {
        for(auto &sh : elements) {
            if(sh.is_within(s.at(0), s.at(1))) {
                return -1;
            }
        }
        return 0;
    }

    /**
     * @brief Print environment
     *
     * Save the position in the trajectory matrix.
     * At the end of the simulation, the matrix is saved in the trajectory file.
     * No real time plot is implemented yet.
     * @param {const std::vector<double> &} agent_position; position of the agent
     */
    void print(const std::vector<double> &agent_position) {
        std::cout << "s: " << agent_position.at(0) << " " << agent_position.at(1) << std::endl;
        trajectory.push_back(agent_position);
    }
};

#endif // CONTINUOUS_WORLD_HPP_
