//
// Created by 54179 on 2016/10/2.
//

#ifndef ASSIGNMENT_1_CAMERA_H
#define ASSIGNMENT_1_CAMERA_H


#include "ray.h"

class Camera {
public:
    virtual Ray generateRay(Vec2f point) = 0;

    virtual float getTMin() const = 0;

    // ====================================================================
    // Here are the prototypes for five pure virtual functions for
    // initializing, placing, dollying, trucking, and rotating an
    // OpenGL camera. These should be added to the Camera class and
    // each of its subclasses
    // ==================================================================== #include #include
    virtual void glInit(int w, int h) = 0;

    virtual void glPlaceCamera(void) = 0;

    virtual void dollyCamera(float dist) = 0;

    virtual void truckCamera(float dx, float dy) = 0;

    virtual void rotateCamera(float rx, float ry) = 0;
};


#endif //ASSIGNMENT_1_CAMERA_H
