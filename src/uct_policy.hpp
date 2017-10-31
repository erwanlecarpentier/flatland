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
    environment * envt; ///< Pointer to an environment, used for action space reduction, termination criterion and generative model

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
     * @brief Set model
     *
     * Take an environment pointer and set it as a model.
     * Used for action space reduction, termination criterion and generative model.
     * @param {environment *} en; input environment
     */
    void set_model(environment * en) {
        envt = en;
    }

    /**
     * @brief Reduced action space
     *
     * Compute the action space available at the given state.
     * @param {const std::vector<double &} s; state
     * @return Return the reduced action space.
     */
    std::vector<std::vector<double>> reduced_action_space(const std::vector<double> &s) {
        std::vector<std::vector<double>> ras;
        for(auto &a : action_space) {
            std::vector<double> s_p;
            if(envt->state_transition(s,a,s_p)) {
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
     * @return Return 'true' if the node is considered terminal.
     */
    bool is_node_terminal(node &v) {
        if(v.is_root()) {
            return envt->is_terminal(v.get_state());
        } else {
            for(auto &s: v.get_states()) {
                if(!envt->is_terminal(s)) {
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
     */
    void sample_new_state(node * v) {
        assert(!v->is_root());
        std::vector<double> a = v->get_incoming_action();
        std::vector<double> s;
        if((v->parent)->is_root()) {
            s = (v->parent)->get_state();
        } else {
            s = (v->parent)->get_last_sampled_state();
        }
        std::vector<double> s_p;
        envt->state_transition(s,a,s_p);
        v->add_to_states(s_p);
    }

    /**
     * @brief Expansion method
     *
     * Expand the node i.e. create a new leaf node.
     * @param {node &} v; reference on the expanded node
     * @return Return a pointer to the created leaf node
     */
    node * expand(node &v) {
        std::vector<double> nodes_action = v.get_next_expansion_action();
        std::vector<double> nodes_state;
        if(v.is_root()) {
            nodes_state = v.get_state();
        } else {
            nodes_state = v.get_last_sampled_state();
        }
        std::vector<double> new_state;
        envt->state_transition(nodes_state,nodes_action,new_state);
        v.create_child(nodes_action,new_state);
        return v.get_last_child();
    }

    /**
     * @brief Tree policy
     *
     * Apply the tree policy.
     * Recursive method.
     * @param {node &} v;
     * @return Return a pointer to the created leaf node or to the current node if terminal.
     */
    node * tree_policy(node &v) {
        if(is_node_terminal(v)) { // terminal
            sample_new_state(&v);
            return &v;
        } else if(!v.is_fully_expanded()) { // expand node
            return expand(v);
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
     */
    void build_uct_tree(const std::vector<double> &s) {
        root_node.clear_node();
        root_node.set_state(s);
        root_node.set_action_space(reduced_action_space(s));
        expd_counter = 0;
        for(unsigned i=0; i<budget; ++i) {
            node *ptr = tree_policy(root_node);
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
     * @return Return the undertaken action at s.
     */
	std::vector<double> operator()(const std::vector<double> &s) {
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
