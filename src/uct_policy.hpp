#ifndef UCT_POLICY_HPP_
#define UCT_POLICY_HPP_

#include <environment.hpp>
#include <node.hpp>

/**
 * @brief UCT policy
 */
class uct_policy {
public:
    node root_node; ///< Root node of the tree
    unsigned budget; ///< Algorithm budget (number of expanded nodes)
    unsigned expd_counter; ///< Counter of the number of expanded nodes
    std::vector<std::vector<double>> action_space; ///< Full action space

    /**
     * @brief Constructor
     *
     * Constructu using the given parameters
     * @param {const parameters &} p; parameters
     */
    uct_policy(const parameters &p) {
        budget = p.TREE_SEARCH_BUDGET;
        expd_counter = 0;
        action_space = p.ACTION_SPACE;
    }

    /**
     * @brief Reduced action space
     *
     * Compute the action space available at the given state.
     * @param {const std::vector<double &} s; state
     * @param {environment &} en; environment
     * @return Return the reduced action space.
     */
    std::vector<std::vector<double>> reduced_action_space(
        const std::vector<double> &s,
        environment &en)
    {
        std::vector<std::vector<double>> ras;
        for(auto &a : action_space) {
            std::vector<double> s_p;
            if(en.state_transition(s,a,s_p)) {
                ras.push_back(a);
            }
        }
        return ras;
    }

    /**
     * @brief Build UCT tree
     *
     * Build a tree wrt vanilla UCT algorithm.
     * The tree is kept in memory.
     * @param {const std::vector<double> &} s; current state of the agent
     */
    void build_uct_tree(const std::vector<double> &s) {
        root_node.clear_node();
        root_node.set_state(s);
        expd_counter = 0;
        for(unsigned i=0; i<budget; ++i) {
            //node *ptr = tree_policy(root_node);
            //double total_return = default_policy(ptr);
            //backup(total_return,ptr);
            ++expd_counter;
        }
    }

    /**
     * @brief Policy operator
     *
     * Policy operator for the undertaken action at given state.
     * @param {const std::vector<double> &} s; given state
     * @param {environment &} en; reference to the real environment for action space
     * reduction (function of the state)
     * @return Return the undertaken action at s.
     */
	std::vector<double> operator()(const std::vector<double> &s, environment &en) {
        build_uct_tree(s);
        //return get_recommended_action(root_node);
	}

    /**
     * @brief Process reward
     *
     * Process the resulting reward from transition (s,a,s_p)
     * @param {std::vector<double> &} s; state
     * @param {std::vector<double> &} a; action
     * @param {std::vector<double> &} s_p; next state
     */
    void process_reward(
        const std::vector<double> & s,
        const std::vector<double> & a,
        const std::vector<double> & s_p)
    {
        // UCT policy does not learn.
        (void) s;
        (void) a;
        (void) s_p;
    }

    /**
     * @brief Set action space
     *
     * Set the action space attribute as the input action space.
     * @param {const std::vector<std::vector<double>>} as; given action space to be copied
     */
    void set_action_space(const std::vector<std::vector<double>> as) {
        action_space = as;
    }
};

#endif // UCT_POLICY_HPP_
