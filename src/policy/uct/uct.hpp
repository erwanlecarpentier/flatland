#ifndef UCT_HPP_
#define UCT_HPP_

// Standard libraries
#include<cassert>
#include<vector>
#include<memory>
#include<numeric>

// My libraries
#include<uct/cnode.hpp>
#include<uct/dnode.hpp>
#include<utils.hpp>

/**
 * @brief UCT algorithm class
 */
template <class ST, class AC, class MD, class PL>
class uct {
public:
    MD model; ///< Generative model
    PL default_policy; ///< Default policy
    double discount_factor; ///< Discount factor
    double uct_parameter; ///< UCT parameter
    unsigned budget; ///< Budget ie number of expanded nodes in the tree
    unsigned global_counter; ///< Global counter of number of visits / nodes expansions
    unsigned horizon; ///< Horizon for the default policy simulation

    /**
     * @brief Constructor
     */
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

    /**
     * @brief Sample return
     *
     * Sample a return with the default policy
     */
    double sample_return(ST &s) {
        if(s.is_terminal()) {
            return 0.;
        }
        double total_return = 0.;
        AC a;
        default_policy(s,a);
        for(unsigned t=0; t<horizon; ++t) {
            ST s_p;
            model.state_transition(s,a,s_p);
            total_return += pow(discount_factor,(double)t) * model.reward_function(s,a,s_p);
            if(s_p.is_terminal()) {
                break;
            }
            s = s_p;
            default_policy(s,a);
        }
        return total_return;
    }

    /**
     * @brief Update value
     */
    void update_value(cnode<ST,AC> * ptr, double q) {
        ptr->sampled_returns.push_back(q);
    }

    /**
     * @brief Select child wrt UCT tree policy
     *
     * The node must be fully expanded.
     */
    cnode<ST,AC> * select_child(dnode<ST,AC> * v) {
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
    double evaluate(dnode<ST,AC> * v) {
        ST s_p;
        global_counter++; // a chance node will be created
        model.state_transition(v->s,v->create_child(),s_p);
        double q = sample_return(s_p);
        update_value(v->children.back().get(),q);//TOCHECK that we have to do this
        return q;
    }

    /**
     * @brief Search tree
     */
    double search_tree(dnode<ST,AC> * v) {
        if(v->is_terminal()) { // terminal node
            return 0.;
        } else if(!v->is_fully_expanded()) { // leaf node, expand it
            return evaluate(v);
        } else { // apply UCT tree policy
            cnode<ST,AC> * ptr = select_child(v);
            ST s_p;
            model.state_transition(v->s,ptr->a,s_p);
            double r = model.reward_function(v->s,ptr->a,s_p);
            double q = 0.;
            unsigned ind = 0; // indice of resulting child
            if(ptr->is_state_already_sampled(s_p,ind)) { // go to node
                q = r + discount_factor * search_tree(ptr->children.at(ind).get());
            } else { // create a new node
                q = r + discount_factor * evaluate(ptr->create_child(s_p,s_p.get_available_actions()));
            }
            update_value(ptr,q);
            return q;
        }
        /* // Original implementation (slower)
        if(v->is_terminal()) { // terminal node
            return 0.;
        } else if(v->is_leaf()) { // leaf node, expand it
            return evaluate(v->s);
        } else { // apply UCT tree policy
            cnode<ST,AC> * ptr = select_child(v);
            ST s_p;
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
    void build_uct_tree(dnode<ST,AC> &root) {
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
    unsigned argmax_value(const dnode<ST,AC> &v) {
        std::vector<double> values;
        for(auto &ch: v.children) {
            values.emplace_back(ch->get_value());
        }
        return argmax(values);
    }

    /**
     * @brief Recommended action
     */
    AC recommended_action(const dnode<ST,AC> &v) {
        return v.children.at(argmax_value(v))->a;
    }

    /**
     * @brief UCT policy operator
     *
     * @param {ST &} s; current state of the agent
     * @param {AC &} a; recommended action after computation
     */
    void operator()(ST &s, AC &a) {
        dnode<ST,AC> root(s,s.get_available_actions(),nullptr);
        build_uct_tree(root);
        a = recommended_action(root);
    }
};

#endif // UCT_HPP_
