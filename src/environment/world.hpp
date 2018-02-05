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
    std::vector<std::vector<double>> trajectory; ///< Matrix of the trajectory for backup
    std::string trajectory_output_path; ///< Output path for the trajectory

    /**
     * @brief Constructor
     *
     * Constructor wrt the given parameters.
     * @param {const parameters &} p; parameters
     */
    world(const parameters &p) {
        trajectory_output_path = p.TRAJECTORY_OUTPUT_PATH;
        p.parse_world(xsize,ysize,walls,rwm);
        initialize_backup(std::vector<std::string>{"x","y"},trajectory_output_path,",");
    }

    /**
     * @brief Is wall encountered
     *
     * Test whether a wall is encountered at the given position
     * @param {const state &} s; given state
     * @return Return true if a wall is encountered.
     */
    int is_wall_encountered_at(const state &s) {
        if(is_less_than(s.x,0.)
        || is_less_than(s.y,0.)
        || is_greater_than(s.x,xsize)
        || is_greater_than(s.y,ysize)) { // 1st: border checking
            return true;
        }
        for(unsigned i=0; i<walls.capacity(); ++i) { // 2nd: wall checking
            if(walls[i].is_within(s.x, s.y)) {
                return true;
            }
        }
        return false;
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

#endif // WORLD_HPP_
