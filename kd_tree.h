#ifndef KD_TREE_H
#define KD_TREE_H
#include <memory>
#include <vector>
#include "geometry.h"

using std::pair, std::unique_ptr, std::vector;

class kd_tree
{
public:
    kd_tree();
};

class node
{
    unique_ptr<node> left, right = nullptr;
    vector<triangle> data;
public:
    node();
    bool is_leaf();
};

#endif // KD_TREE_H
