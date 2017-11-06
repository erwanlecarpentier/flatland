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
    std::vector<shape> elements; ///< Container of the wall elements of the world
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
     * @param {const std::vector<double> &} s; given state
     * @return Return the value of the world.
     */
    int get_value_at(const std::vector<double> &s) {
        if(is_less_than(s[0],0.)
        || is_less_than(s[1],0.)
        || is_greater_than(s[0],xsize)
        || is_greater_than(s[1],ysize)) { // Border checking
            return -1;
        }
        for(auto &sh : elements) { // Wall checking is performed first
            if(sh.is_within(s[0], s[1])) {
                return -1;
            }
        }
        if(goal.is_within(s[0], s[1])) { // Goal checking
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
     * @param {const std::vector<double> &} agent_position; position of the agent
     */
    void print(const std::vector<double> &agent_position) {
        //std::cout << "s: " << agent_position.at(0) << " " << agent_position.at(1) << std::endl;
        trajectory.push_back(agent_position);
    }

    /**
     * @brief Save trajectory
     *
     * Save the trajectory for plotting purpose.
     */
    void save_trajectory() {
        std::cout << "traj: " << std::endl;//TRM
        for(auto &elt : trajectory) {//TRM
            printv(elt);
        }
        save_matrix(trajectory,trajectory_output_path,",",std::ofstream::app);
    }
};

#endif // CONTINUOUS_WORLD_HPP_
