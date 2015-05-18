#include "src/tracker.hpp"
#include <iostream>
#include <sstream>

//util
std::string _int_to_str (int& val) {
    std::ostringstream convert;
    convert << val;
    return convert.str();
}

int _str_to_int (std::string& str) {
    int val;
    std::istringstream convert(str);
    convert >> val;
    return val;
}

void _key_decomposition(std::string& key, std::string& sstr1, std::string& sstr2) {
    std::size_t separator = key.find("_");
    sstr1 = key.substr(0, separator + 1);
    sstr2 = key.substr(separator + 1);
}

//Ray Object Node
RayObjectNode::RayObjectNode (Ray& new_ray, Object& new_obj, RayObjectNode *par=NULL) : ray(new_ray) {
    key = encode_key(new_ray.id, new_obj.id);
    //ray = new_ray; //copy
    obj_ptr = &new_obj;
    parent = par;
}

std::string RayObjectNode::encode_key (int& rid, int& oid) {
    std::string str_rid, str_oid;
    str_rid = _int_to_str(rid);
    str_oid = _int_to_str(oid);
    return str_rid + "_" + str_oid;
}

void RayObjectNode::decode_key(std::string& key, int& rid, int &oid) {
    std::string str_rid, str_oid;
    _key_decomposition(key, str_rid, str_oid);
    rid = _str_to_int(str_rid);
    oid = _str_to_int(str_oid);
}

void RayObjectNode::print() {
    std::cout << ray;
    std::cout << obj_ptr;
}

//Ray Object Tree
RayObjectTree::RayObjectTree(RayObjectNode& ron) {
    root = &ron;
    size++;
}

RayObjectTree::RayObjectTree(Ray& ray, Object& obj) {
    root = new RayObjectNode(ray, obj, NULL);
    size++;
}

void RayObjectTree::insert(std::string& parent_key, RayObjectNode child) {
    if (root == NULL) {
        root = &child; //I hope this makes a copy!
        size++;
    }
    RayObjectNode *parent = search_by_object_id(parent_key, root);
    if (parent) {
        parent->children.push_back(&child);
        size++;
    }
}

RayObjectNode* RayObjectTree::search_by_object_id(const std::string& key, RayObjectNode *node)  {
    if (compare_keys(key, node->key)) return node;
    if (node->children.size() == 0) return NULL;
    for (int i=0; i < node->children.size(); ++i) {
        search_by_object_id(key, node->children[i]);
    }
}

void RayObjectTree::traverse_dfs(RayObjectNode node) {
    if (node.children.size() == 0) return;
    node.print();
    for (int i=0; i < node.children.size(); ++i) {
       traverse_dfs(*node.children[i]);
    }
}

//compare on object type, object id, ray type, ray id or some subset
/*
bool RayObjectTree::compare_internal_ids(const std::string& param, int& id, RayObjectNode& node) {
    if ((param == "ray" && id == node.ray.id) ||
       (param == "obj" && id == node.obj_ptr->id)) {
           return true;
    }
    return false;
}*/

//bool RayObjectTree::compare_keys(const std::string& key, const RayObjectNode& node) {
//static
bool RayObjectTree::compare_keys(const std::string& key1, const std::string& key2) {
    if (key1 == key2) return true;
    return false;
}

//static
bool RayObjectTree::compare_rays(const Ray& ray1, const Ray& ray2) {
    //not sure how reliable ID comp is
    //this should probably be an overload on ray == method
    if (ray1.id == ray2.id) return true;

    if ((ray1.origin == ray2.origin) &&
        (ray1.direction == ray2.direction) &&
        (ray1.type == ray2.type)) {
        return true;
    }
    return false;
}


void RayObjectTree::traverse_bfs(RayObjectNode node) {
    //TODO: Usually requires a queue
    // is there a recursive queueless implementation of bfs?
}

