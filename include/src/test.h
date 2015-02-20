#ifndef RTTEST_H
#define RTTEST_H

class RTTest() {
public:
    RTTest();
    ~RTTest();

    void test_ray_pixel_conversion();
    void test_sphere_intersection();
    void test_sphere_reflection();
    void test_color_combination();
};

#endif
