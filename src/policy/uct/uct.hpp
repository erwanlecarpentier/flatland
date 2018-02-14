#ifndef UCT_HPP_
#define UCT_HPP_

#include<cassert>
#include<vector>
#include<memory>
#include<numeric>

#include<uct/cnode.hpp>
#include<uct/dnode.hpp>
#include<utils.hpp>

/**
 * @brief UCT algorithm class
 */
template <class MD, class PL>
class uct {
public:
    typedef MD MD_type;
    typedef PL PL_type;

    MD model; ///< Generative model
    PL default_policy; ///< Default policy
    double discount_factor; ///< Discount factor
    double uct_parameter; ///< UCT parameter
    unsigned budget; ///< Budget ie number of expanded nodes in the tree
    unsigned global_counter; ///< Global counter of number of visits / nodes expansions
    unsigned horizon; ///< Horizon for the default policy simulation

    /**
     * @brief Constructor
     * @deprecated
     */
    /*
    uct(
        MD _model,
        PL _default_policy,
        double _discount_factor,
        double _uct_parameter,
        unsigned _budget,
        unsigned _horizon) :
        model(_model),
        default_policy(_default_policy),
        discount_factor(_discount_factor),
        uct_parameter(_uct_parameter),
        budget(_budget),
        horizon(_horizon)
    {
        global_counter = 0;
    }
    */

    /**
     * @brief Constructor
     */
    uct(const parameters &p, environment *en) :
        model(*en),
        default_policy(p,en)
    {
        global_counter = 0;
        uct_parameter = p.UCT_CST;
        budget = p.TREE_SEARCH_BUDGET;
        discount_factor = p.DISCOUNT_FACTOR;
        horizon = p.DEFAULT_POLICY_HORIZON;
    }

    /**
     * @brief Sample return
     *
     * Sample a return with the default policy
     */
    double sample_return(state &s) {
        if(model.is_terminal(s)) {
            return 0.;
        }
        double total_return = 0.;
        std::shared_ptr<action> a = default_policy(s);
        for(unsigned t=0; t<horizon; ++t) {
            state s_p;
            model.state_transition(s,a,s_p);
            total_return += pow(discount_factor,(double)t) * model.reward_function(s,a,s_p);
            if(model.is_terminal(s_p)) {
                break;
            }
            s = s_p;
            a = default_policy(s);
        }
        return total_return;
    }

    /**
     * @brief Update value
     */
    void update_value(cnode * ptr, double q) {
        ptr->sampled_returns.push_back(q);
    }

    /**
     * @brief Select child wrt UCT tree policy
     *
     * The node must be fully expanded.
     */
    cnode * select_child(dnode * v) {
        std::vector<double> children_uct_scores;
        for(auto &c : v->children) {
            children_uct_scores.emplace_back(
                c->get_value()
                + 2 * uct_parameter *
                sqrt(log((double) global_counter) / ((double) c->sampled_returns.size()))
            );
        }
        return v->children.at(argmax(children_uct_scores)).get();
    }

    /**
     * @brief Evaluate
     *
     * Create a new child node (chance node) and sample a return value with the default
     * policy.
     */
    double evaluate(dnode * v) {
        state s_p;
        global_counter++; // a chance node will be created
        model.state_transition(v->s,v->create_child(),s_p);
        double q = sample_return(s_p);
        update_value(v->children.back().get(),q);//TOCHECK that we have to do this
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
    bool is_state_already_sampled(cnode * ptr, state &s, unsigned &ind) {
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
     */
    double search_tree(dnode * v) {
        if(model.is_terminal(v->s)) { // terminal node
            return 0.;
        } else if(!v->is_fully_expanded()) { // leaf node, expand it
            return evaluate(v);
        } else { // apply UCT tree policy
            cnode * ptr = select_child(v);
            state s_p;
            model.state_transition(v->s,ptr->a,s_p);
            double r = model.reward_function(v->s,ptr->a,s_p);
            double q = 0.;
            unsigned ind = 0; // indice of resulting child
            if(is_state_already_sampled(ptr,s_p,ind)) { // go to node
                q = r + discount_factor * search_tree(ptr->children.at(ind).get());
            } else { // leaf node, create a new node
                ptr->children.emplace_back(std::unique_ptr<dnode>(new dnode(s_p,model.get_action_space(s_p),ptr)));
                q = r + discount_factor * evaluate(ptr->get_last_child());
            }
            update_value(ptr,q);
            return q;
        }
        /* // Original implementation (slower)
        if(model.is_terminal(v->s)) { // terminal node
            return 0.;
        } else if(v->is_leaf()) { // leaf node, expand it
            return evaluate(v->s);
        } else { // apply UCT tree policy
            cnode<state,AC> * ptr = select_child(v);
            state s_p;
            model.state_transition(v->s,ptr->a,s_p);
            double r = model.reward_function(v->s,ptr->a,s_p);
            double q = r + discount_factor * search_tree(ptr->create_child(s_p,s_p.get_available_actions())); //TRM
            update_value(ptr,q);
            return q;
        }
        */
    }

    /**
     * @brief Build UCT tree
     */
    void build_uct_tree(dnode &root) {
        for(unsigned i=0; i<budget; ++i) {
            search_tree(&root);
        }
        global_counter = 0;
    }

    /**
     * @brief Argmax value
     *
     * @return Return the indice of the child with the maximum value
     */
    unsigned argmax_value(const dnode &v) {
        std::vector<double> values;
        for(auto &ch: v.children) {
            values.emplace_back(ch->get_value());
        }
        return argmax(values);
    }

    /**
     * @brief Recommended action
     */
    std::shared_ptr<action> recommended_action(const dnode &v) {
        return v.children.at(argmax_value(v))->a;
    }

    /**
     * @brief UCT policy operator
     *
     * Policy operator for the undertaken action at given state.
     * @param {const state &} s; current state of the agent
     * @return Return the undertaken action at s.
     */
    std::shared_ptr<action> operator()(const state &s) {
        dnode root(s,model.get_action_space(s),nullptr);
        build_uct_tree(root);
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
        /* No reward processing for UCT policy */
    }

    /**
     * @brief Get backup
     *
     * Get the backed-up values.
     * @return Return a vector containing the values to be saved.
     */
    std::vector<double> get_backup() {
        return std::vector<double>{(double)global_counter};
    }
};

#endif // UCT_HPP_
