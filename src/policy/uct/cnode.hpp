#ifndef CNODE_HPP_
#define CNODE_HPP_

template <class ST, class AC> class dnode; // forward declaration

/**
 * @brief Chance node class
 */
template <class ST, class AC>
class cnode {
public:
    ST s; ///< Labelling state
    AC a; ///< Labelling action
    std::vector<std::unique_ptr<dnode<ST,AC>>> children; ///< Child nodes
    std::vector<double> sampled_returns; ///< Sampled returns
    //unsigned nvis; ///< Number of visits //TRM?

    /**
     * @brief Constructor
     */
    cnode(
        ST _s,
        AC _a) :
        s(_s),
        a(_a)
    {
        //nvis = 0;//TRM?
    }

    /**
     * @brief Create Child
     */
    dnode<ST,AC> * create_child(ST _s, std::vector<AC> _actions) {
        children.emplace_back(std::unique_ptr<dnode<ST,AC>>(new dnode<ST,AC>(_s,_actions,this)));
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
     * @param {ST &} s; sampled state
     * @param {unsigned &} ind; indice modified to the value of the indice of the existing
     * decision node with state s if the comparison succeeds.
     */
    bool is_state_already_sampled(ST &s, unsigned &ind) {
        for(unsigned i=0; i<children.size(); ++i) {
            if(s.is_equal_to(children[i]->s)) {
                ind = i;
                return true;
            }
        }
        return false;
    }
};

#endif // CNODE_HPP_
