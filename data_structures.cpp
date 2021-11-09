#include <algorithm>
#include <iostream>
#include "data_structures.h"

basic_kd_tree::basic_kd_tree(std::vector<std::shared_ptr<surface>> &objs, unsigned depth, size_t min_node_size, unsigned max_depth){
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
            upper = std::make_shared<basic_kd_tree>(upper_list, depth + 1, min_node_size, max_depth);
            lower = std::make_shared<basic_kd_tree>(lower_list, depth + 1, min_node_size, max_depth);
        }

    }
}


fast_kd_tree::fast_kd_tree(std::vector<std::shared_ptr<surface>> &objs, unsigned depth, size_t min_node_size, unsigned max_depth){
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
        lower_node_upper_bound = -1E30f;
        upper_node_lower_bound = 1E30f;

        split_plane =  objs[num_obj/2]->centroid()[split_dim];



        for (size_t i=0; i<num_obj; i++) {
                float centroidCoord = objs[i]->centroid()[split_dim];
                bool sortIntoUpper = false;
                if (centroidCoord >= split_plane) {
                        sortIntoUpper = true;
                }

                if (sortIntoUpper) {
                        upper_list.push_back(objs[i]);
                        upper_node_lower_bound = std::min(upper_node_lower_bound, objs[i]->bounding_box().min[split_dim]);
                } else {
                        lower_list.push_back(objs[i]);
                        lower_node_upper_bound = std::max(lower_node_upper_bound, objs[i]->bounding_box().max[split_dim]);
                }
        }
        if (lower_list.size() == objs.size() || upper_list.size() == objs.size()) {
                node_objs = objs;
        } else {
                if (!lower_list.empty()) lower = std::make_shared<fast_kd_tree>(lower_list, depth + 1, min_node_size, max_depth);
                if (!upper_list.empty()) upper = std::make_shared<fast_kd_tree>(upper_list, depth + 1, min_node_size, max_depth);

        }

    }
}

