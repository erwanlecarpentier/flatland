#ifndef WORLD_HPP_
#define WORLD_HPP_

#include <circle.hpp>
#include <rectangle.hpp>
#include <shape.hpp>
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
    std::vector<circle> goals; ///< Goals (waypoints) of the environment
    std::vector<std::vector<double>> trajectory; ///< Matrix of the trajectory for backup
    std::string trajectory_output_path; ///< Output path for the trajectory

    /**
     * @brief Constructor
     *
     * Constructor wrt the given parameters.
     * @param {parameters &} p; parameters
     */
    world(parameters &p){
        trajectory_output_path = p.TRAJECTORY_OUTPUT_PATH;
        p.parse_world(xsize,ysize,walls,goals);
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
        for(unsigned i=0; i<walls.capacity(); ++i) { // Wall checking is performed first
            if(walls[i].is_within(s.x, s.y)) {
                return -1;
            }
        }
        for(auto &g : goals) {
            if(g.is_within(s.x,s.y)) {
                return +1;
            }
        }
        /*
        std::vector<unsigned> matching_goals_indices;
        for(unsigned i=0; i<goals.size(); ++i) { // Goal checking
            if(goals[i].is_within(s.x, s.y)) {
                matching_goals_indices.push_back(i);
            }
        }
        if(matching_goals_indices.size() > 0) { // Goal(s) reached
            //remove_elements(goals,matching_goals_indices);
            return +1;
        }
        */
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

#endif // WORLD_HPP_
