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
    double depth; ///< Depth

    /**
     * @brief Constructor
     */
    cnode(
        state _s,
        std::shared_ptr<action> _a,
        double _depth = 0) :
        s(_s),
        a(_a),
        depth(_depth)
    {
        //
    }

    dnode * get_last_child() const {
        return children.back().get();
    }

    /**
     * @brief Get value
     *
     * Get the value of the node.
     * This is the mean of the sampled returns.
     * @return Return the value of the node.
     */
    double get_value() const {
        return (1. / (double) sampled_returns.size()) * std::accumulate(sampled_returns.begin(),sampled_returns.end(),0.0);
    }
};

#endif // CNODE_HPP_
