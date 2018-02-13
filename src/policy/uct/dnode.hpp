#ifndef DNODE_HPP_
#define DNODE_HPP_

#include<uct/cnode.hpp>

/**
 * @brief Decision node class
 */
class dnode {
public:
    state s; ///< Labelling state
    std::vector<std::shared_ptr<action>> actions; ///< Available actions, iteratively removed
    cnode * parent; ///< Pointer to parent node
    std::vector<std::unique_ptr<cnode>> children; ///< Child nodes
    //unsigned nvis; ///< Number of visits//TRM?

    /**
     * @brief Constructor
     */
    dnode(
        state _s,
        std::vector<std::shared_ptr<action>> _actions,
        cnode * _parent) :
        s(_s),
        actions(_actions),
        parent(_parent)
    {
        //nvis = 0;//TRM?
    }

    /**
     * @brief Create Child
     *
     * @return Return the sampled action.
     * @warning Remove the sampled action from the actions vector.
     */
    std::shared_ptr<action> create_child() {
        unsigned indice = rand_indice(actions);
        std::shared_ptr<action> sampled_action = actions.at(indice);
        actions.erase(actions.begin() + indice);
        children.emplace_back(
            std::unique_ptr<cnode>(new cnode(s,sampled_action))
        );
        return sampled_action;
    }

    /**
     * @brief Get children values
     */
    std::vector<double> get_children_values() {
        std::vector<double> children_values;
        for(auto &c : children) {
            children_values.emplace_back(c->get_value());
        }
        return children_values;
    }

    /**
     * @brief Get value
     */
    double get_value() {
        //assert(nvis > 0); // must have been visited at least once//TRM?
        std::vector<double> children_values = get_children_values();
        return children.at(argmax(children_values))->get_value();
    }

    /**
     * @brief Is fully expanded
     *
     * Node is a leaf.
     */
    bool is_fully_expanded() {
        return (actions.size() == 0);
    }
};

#endif // DNODE_HPP_
