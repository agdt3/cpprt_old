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
RayObjectNode::RayObjectNode (Ray& new_ray, Object& new_obj, RayObjectNode *parent=NULL) : ray(new_ray) {
    key = encode_key(new_ray.id, new_obj.id);
    //ray = new_ray; //copy
    obj_ptr = &new_obj;
    parent = parent;
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
RayObjecTree::RayObjectTree(Ray ray&, Object obj&) {
    root = RayObjectNode(ray, object, NULL);
}

void RayObjecTree::insert(std::string& parent_key, RayObjectNode child) {
    if (root == NULL) {
        RayObjectNode node = new RayObjectNode();
        root = node;
    }
    RayObjectNode parent = search_by_object_id(key, root);
    if (parent != NULL) {
        parent->children.push(child);
    }
}

RayObjectNode* RayObjecTree::search_by_object_id(const std::string& key, RayObjectNode *node)  {
    if (compare_keys(key, node)) return node;
    if (node->children.size() == 0) return NULL;
    for (int i=0; i < node.children.size(); ++i) {
        search_by_object_id(key, node.children[i]);
    }
}

RayObjecTree::traverse_dfs(RayObjectNode node) {
    if (node.children.size() == 0) return;
    node.print();
    for (int i=0; i < node.children.size(); ++i) {
       traverse_dfs(node.children[i]);
    }
}

//compare on object type, object id, ray type, ray id or some subset
bool RayObjecTree::compare_internal_ids(const std::string& param, int& id, RayObjectNode& node) {
    if (param == "ray" && id = node.ray.id) ||
       (param == "obj" && id == node.obj.id) {
           return true;
    }
    return false;
}

bool RayObjecTree::compare_keys(const std::string& key, const RayObjectNode& node) {
    if (node.id == key) return true;
    return false;
}

void RayObjecTree::traverse_bfs(RayObjectNode node) {
    //TODO: Usually requires a queue
    // is there a recursive queueless implementation of bfs?
}

