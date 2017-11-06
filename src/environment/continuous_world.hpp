#ifndef CONTINUOUS_WORLD_HPP_
#define CONTINUOUS_WORLD_HPP_

#include <circle.hpp>
#include <rectangle.hpp>
#include <shape.hpp>
#include <save.hpp>

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
    std::vector<std::unique_ptr<shape>> elements; ///< Container of the wall elements of the world
    circle goal;
    std::vector<std::vector<double>> trajectory; ///< Matrix of the trajectory for backup
    std::string trajectory_output_path; ///< Output path for the trajectory

    /**
     * @brief Constructor
     *
     * Constructor wrt the given parameters.
     * @param {parameters &} p; parameters
     */
    continuous_world(parameters &p){
        trajectory_output_path = p.TRAJECTORY_OUTPUT_PATH;
        p.parse_cworld(xsize,ysize,elements,goal);
        initialize_backup(std::vector<std::string>{"x","y"},trajectory_output_path,",");
    }

    /**
     * @brief World value
     *
     * Evaluate the world at the given state.
     * @param {const state &} s; given state
     * @return Return the value of the world.
     */
    int get_value_at(const state &s) {
        if(is_less_than(s.x,0.)
        || is_less_than(s.y,0.)
        || is_greater_than(s.x,xsize)
        || is_greater_than(s.y,ysize)) { // Border checking
            return -1;
        }
        for(auto &sh : elements) { // Wall checking is performed first
            if(sh->is_within(s.x, s.y)) {
                return -1;
            }
        }
        if(goal.is_within(s.x, s.y)) { // Goal checking
            return +1;
        }
        return 0;
    }

    /**
     * @brief Print environment
     *
     * Save the position in the trajectory matrix.
     * At the end of the simulation, the matrix is saved in the trajectory file.
     * No real time plot is implemented yet.
     * @param {const state &} s; state of the agent
     */
    void print(const state &s) {
        trajectory.push_back(std::vector<double>{s.x,s.y});
    }

    /**
     * @brief Save trajectory
     *
     * Save the trajectory for plotting purpose.
     */
    void save_trajectory() {
        save_matrix(trajectory,trajectory_output_path,",",std::ofstream::app);
    }
};

#endif // CONTINUOUS_WORLD_HPP_
