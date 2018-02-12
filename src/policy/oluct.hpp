#ifndef OLUCT_POLICY_HPP_
#define OLUCT_POLICY_HPP_

#include <environment.hpp>
#include <node.hpp>
#include <random.hpp>

/**
 * @brief OLUCT policy
 */
template <class DFTPLC>
class oluct {
public:
    typedef DFTPLC DFTPLC_type;
    environment model; ///< Copy of the environment, used for action space reduction, termination criterion and generative model, also its attributes may be changed according to the used configuration
    node root_node; ///< Root node of the tree
    double uct_cst; ///< UCT constant within UCT formula
    double discount_factor; ///< MDP discount factor
    unsigned horizon; ///< Horizon for default policy
    unsigned budget; ///< Algorithm budget (number of expanded nodes)
    unsigned expd_counter; ///< Counter of the number of expanded nodes
    unsigned nb_calls; ///< Number of calls to the generative model
    DFTPLC dflt_policy; ///< Default policy

    /**
     * @brief Constructor
     *
     * Make use of the input parameters.
     * @param {const parameters &} p; parameters
     * @param {environment *} en; pointer to the environment, used for action space reduction,
     * termination criterion and generative model
     */
    oluct(const parameters &p, environment *en) :
        model(*en),
        root_node(state(),model.action_space), // initialise with default state
        dflt_policy(p,en)
    {
        model.misstep_probability = p.MODEL_MISSTEP_PROBABILITY;
        model.state_gaussian_stddev = p.MODEL_STATE_GAUSSIAN_STDDEV;
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
     * Still another state will be sampled in the tree policy method.
     * If the node is root, only the labelling state is tested.
     * @param {node &} v; tested node
     * @return Return 'true' if the node is considered terminal.
     */
    bool is_node_terminal(node &v) {
        if(v.is_root()) {
            return model.is_terminal(v.get_state());
        } else {
            for(auto &s: v.get_sampled_states()) {
                if(!model.is_terminal(s)) {
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
        model.state_transition(s,a,s_p);
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
            model.action_space //TODO: warning - stochastic case
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
        for(auto &c : v.children) {
            assert(c.get_visits_count() != 0 && expd_counter > 0);
            uct_scores.emplace_back(
                c.get_value() + 2 * uct_cst *
                sqrt(log((double) expd_counter)/ ((double) c.get_visits_count()))
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
     * @return Return the sampled total return.
     */
    double default_policy(node * ptr) {
        state s = ptr->get_last_sampled_state();
        if(is_node_terminal(*ptr)) {
            std::shared_ptr<action> a(new navigation_action()); // default action
            return model.reward_function(s,a,s);
        }
        double total_return = 0.;
        std::shared_ptr<action> a = dflt_policy(s);
        for(unsigned t=0; t<horizon; ++t) {
            state s_p;
            generative_model(s,a,s_p);
            total_return += pow(discount_factor,(double)t) * model.reward_function(s,a,s_p);
            if(model.is_terminal(s)) { // Termination criterion
                break;
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
        total_return += model.reward_function( // add the reward of the transition
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
     * @brief Policy operator
     *
     * Policy operator for the undertaken action at given state.
     * @param {const state &} s; given state
     * @return Return the undertaken action at s.
     */
	std::shared_ptr<action> operator()(const state &s) {
        model.step(s);
        //TODO: update the model inside the tree for wp -> probably adding table of reached wp in state
        build_oluct_tree(s);
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
