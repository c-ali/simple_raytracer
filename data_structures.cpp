#include <algorithm>
#include "data_structures.h"
#include <iostream>

kd_tree::kd_tree(std::vector<std::shared_ptr<surface>> &objs, unsigned depth, size_t min_node_size, unsigned max_depth){
    upper = NULL;
    lower = NULL;
    this->split_dim = depth % 3;
    size_t num_obj = objs.size();

    if(num_obj < min_node_size || depth == max_depth)
        node_objs = objs;
    else{
        std::sort(objs.begin(), objs.end(), [this](std::shared_ptr<surface> t1, std::shared_ptr<surface> t2) { return t1->centroid()[split_dim] < t2->centroid()[split_dim]; });

        std::vector<std::shared_ptr<surface>> upper_list, lower_list;
        split_plane =  objs[num_obj/2]->centroid()[split_dim];

        for(size_t i = 0; i < num_obj; ++i){
            bool sort_into_upper = true;
            bool sort_into_lower = true;
            if(objs[i]->centroid()[split_dim] > split_plane){
                upper_list.push_back(objs.at(i));
                upper_split_bbox = box_union(upper_split_bbox, objs.at(i)->bounding_box());
            }
            else{
                lower_list.push_back(objs.at(i));
                lower_split_bbox = box_union(lower_split_bbox, objs.at(i)->bounding_box());
                //upper_split_lbound = std::max(upper_split_lbound, objs[i]->bounding_box().max[split_dim]);            }
            }
            if(!(sort_into_lower || sort_into_upper))
                std::cout<<"test"<<std::endl;

            //upper_split_lbound = -1E30f;
            //lower_split_ubound = 1E30f;
            upper_split_bbox = box();
            lower_split_bbox = box();


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
