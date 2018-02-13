#ifndef CNODE_HPP_
#define CNODE_HPP_

class dnode; // forward declaration

/**
 * @brief Chance node class
 */
class cnode {
public:
    state s; ///< Labelling state
    std::shared_ptr<action> a; ///< Labelling action
    std::vector<std::unique_ptr<dnode>> children; ///< Child nodes
    std::vector<double> sampled_returns; ///< Sampled returns
    //unsigned nvis; ///< Number of visits //TRM?

    /**
     * @brief Constructor
     */
    cnode(
        state _s,
        std::shared_ptr<action> _a) :
        s(_s),
        a(_a)
    {
        //nvis = 0;//TRM?
    }

    /**
     * @brief Create Child
     */
    /* //TRM outdated
    dnode * create_child(state _s, std::vector<std::shared_ptr<action>> _actions) {
        children.emplace_back(std::unique_ptr<dnode>(new dnode(_s,_actions,this)));
        return children.back().get();
    }
    */

    dnode * get_last_child() {
        return children.back().get();
    }

    /**
     * @brief Get value
     */
    double get_value() {
        //assert(nvis > 0); // must have been visited at least once//TRM?
        return (1. / (double) sampled_returns.size()) * std::accumulate(sampled_returns.begin(),sampled_returns.end(),0.0);
    }

    /**
     * @brief Is state already sampled
     *
     * Equality operator.
     * @param {state &} s; sampled state
     * @param {unsigned &} ind; indice modified to the value of the indice of the existing
     * decision node with state s if the comparison succeeds.
     */
    /*
    bool is_state_already_sampled(state &s, unsigned &ind) {
        for(unsigned i=0; i<children.size(); ++i) {
            if(s.is_equal_to(children[i]->s)) {
                ind = i;
                return true;
            }
        }
        return false;
    }
    */
};

#endif // CNODE_HPP_
