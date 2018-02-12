#ifndef DNODE_HPP_
#define DNODE_HPP_

#include<uct/cnode.hpp>

/**
 * @brief Decision node class
 */
template <class ST, class AC>
class dnode {
public:
    ST s; ///< Labelling state
    std::vector<AC> actions; ///< Available actions, iteratively removed
    cnode<ST,AC> * parent; ///< Pointer to parent node
    std::vector<std::unique_ptr<cnode<ST,AC>>> children; ///< Child nodes
    //unsigned nvis; ///< Number of visits//TRM?

    /**
     * @brief Constructor
     */
    dnode(
        ST _s,
        std::vector<AC> _actions,
        cnode<ST,AC> * _parent) :
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
    AC create_child() {
        unsigned indice = rand_indice(actions);
        AC sampled_action = actions.at(indice);
        actions.erase(actions.begin() + indice);
        children.emplace_back(
            std::unique_ptr<cnode<ST,AC>>(new cnode<ST,AC>(s,sampled_action))
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
        return children.at(this->argmax(children_values))->get_value();
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
