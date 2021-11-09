#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <memory>
#include <vector>
#include "geometry.h"

class kd_tree
{
public:
    float split_plane;
    unsigned split_dim;
    std::vector<std::shared_ptr<surface>> node_objs;
};


class basic_kd_tree : public kd_tree
{
public:
    std::shared_ptr<basic_kd_tree> upper, lower;
    box upper_split_bbox, lower_split_bbox;
    basic_kd_tree(std::vector<std::shared_ptr<surface>> &objs, unsigned depth = 0, size_t min_node_size = 2, unsigned max_depth = 10);
};

class fast_kd_tree : public  kd_tree
{
public:
    std::shared_ptr<fast_kd_tree> upper, lower;
    float lower_node_upper_bound, upper_node_lower_bound;
    fast_kd_tree(std::vector<std::shared_ptr<surface>> &objs, unsigned depth = 0, size_t min_node_size = 2, unsigned max_depth = 10);
};

#endif // DATA_STRUCTURES_H
