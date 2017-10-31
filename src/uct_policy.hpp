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
     * Make use of the input parameters.
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
     * @brief Terminal node test
     *
     * A node is considered terminal if all of its states are terminal states.
     * However another state will still be sampled in the tree policy method.
     * If the node is root, only the labelling state is tested.
     * @param {node &} v; tested node
     * @param {environment &} en; environment
     * @return Return 'true' if the node is considered terminal.
     */
    bool is_node_terminal(
        node &v,
        environment &en)
    {
        if(v.is_root()) {
            return en.is_terminal(v.get_state());
        } else {
            for(auto &s: v.get_states()) {
                if(!en.is_terminal(s)) {
                    return false;
                }
            }
            return true;
        }
    }

    /**
     * @brief State sampling
     *
     * Sample a new state w.r.t. to the incoming action and the parents state and add it to
     * the node.
     * @param {node *} v; pointer to the node
     * @param {environment &} en; environment
     */
    void sample_new_state(
        node * v,
        environment &en)
    {
        assert(!v->is_root());
        std::vector<double> a = v->get_incoming_action();
        std::vector<double> s;
        if((v->parent)->is_root()) {
            s = (v->parent)->get_state();
        } else {
            s = (v->parent)->get_last_sampled_state();
        }
        std::vector<double> s_p;
        en.state_transition(s,a,s_p);
        v->add_to_states(s_p);
    }

    /**
     * @brief Expansion method
     *
     * Expand the node i.e. create a new leaf node.
     * @param {node &} v; reference on the expanded node
     * @param {environment &} en; environment
     * @return Return a pointer to the created leaf node
     */
    node * expand(
        node &v,
        environment &en)
    {
        std::vector<double> nodes_action = v.get_next_expansion_action();
        std::vector<double> nodes_state;
        if(v.is_root()) {
            nodes_state = v.get_state();
        } else {
            nodes_state = v.get_last_sampled_state();
        }
        std::vector<double> new_state;
        en.state_transition(nodes_state,nodes_action,new_state);
        v.create_child(nodes_action,new_state);
        return v.get_last_child();
    }

    /**
     * @brief Tree policy
     *
     * Apply the tree policy.
     * Recursive method.
     * @param {node &} v;
     * @param {environment &} en; environment
     * @return Return a pointer to the created leaf node or to the current node if terminal.
     */
    node * tree_policy(
        node &v,
        environment &en)
    {
        if(is_node_terminal(v,en)) { // terminal
            sample_new_state(&v,en);
            return &v;
        } else if(!v.is_fully_expanded()) { // expand node
            return expand(v,en);
        } else { // apply UCT tree policy
            //node * v_p = uct_child(v);
            //sample_new_state(v_p);
            //return tree_policy(*v_p);
        }
    }

    /**
     * @brief Build UCT tree
     *
     * Build a tree wrt vanilla UCT algorithm.
     * The tree is kept in memory.
     * @param {const std::vector<double> &} s; current state of the agent
     * @param {environment &} en; reference to the environment
     */
    void build_uct_tree(
        const std::vector<double> &s,
        environment &en)
    {
        root_node.clear_node();
        root_node.set_state(s);
        root_node.set_action_space(reduced_action_space(s,en));
        expd_counter = 0;
        for(unsigned i=0; i<budget; ++i) {
            node *ptr = tree_policy(root_node,en);
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
        build_uct_tree(s,en);
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
