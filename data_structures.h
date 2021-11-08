#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <memory>
#include "geometry.h"



class kd_tree
{
public:
    float split_plane;
    box upper_split_bbox, lower_split_bbox;
    unsigned split_dim;
    std::shared_ptr<kd_tree> upper, lower;
    std::vector<std::shared_ptr<surface>> node_objs;
    kd_tree(std::vector<std::shared_ptr<surface>> &objs, unsigned depth = 0, size_t min_node_size = 2, unsigned max_depth = 10);
};

#endif // DATA_STRUCTURES_H
