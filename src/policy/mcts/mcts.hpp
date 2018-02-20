#ifndef UCT_HPP_
#define UCT_HPP_

#include<cassert>
#include<vector>
#include<memory>
#include<numeric>

#include<mcts/cnode.hpp>
#include<mcts/dnode.hpp>
#include<utils.hpp>

/**
 * @brief MCTS algorithm class
 */
template <class MD, class PL>
class mcts {
public:
    typedef MD MD_type;
    typedef PL PL_type;

    MD model; ///< Generative model
    PL default_policy; ///< Default policy
    double discount_factor; ///< Discount factor
    double uct_parameter; ///< UCT parameter
    double terminal_state_value = 0.; ///< Terminal state value
    unsigned budget; ///< Budget ie number of expanded nodes in the tree
    unsigned nb_cnodes; ///< Number of expanded nodes
    unsigned nb_calls; ///< Number of calls to the generative model
    unsigned horizon; ///< Horizon for the default policy simulation
    unsigned mcts_strategy_switch; ///< Strategy switch for MCTS algorithm
    bool is_model_dynamic; ///< Is the model dynamic

    /**
     * @brief Constructor
     */
    mcts(const parameters &p) : model(p), default_policy(p) {
        // use the specific parameters of the given model
        model.misstep_probability = p.MODEL_MISSTEP_PROBABILITY;
        model.state_gaussian_stddev = p.MODEL_STATE_GAUSSIAN_STDDEV;
        nb_cnodes = 0;
        nb_calls = 0;
        uct_parameter = p.UCT_CST;
        budget = p.TREE_SEARCH_BUDGET;
        discount_factor = p.DISCOUNT_FACTOR;
        horizon = p.DEFAULT_POLICY_HORIZON;
        is_model_dynamic = p.IS_MODEL_DYNAMIC;
        mcts_strategy_switch = p.MCTS_STRATEGY_SWITCH;
    }

    /**
     * @brief Generative model
     *
     * Perform a call to the generative model.
     * Increment the number of calls.
     * @param {const state &} s; state
     * @param {std::shared_ptr<action>} a; copy of the action
     * @param {state &} s_p; next state
     */
    state generative_model(const state &s, std::shared_ptr<action> a, MD &mod) {
        ++nb_calls;
        state s_p;
        mod.state_transition(s,a,s_p);
        return s_p;
    }

    /**
     * @brief Sample return
     *
     * Sample a return with the default policy starting at the input state.
     * @param {state} s; input state
     * @return Return the sampled return.
     */
    double sample_return(cnode * ptr, MD &mod) {
        if(mod.is_terminal(ptr->s)) {
            return terminal_state_value;
        }
        double total_return = 0.;
        state s = ptr->s;
        std::shared_ptr<action> a = ptr->a;
        for(unsigned t=0; t<horizon; ++t) {
            state s_p = generative_model(s,a,mod);
            total_return += pow(discount_factor,(double)t) * mod.reward_function(s,a,s_p);
            if(is_model_dynamic) {
                mod.step(s_p);
            }
            if(mod.is_terminal(s_p)) {
                break;
            }
            s = s_p;
            a = default_policy(s);
        }
        return total_return;
    }

    /**
     * @brief Update value
     *
     * Update the value of a chance node by stacking a new sampled value.
     * @param {cnode *} ptr; pointer to the updated chance node
     * @param {double} q; new sampled value
     */
    void update_value(cnode * ptr, double q) const {
        ptr->sampled_returns.push_back(q);
    }

    /**
     * @brief MCTS strategy
     *
     * Select child of a decision node wrt the MCTS strategy.
     * The node must be fully expanded.
     * @param {dnode *} v; decision node
     * @return Return to the select child, which is a chance node.
     */
    cnode * mcts_strategy(dnode * v) const {
        return v->children.at(rand_indice(v->children)).get();
    }

    /**
     * @brief UCT strategy
     *
     * Select child of a decision node wrt the UCT strategy.
     * The node must be fully expanded.
     * @param {dnode *} v; decision node
     * @return Return to the select child, which is a chance node.
     */
    cnode * uct_strategy(dnode * v) const {
        std::vector<double> children_uct_scores;
        for(auto &c : v->children) {
            children_uct_scores.emplace_back(
                c->get_value()
                + 2 * uct_parameter *
                sqrt(log((double) nb_cnodes) / ((double) c->sampled_returns.size()))
            );
        }
        return v->children.at(argmax(children_uct_scores)).get();
    }

    /**
     * @brief TUCT strategy
     *
     * Select child of a decision node wrt the TUCT strategy.
     * The node must be fully expanded.
     * @param {dnode *} v; decision node
     * @return Return to the select child, which is a chance node.
     */
    cnode * tuct_strategy(dnode * v) const {
        //TODO
    }

    /**
     * @brief Select child
     *
     * Select child of a decision node wrt one of the implemented strategies.
     * The node must be fully expanded.
     * @param {dnode *} v; decision node
     * @return Return to the select child, which is a chance node.
     */
    cnode * select_child(dnode * v) const {
        switch(mcts_strategy_switch) {
            case 0: { // UCT
                return uct_strategy(v);
            }
            case 1: { // TUCT
                return tuct_strategy(v);
            }
            default: { // Vanilla MCTS
                return mcts_strategy(v);
            }
        }
    }

    /**
     * @brief Evaluate
     *
     * Create a new child node to a decision node and sample a return value with the default
     * policy.
     * @param {dnode *} v; pointer to the decision node
     * @return Return the sampled value.
     */
    double evaluate(dnode * v, MD &mod) {
        nb_cnodes++; // a chance node will be created
        v->create_child();
        double q = sample_return(v->children.back().get(),mod);
        update_value(v->children.back().get(),q);
        return q;
    }

    /**
     * @brief Is state already sampled
     *
     * Equality operator.
     * @param {cnode *} ptr; pointer to the chance node
     * @param {state &} s; sampled state
     * @param {unsigned &} ind; indice modified to the value of the indice of the existing
     * decision node with state s if the comparison succeeds.
     */
    bool is_state_already_sampled(cnode * ptr, state &s, unsigned &ind) const {
        for(unsigned i=0; i<ptr->children.size(); ++i) {
            if(s.is_equal_to(ptr->children[i]->s)) {
                ind = i;
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Search tree
     *
     * Search within the tree, starting from the input decision node.
     * Recursive method.
     * @param {dnode *} v; input decision node
     * @return Return the sampled return at the given decision node
     */
    double search_tree(dnode * v, MD &mod) {
        if(mod.is_terminal(v->s)) { // terminal node
            return terminal_state_value;
        } else if(!v->is_fully_expanded()) { // leaf node, expand it
            return evaluate(v, mod);
        } else { // apply tree policy
            cnode * ptr = select_child(v);
            state s_p = generative_model(v->s,ptr->a,mod);
            double r = mod.reward_function(v->s,ptr->a,s_p);
            if(is_model_dynamic) {
                mod.step(s_p);
            }
            double q = 0.;
            unsigned ind = 0; // indice of resulting child
            if(is_state_already_sampled(ptr,s_p,ind)) { // go to node
                q = r + discount_factor * search_tree(ptr->children.at(ind).get(), mod);
            } else { // leaf node, create a new node
                ptr->children.emplace_back(std::unique_ptr<dnode>(
                    new dnode(s_p,mod.get_action_space(s_p),ptr->depth+1)
                ));
                q = r + discount_factor * evaluate(ptr->get_last_child(), mod);
            }
            update_value(ptr,q);
            return q;
        }
        /* // Original implementation (slower)
        if(mod.is_terminal(v->s)) { // terminal node
            return 0.;
        } else if(v->is_leaf()) { // leaf node, expand it
            return evaluate(v->s);
        } else { // apply tree policy
            cnode<state,AC> * ptr = select_child(v);
            state s_p = generative_model(v->s,ptr->a,mod);
            double r = mod.reward_function(v->s,ptr->a,s_p);
            ptr->children.emplace_back(std::unique_ptr<dnode>(
                new dnode(s_p,mod.get_action_space(s_p),ptr->depth+1)
            );
            double q = r + discount_factor * search_tree(ptr->get_last_child());
            update_value(ptr,q);
            return q;
        }
        */
    }

    /**
     * @brief Build tree
     *
     * Build a tree at the input root node.
     * @param {dnode &} root; reference to the input root node
     */
    void build_tree(dnode &root) {
        for(unsigned i=0; i<budget; ++i) {
            MD mod = model.get_copy();
            search_tree(&root, mod);
        }
        nb_cnodes = 0;
    }

    /**
     * @brief Argmax value
     *
     * Get the indice of the child with the maximum value of an input decision node.
     * @param {const dnode &} v; input decision node
     * @return Return the indice of the child with the maximum value
     */
    unsigned argmax_value(const dnode &v) const {
        std::vector<double> values;
        for(auto &ch: v.children) {
            values.emplace_back(ch->get_value());
        }
        return argmax(values);
    }

    /**
     * @brief Recommended action
     *
     * Get the recommended action from an input decision node.
     * @param {const dnode &} v; input decision node
     * @return Return the recommended action at the input decision node.
     */
    std::shared_ptr<action> recommended_action(const dnode &v) {
        return v.children.at(argmax_value(v))->a;
    }

    /**
     * @brief MCTS policy operator
     *
     * Policy operator for the undertaken action at given state.
     * @param {const state &} s; current state of the agent
     * @return Return the undertaken action at s.
     */
    std::shared_ptr<action> operator()(const state &s) {
        dnode root(s,model.get_action_space(s));
        build_tree(root);
        model.step(s); // update the model
        return recommended_action(root);
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
        /* No reward processing for MCTS policy */
    }

    /**
     * @brief Get backup
     *
     * Get the backed-up values.
     * @return Return a vector containing the values to be saved.
     */
    std::vector<double> get_backup() const {
        return std::vector<double>{(double)nb_calls};
    }
};

#endif // UCT_HPP_
