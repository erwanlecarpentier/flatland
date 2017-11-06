#ifndef DISCRETE_WORLD_HPP_
#define DISCRETE_WORLD_HPP_

/**
 * @brief Discrete world
 *
 * Discrete grid world
 */
class discrete_world {
public:
    std::vector<std::vector<int>> grid_world; ///< discrete map for discrete world

    /**
     * @brief Constructor
     *
     * Constructor wrt the given parameters.
     * @param {parameters &} p; parameters
     */
    discrete_world(parameters &p) {
        p.parse_grid(grid_world);
    }

    /**
     * @brief World value
     *
     * Evaluate the world at the given state.
     * @param {const std::vector<double> &} s; given state
     * @return Return the value of the world.
     */
    int get_value_at(const std::vector<double> &s) {
        return grid_world.at(s.at(0)).at(s.at(1));
    }

    /**
     * @brief Print grid with agent's position
     *
     * Print the environment including the agent's position.
     * @param {const std::vector<double> &} agent_position; position of the agent
     */
    void print(const std::vector<double> &agent_position) {
        unsigned rval = (unsigned) agent_position[0];
        unsigned cval = (unsigned) agent_position[1];
        for(unsigned i=0; i<grid_world.size(); ++i) {
            for(unsigned j=0; j<grid_world[i].size(); ++j) {
                if(i == rval && j == cval) {
                    std::cout << "X ";
                } else {
                    if(grid_world[i][j] < 0) {
                        std::cout << "- ";
                    } else {
                        std::cout << grid_world[i][j] << " ";
                    }
                }
            }
            std::cout << std::endl;
        }
    }
};

#endif // DISCRETE_WORLD_HPP_
