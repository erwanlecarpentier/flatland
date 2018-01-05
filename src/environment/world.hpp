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
    //std::vector<circle> goals; ///< Goals (waypoints) of the environment //TRM
    reward_model rwm; ///< Reward model of the environment
    std::vector<std::vector<double>> trajectory; ///< Matrix of the trajectory for backup
    std::string trajectory_output_path; ///< Output path for the trajectory
    //unsigned initial_number_of_goals; ///< Initial number of goals //TRM TODO: rwm must be able to say if it is terminal

    /**
     * @brief Constructor
     *
     * Constructor wrt the given parameters.
     * @param {const parameters &} p; parameters
     */
    world(const parameters &p) {
        trajectory_output_path = p.TRAJECTORY_OUTPUT_PATH;
        p.parse_world(xsize,ysize,walls,rwm);
        //initial_number_of_goals = goals.size(); //TRM
        initialize_backup(std::vector<std::string>{"x","y"},trajectory_output_path,",");
    }

    /**
     * @brief Remove elements
     *
     * Remove the given elements of the input vector.
     * @param {std::vector<T> &} v; input vector
     * @param {std::vector<unsigned> &} indices; indices of the vectors to be removed
     */
    template <class T>
    void remove_elements(std::vector<T> &v, std::vector<unsigned> &indices) {
        for (unsigned i=0; i<indices.size(); ++i) {
            v.erase(v.begin() + indices[i]);
            for(unsigned j=i+1; j<indices.size(); ++j) {
                --indices[j];
            }
        }
    }


    /**
     * @brief Remove waypoints at given position
     *
     * Remove all the waypoints at the position of the given state.
     * @param {const state &} s; given state
     * @return Return the number of removed waypoints.
     */
    unsigned remove_waypoints_at(const state &s) {
        std::vector<unsigned> matching_goals_indices;
        unsigned counter = 0;
        for(unsigned i=0; i<rwm.waypoints.size(); ++i) { // Goal checking
            if(rwm.waypoints[i].is_within(s.x, s.y)) {
                ++counter;
                matching_goals_indices.push_back(i);
            }
        }
        remove_elements(rwm.waypoints,matching_goals_indices);
        return counter;
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
