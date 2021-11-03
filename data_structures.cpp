#include <algorithm>
#include "data_structures.h"
#include <iostream>

kd_tree::kd_tree(std::vector<std::shared_ptr<surface>> &objs, unsigned depth, size_t min_node_size, unsigned max_depth){
    upper = NULL;
    lower = NULL;
    this->split_dim = depth % 3;
    size_t num_obj = objs.size();

    if(num_obj < min_node_size || depth == max_depth){
        node_objs = objs;
    }
    else{
        std::sort(objs.begin(), objs.end(), [this](std::shared_ptr<surface> t1, std::shared_ptr<surface> t2) { return t1->centroid()[split_dim] < t2->centroid()[split_dim]; });

        std::vector<std::shared_ptr<surface>> upper_list, lower_list;
        split_plane =  objs[num_obj/2]->centroid()[split_dim];

        upper_split_bbox = box();
        lower_split_bbox = box();

        for(size_t i = 0; i < num_obj; ++i){
            if(objs[i]->centroid()[split_dim] > split_plane){
                upper_list.push_back(objs[i]);
                upper_split_bbox = box_union(upper_split_bbox, objs[i]->bounding_box());
            }
            else{
                lower_list.push_back(objs[i]);
                lower_split_bbox = box_union(lower_split_bbox, objs[i]->bounding_box());
            }

        }
        if(upper_list.size() == 0 || lower_list.size() == 0){
            node_objs = objs;
        }
        else{
            upper = std::make_shared<kd_tree>(upper_list, depth + 1, min_node_size, max_depth);
            lower = std::make_shared<kd_tree>(lower_list, depth + 1, min_node_size, max_depth);
        }

    }
}
