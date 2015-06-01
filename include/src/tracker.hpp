#include <vector>
#include <set>
#include <map>
#include <string>
#include "ray.h"
#include "objects.h"

#ifndef TRACKER_HPP
#define TRACKER_HPP



//Primarily I need to be able to differentiate between the ray types that have
//hit the object. This makes things like Tris maybe not as useful
//A tri could track all the objects hit by a set of rays generated at a single
//pixel origin. But it may not be able to determine the difference between
//the number of camera rays vs. the number of reflected rays that have


//I want to track
//per pixel, where each ray went and the offshoot rays it produced
//for each such ray, I want to know th object it slammed into
//and some other properties of that object

//Ray_Object struct defines the ray that hit or didn't
//and what it may have hit
//would like to filter by objects hit and pixel starting points

struct RayObjectNode
{
    std::string key; //composed of ray and object id
    //why ref and pointer?
    //rays get destroyed and are concrete
    //objects persist and are abstract
    Ray ray;
    Object *obj_ptr;
    RayObjectNode* parent;
    std::vector<RayObjectNode*> children;

    RayObjectNode(Ray&, Object&, RayObjectNode*);

    std::string encode_key(int&, int&);

    void decode_key(std::string&, int&, int&);

    void print();
};

class RayObjectTree
{
public:
    RayObjectNode *root;
    unsigned int size = 0;

    RayObjectTree();
    RayObjectTree(RayObjectNode&);
    RayObjectTree(Ray&, Object&);

    void insert(std::string&, RayObjectNode);
    RayObjectNode* search_by_object_id(const std::string&, RayObjectNode*);
    void traverse_dfs(RayObjectNode);
    bool compare_internal_ids(const std::string&, const int&, const struct RayObjectNode&);
    static bool compare_keys(const std::string& key1, const std::string& key2);
    static bool compare_rays(const Ray&, const Ray&);
    //TODO: Usually requires a queue
    // is there a recursive queueless implementation of bfs?
    void traverse_bfs(RayObjectNode node);
};


/*
class Tracker
{
public:
    //Will allow us to add multiple trees per pixel if required
    multimap<string, RayObjectTree> tree_map;

    //for future memoization
    RayObjectTree current_tree = NULL;

    Tracker(){};
    ~Tracker(){};

    void add_node(int x, int y, Ray ray, Object obj) {
        //TODO: memoize tree
        std::string key = encode_point_key(x, y);
        RayObjectTree tree = find_or_create_tree(key);
        RayObjectNode ron = RayObjectNode(ray, obj);
        tree.insert(parent_id, ron);
    }

    RayObjectTree* find_or_create_tree(const std::string& key) {
       if (multimap.find(key) == multimap.end()) {
            RayObjectTree* tree = build_tree();
            add_tree(key, tree);
            return tree;
        }
        else {
            return &multimap.find(key)->second;
        }
    }

    RayObjectTree* build_tree(std::string_key) {
        RayObjectTree tree = new RayObjectTree();
        return &tree;
    }

    void add_tree(std::string key, RayObjectTree rot) {
        multimap.insert(std::pair<string, RayObjectTree> (key, rot));
    }

    std::string encode_point_key(int& x, int& y) {
        std::string res_x, std::string res_y;
        ostringstream convert;
        convert << x;
        res_x = convert.str();
        convert << y;
        res_y = convert.str();
        return res_x + "_" + res_y;
    }

    void decode_point_key(std::string& key, int& x, int &y) {
        std::string str_x, std::string str_y;
        _key_decomposition(key, str_x, str_y);
        std::cout << str_x << " " << std_y << endl;

        istringstream convert(str_x)
        convert >> x;
        istringstream convert(str_y)
        convert >> y;
    }

    void encode_type_key(std::string& key, RayType rtype, ObjectType otype){
        std::string res_rt, std::string res_ot;
        ostringstream convert;
        convert << rtype; //will be some small int
        res_rt = convert.str();
        convert << otype;
        res_ot = convert.str();
        key = res_rt + "_" + res_ot;
    }

    //seems janky to convert to int
    void decode_type_key(std::string& key, int& rtype, int& otype) {
        std::string str_rt, std::string str_ot;
        _key_decomposition(key, str_rt, str_ot);
        std::cout << str_rt << " " << std_ot << endl;

        istringstream convert(str_rt)
        convert >> rtype;
        istringstream convert(str_ot)
        convert >> otype;
    }

    void _key_decomposition(std::string& key, std::string& sstr1, std::string& sstr2) {
        std::size_t separator = key.find("_");
        sstr1 = key.substr(0, separator + 1);
        sstr2 = key.substr(separator + 1);
    }

}

*/



/*
struct RayObjectNode
{
    int x, y; //starting pixel position
    set<ObjectType> object_set; //which objects has this set of rays hit
    set<RayType> ray_set; //what types of rays have been produced
    vector<RayIntersectionObject> rov; //all pairs of rays/objects
};

struct RayIntersectionObject
{
    Ray ray;
    Object obj;
};

class Tracker
{
public:
    int size;
    //a very naive container of objects
    //vector<RayObjectNode> container;

    //TODO: find a better way to encode and
    //decode keys
    multimap<string, RayIntersectionObject> rio_map;

    //object_type_key map
    //for each unique object, there is a key
    //that maps to the pixel value of the set of items
    //that intersected with it
    multimap<string, string> otk_map;

    //so this map should probably be a hash
    //that maps some combination of rayType_objectType
    //to a pixel key x_y
    multimap<string, string>;

    //multimap that maps a "x_y" key to
    //rayintersection objects
    //or a map that maps "x_y" to a vector of RayObjectNodes
    //gain simplicity but lose metadata
    Tracker() {
        size = 0;
    };

    void encode_point_key(std::string& key, int& x, int& y) {
        std::string res_x, std::string res_y;
        ostringstream convert;
        convert << x;
        res_x = convert.str();
        convert << y;
        res_y = convert.str();
        key = res_x + "_" + res_y;
    }

    void decode_point_key(std::string& key, int& x, int &y) {
        std::string str_x, std::string str_y;
        _key_decomposition(key, str_x, str_y);
        std::cout << str_x << " " << std_y << endl;

        istringstream convert(str_x)
        convert >> x;
        istringstream convert(str_y)
        convert >> y;
    }

    void encode_type_key(std::string& key, Ray& ray, Object& obj){
        std::string res_rt, std::string res_ot;
        ostringstream convert;
        convert << ray.type; //will be some small int
        res_rt = convert.str();
        convert << obj.type;
        res_ot = convert.str();
        key = res_rt + "_" + res_ot;
    }

    void _key_decomposition(std::string& key, std::string& sstr1, std::string& sstr2) {
        std::size_t separator = key.find("_");
        sstr1 = key.substr(0, separator + 1);
        sstr2 = key.substr(separator + 1);
    }

    void insert(int& x, int& y, Ray& ray, Object& object) {
        RayIntersectionObject rio = RayIntersectionObject();
        rio.ray = ray;
        rio.obj = object;
        string key;
        encode_point_key(key, x, y);
        rio_map.insert(std::pair<string, RayIntersectionObject>(key, rio));
    }

    void print_value(int& x, int& y) {
        string key;
        encode_point_key(key, x, y);

        //get thing from map
    }

    RayObjectNode find_by_pixel(int x, int y) {
        //crappy linear search
        for(int i=0; i < container.size; i++) {
            if (contaniner[i].x == x && container[i].y == y){
                return container[i];
            }
        }
        return NULL;
    }
};
*/
#endif




/*
Let's theorize about some other structures here:
What information do we/will we have?
pixel x, pixel y values
ray type
object type
the fact that ray u hit object v
eventually the unique object id

What do we want?
search per pixel
the *tree* of ray-object intersections - not a list
search per object hit (via unique id)
search per ray_type
search per object_type (less relevant)

implementation ideas:
multimaps with some values just being aggregates of keys?
how much resolution do we want? Do we eventually want to point at the top of
a tree or just at the ray-object intersection node

one idea:
map of trees, keyed by pixel. all other references are maps of keys to keys

*/


