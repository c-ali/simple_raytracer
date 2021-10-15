#include <algorithm>
#include <memory>
#include "kd_tree.h"
#include "geometry.h"

using std::pair, std::shared_ptr, std::unique_ptr, std::vector, std::make_unique;

kd_tree::kd_tree(mesh &msh){
    for(int i = 0; i < depth; ++i){
    }
}

bool node::is_leaf(){
    return data.size() > 0;
}

node::node(vector<shared_ptr<triangle>> data, vector<shared_ptr<triangle>>::const_iterator start,vector<shared_ptr<triangle>>::const_iterator end){
    vector<shared_ptr<triangle>> new_data(start, end);
    data = new_data;
}

void node::split(int dim){
    std::sort(data.begin(), data.end(), [=](shared_ptr<triangle> a, shared_ptr<triangle> b) { return a->bounding_box().min[dim] < b->bounding_box().min[dim]; });
    int median_idx = (int) data.size() / 2;
    left = make_unique<node>(data, data.begin(), data.begin() + median_idx);
    right = make_unique<node>(data, data.begin() + median_idx + 1, data.end());
}
