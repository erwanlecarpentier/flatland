#ifndef OLUCT_POLICY_HPP_
#define OLUCT_POLICY_HPP_

#include <environment.hpp>
#include <node.hpp>
#include <random.hpp>

/**
 * @brief OLUCT policy
 */
template <class PL>
class oluct {
public:
    typedef PL PL_type;

    PL dflt_policy; ///< Default policy
    environment model; ///< Copy of the environment, used for action space reduction, termination criterion and generative model, also its attributes may be changed according to the used configuration
    node root_node; ///< Root node of the tree
    double uct_cst; ///< UCT constant within UCT formula
    double discount_factor; ///< MDP discount factor
    unsigned horizon; ///< Horizon for default policy
    unsigned budget; ///< Algorithm budget (number of expanded nodes)
    unsigned expd_counter; ///< Counter of the number of expanded nodes
    unsigned nb_calls; ///< Number of calls to the generative model
    bool is_model_dynamic; ///< Is the model dynamic

    /**
     * @brief Constructor
     *
     * Construct wrt the given parameters.
     * @param {const parameters &} p; parameters
     */
    oluct(const parameters &p) :
        dflt_policy(p),
        model(p),
        root_node(state(),model.action_space) // initialise with default state
    {
        // use the specific parameters of the given model
        model.misstep_probability = p.MODEL_MISSTEP_PROBABILITY;
        model.state_gaussian_stddev = p.MODEL_STATE_GAUSSIAN_STDDEV;
        budget = p.TREE_SEARCH_BUDGET;
        expd_counter = 0;
        nb_calls = 0;
        uct_cst = p.UCT_CST;
        discount_factor = p.DISCOUNT_FACTOR;
        horizon = p.DEFAULT_POLICY_HORIZON;
        is_model_dynamic = p.IS_MODEL_DYNAMIC;
    }

    /**
     * @brief Terminal node test
     *
     * A node is considered terminal if all of its states are terminal states.
     * Still another state will be sampled in the tree policy method.
     * If the node is root, only the labelling state is tested.
     * @param {node &} v; tested node
     * @return Return 'true' if the node is considered terminal.
     */
    bool is_node_terminal(node &v, environment &md) {
        if(v.is_root()) {
            return md.is_terminal(v.get_state());
        } else {
            for(auto &s: v.get_sampled_states()) {
                if(!md.is_terminal(s)) {
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
     * @return Return next state
     */
    state generative_model(
        const state &s,
        std::shared_ptr<action> a,
        environment &md)
    {
        ++nb_calls;
        state s_p;
        md.state_transition(s,a,s_p);
        return s_p;
    }

    /**
     * @brief State sampling
     *
     * Sample a new state w.r.t. to the incoming action and the parents state and add it to
     * the node.
     * @param {node *} v; pointer to the node
     * @return Return sampled state
     */
    state sample_new_state(node * v, environment &md) {
        assert(!v->is_root());
        std::shared_ptr<action> a = v->get_incoming_action();
        state s = (v->parent)->get_state_or_last();
        state s_p = generative_model(s,a,md);
        v->add_to_states(s_p);
        return s_p;
    }

    /**
     * @brief Expansion method
     *
     * Expand the node i.e. create a new leaf node.
     * @param {node &} v; reference on the expanded node
     * @return Return a pointer to the created leaf node
     */
    node * expand(node &v, environment &md) {
        std::shared_ptr<action> nodes_action = v.get_next_expansion_action();
        state nodes_state = v.get_state_or_last();
        state new_state = generative_model(nodes_state,nodes_action,md);
        v.create_child(
            nodes_action,
            new_state,
            md.action_space //TODO: warning - stochastic case
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
    node * uct_child(node &v) const {
        std::vector<double> uct_scores;
        for(auto &c : v.children) {
            assert(c.get_visits_count() != 0 && expd_counter > 0);
            uct_scores.emplace_back(
                c.get_value() + 2 * uct_cst *
                sqrt(log((double) expd_counter)/ ((double) c.get_visits_count()))
            );
        }
        return &v.children.at(argmax(uct_scores));
    }

    /**
     * @brief Tree policy
     *
     * Apply the tree policy.
     * Recursive method.
     * @param {node &} v;
     * @return Return a pointer to the created leaf node or to the current node if terminal.
     */
    node * tree_policy(node &v, environment &md) {
        if(is_node_terminal(v,md)) { // terminal
            sample_new_state(&v,md);
            return &v;
        } else if(!v.is_fully_expanded()) { // expand node
            return expand(v,md);
        } else { // apply UCT tree policy
            node * v_p = uct_child(v);
            if(is_model_dynamic) {
                md.step(sample_new_state(v_p,md));
            }
            return tree_policy(*v_p,md);
        }
    }

    /**
     * @brief Default policy
     *
     * Compute the total return by running an episode with the default policy.
     * The simulation starts from the last sampled state of the input node.
     * @param {node *} ptr; pointer to the input node
     * @return Return the sampled total return.
     */
    double default_policy(node * ptr, environment &md) {
        state s = ptr->get_last_sampled_state();
        if(is_node_terminal(*ptr,md)) {
            std::shared_ptr<action> a(new navigation_action()); // default action
            return md.reward_function(s,a,s);
        }
        double total_return = 0.;
        std::shared_ptr<action> a = dflt_policy(s);
        for(unsigned t=0; t<horizon; ++t) {
            state s_p = generative_model(s,a,md);
            total_return += pow(discount_factor,(double)t) * md.reward_function(s,a,s_p);
            if(md.is_terminal(s_p)) { // Termination criterion
                break;
            }
            if(is_model_dynamic) {
                md.step(s_p);
            }
            s = s_p;
            a = dflt_policy(s);
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
        total_return += model.reward_function( // add the reward of the transition //TODO replace this like mcts algorithm implementation (recursive calls, no backup)
            ptr->parent->get_state_or_last(),
            ptr->get_incoming_action(),
            ptr->get_last_sampled_state()
        );
        if(!ptr->parent->is_root()) {
            backup(total_return,ptr->parent);
        }
    }

    void print_tree_base() {
        std::cout << std::endl;
        std::cout << "nbch: " << root_node.get_nb_children() << std::endl;
        std::cout << "V: ";
        for(auto &ch : root_node.children) {
            std::cout << ch.get_value() << " ";
        }
        std::cout << std::endl;
        std::cout << "A:\n";
        for(auto &ch : root_node.children) {
            ch.get_incoming_action()->print();
        }
    }

    /**
     * @brief Build OLUCT tree
     *
     * Build a tree wrt the OLUCT algorithm.
     * The tree is kept in memory.
     * @param {const state &} s; current state of the agent
     */
    void build_oluct_tree(const state &s) {
        root_node.clear_node();
        root_node.set_as_root();
        root_node.set_state(s);
        root_node.set_action_space(model.get_action_space(s));
        root_node.shuffle_action_space();
        expd_counter = 0;
        for(unsigned i=0; i<budget; ++i) {
            environment cp = model.get_copy();
            node *ptr = tree_policy(root_node,cp);
            double total_return = default_policy(ptr,cp);
            backup(total_return,ptr);
            ++expd_counter;
        }
    }

    /**
     * @brief Argmax of the score
     *
     * Get the indice of the child achieving the best score.
     * The score here has to be understood as the value estimate.
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
     * Get the greedy action wrt the values of the subsequent nodes.
     * The indice of the selected action given as argument is modified consequently.
     * @param {const node &} v; root node of the tree
     * @param {unsigned &} indice; indice of the selected action
     * @return Return the action with the highest score (leading to the child node with the
     * higher value).
     */
    std::shared_ptr<action> get_recommended_action(const node &v, unsigned &indice) {
        indice = argmax_score(v);
        return v.get_action_at(indice);
    }

    /**
     * @brief OLUCT policy operator
     *
     * Policy operator for the undertaken action at given state.
     * @param {const state &} s; current state of the agent
     * @return Return the undertaken action at s.
     */
	std::shared_ptr<action> operator()(const state &s) {
        build_oluct_tree(s);
        model.step(s); // update the model
        unsigned indice = 0;
        return get_recommended_action(root_node,indice);
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
