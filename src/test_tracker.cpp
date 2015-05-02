#include <gtest/gtest.h>
#include <src/tracker.hpp>
#include <src/camera.h>
#include <src/ray.h>
#include <src/pixel.h>
#include <src/objects.h>
#include <iostream>

namespace {
class BaseRayObjectNodeTest: public ::testing::Test
{
protected:
    float TOLERANCE = 0.003;
    Ray *ray1;
    Sphere *sphere1;

    BaseRayObjectNodeTest() {
        vec3 origin = vec3(0.0);
        vec3 direction = glm::normalize(vec3(0.0, 0.0, -1.0));
        ray1 = new Ray(origin, direction, RayType::camera, 1);

        mat4 trsp = Transform::translate(0.0, 0.0, -3.0);
        sphere1 = new Sphere(1.0, &trsp, vec4(0.0, 0.0, 0.0, 1.0), 1.0, 1);
    }

    virtual ~BaseRayObjectNodeTest() {
        if (ray1 != NULL) {
            delete ray1;
        }

        if (sphere1 != NULL) {
            delete sphere1;
        }
    }

    virtual void SetUp() {}

    virtual void Teardown() {}
};

TEST_F(BaseRayObjectNodeTest, RayObjectNodeInstantiatesCorrectly) {
    RayObjectNode ron = RayObjectNode(*ray1, *sphere1, NULL);
    EXPECT_EQ(ron.key, "1_1");
    EXPECT_EQ(ron.ray.direction, ray1->direction);
    EXPECT_EQ(ron.ray.origin, ray1->origin);
    //EXPECT_EQ(ron.object, *sphere1);
}

TEST_F(BaseRayObjectNodeTest, OjectNodeEncodesDecodesCorrectly) {
    RayObjectNode ron = RayObjectNode(*ray1, *sphere1, NULL);
    int irid = 2;
    int ioid = 4;
    std::string key1 = ron.encode_key(irid, ioid);
    EXPECT_EQ(key1, "2_4");

    int rid, oid;
    ron.decode_key(key1, rid, oid);
    EXPECT_EQ(rid, 2);
    EXPECT_EQ(oid, 4);
}

} //namespace
