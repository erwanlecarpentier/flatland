#ifndef UCT_POLICY_HPP_
#define UCT_POLICY_HPP_

#include <environment.hpp>
#include <node.hpp>
#include <random.hpp>

/**
 * @brief UCT policy
 */
template <class WRLD>
class uct {
public:
    environment<WRLD> * envt; ///< Pointer to an environment, used for action space reduction, termination criterion and generative model
    node root_node; ///< Root node of the tree
    double uct_cst; ///< UCT constant within UCT formula
    double discount_factor; ///< MDP discount factor
    unsigned horizon; ///< Horizon for default policy
    unsigned budget; ///< Algorithm budget (number of expanded nodes)
    unsigned expd_counter; ///< Counter of the number of expanded nodes
    unsigned nb_calls; ///< Number of calls to the generative model
    random_policy<WRLD> rndplc; ///< Random policy used as a default policy

    /**
     * @brief Constructor
     *
     * Make use of the input parameters.
     * @param {const parameters &} p; parameters
     * @param {environment *} en; pointer to the environment, used for action space reduction,
     * termination criterion and generative model
     */
    uct(const parameters &p, environment<WRLD> *en) :
        envt(en),
        root_node(state(),envt->action_space), // null state as default
        rndplc(p,en)
    {
        budget = p.TREE_SEARCH_BUDGET;
        expd_counter = 0;
        nb_calls = 0;
        uct_cst = p.UCT_CST;
        discount_factor = p.DISCOUNT_FACTOR;
        horizon = p.DEFAULT_POLICY_HORIZON;
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
     * @brief Generative model
     *
     * Perform a call to the generative model.
     * @param {const state &} s; state
     * @param {std::shared_ptr<action>} a; copy of the action
     * @param {state &} s_p; next state
     */
    void generative_model(
        const state &s,
        std::shared_ptr<action> a,
        state &s_p)
    {
        envt->state_transition(s,a,s_p);
        ++nb_calls;
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
        std::shared_ptr<action> a = v->get_incoming_action();
        state s = (v->parent)->get_state_or_last();
        state s_p;
        generative_model(s,a,s_p);
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
        std::shared_ptr<action> nodes_action = v.get_next_expansion_action();
        state nodes_state = v.get_state_or_last();
        state new_state;
        generative_model(nodes_state,nodes_action,new_state);
        v.create_child(
            nodes_action,
            new_state,
            envt->action_space //TODO: warning - stochastic case
        );
        return v.get_last_child();
    }

    /**
     * @brief UCT child
     *
     * UCT selection method for the tree policy.
     * @param {node &} v; parent node
     * @return Return the selected child according to the UCT formula
     */
    node * uct_child(node &v) {
        std::vector<double> uct_scores;
        for(auto &elt : v.children) {
            assert(elt.get_visits_count() != 0 && expd_counter > 0);
            uct_scores.emplace_back(
                elt.get_value() + 2 * uct_cst *
                sqrt(log((double) expd_counter)/ ((double) elt.get_visits_count()))
            );
        }
        unsigned ind = argmax(uct_scores);
        return &v.children.at(ind);
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
            node * v_p = uct_child(v);
            sample_new_state(v_p);
            return tree_policy(*v_p);
        }
    }

    /**
     * @brief Default policy
     *
     * Compute the total return by running an episode with the default policy.
     * The simulation starts from the last sampled state of the input node.
     * @param {node *} ptr; pointer to the input node
     */
    double default_policy(node * ptr) {
        state s = ptr->get_last_sampled_state();
        if(is_node_terminal(*ptr)) {
            std::shared_ptr<action> a(new navigation_action()); // default action
            return envt->reward_function(s,a,s);
        }
        double total_return = 0.;
        std::shared_ptr<action> a = rndplc(s);
        for(unsigned t=0; t<horizon; ++t) {
            state s_p;
            generative_model(s,a,s_p);
            total_return += pow(discount_factor,(double)t) * envt->reward_function(s,a,s_p);
            if(envt->is_terminal(s)) { // Termination criterion
                break;
            }
            s = s_p;
            a = rndplc(s);
        }
        return total_return;
    }

    /**
     * @brief Backup method
     *
     * Increment all the visited nodes visits counters and update their values w.r.t. the
     * given discounted return.
     * This method is recursive.
     * @param {double &} total_return; return to be backed up, iteratively discounted
     * @param {node *} ptr; pointer to the node, first the leaf node, then to the parents
     */
    void backup(double &total_return, node * ptr) {
        ptr->increment_visits_count();
        ptr->add_to_value(total_return);
        total_return *= discount_factor; // apply the discount for the parent node
        total_return += envt->reward_function( // add the reward of the transition
            ptr->parent->get_state_or_last(),
            ptr->get_incoming_action(),
            ptr->get_last_sampled_state()
        );
        if(!ptr->parent->is_root()) {
            backup(total_return,ptr->parent);
        }
    }

    /**
     * @brief Build UCT tree
     *
     * Build a tree wrt vanilla UCT algorithm.
     * The tree is kept in memory.
     * @param {const state &} s; current state of the agent
     */
    void build_uct_tree(const state &s) {
        root_node.clear_node();
        root_node.set_as_root();
        root_node.set_state(s);
        root_node.set_action_space(envt->get_action_space(s));
        expd_counter = 0;
        for(unsigned i=0; i<budget; ++i) {
            node *ptr = tree_policy(root_node);
            double total_return = default_policy(ptr);
            backup(total_return,ptr);
            ++expd_counter;
        }
    }

    /**
     * @brief Argmax of the score
     *
     * Get the indice of the child achieving the best score.
     * @param {const node &} v; node of the tree
     * @return Return the indice of the child achieving the best score.
     */
    unsigned argmax_score(const node &v) {
        std::vector<double> values;
        for(auto &ch: v.children) {
            values.push_back(ch.get_value());
        }
        return argmax(values);
    }

    /**
     * @brief Get the recommended action at a certain node
     *
     * This is the policy decision after the tree construction (recommended action).
     * It gets the greedy action wrt the values of the subsequent nodes.
     * @param {const node &} v; root node of the tree
     * @return Return the action with the highest score (leading to the child with the higher
     * value).
     */
    std::shared_ptr<action> get_recommended_action(const node &v) {
        return v.get_action_at(argmax_score(v));
    }

    /**
     * @brief Policy operator
     *
     * Policy operator for the undertaken action at given state.
     * @param {const state &} s; given state
     * @return Return the undertaken action at s.
     */
	std::shared_ptr<action> operator()(const state &s) {
        build_uct_tree(s);
        return get_recommended_action(root_node);
	}

    /**
     * @brief Process reward
     *
     * Process the resulting reward from transition (s,a,s_p)
     * @param {state &} s; state
     * @param {std::shared_ptr<action> &} a; action
     * @param {state &} s_p; next state
     */
    void process_reward(
        const state & s,
        const std::shared_ptr<action> & a,
        const state & s_p)
    {
        (void) s;
        (void) a;
        (void) s_p;
    }

    /**
     * @brief Get backup
     *
     * Get the backed-up values.
     * @return Return a vector containing the values to be saved.
     */
    std::vector<double> get_backup() {
        return std::vector<double>{(double)nb_calls};
    }
};

#endif // UCT_POLICY_HPP_
