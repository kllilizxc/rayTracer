//
// Created by 54179 on 2016/10/2.
//

#ifndef ASSIGNMENT_1_ORTHOGRAPHICCAMERA_H
#define ASSIGNMENT_1_ORTHOGRAPHICCAMERA_H


#include <GL/gl.h>
#include "camera.h"

class OrthographicCamera : public Camera {
public:
    OrthographicCamera(Vec3f center, Vec3f direction, Vec3f up, float imageSize);

    virtual Ray generateRay(Vec2f point);

    virtual float getTMin() const;

    void glInit(int w, int h);

    void glPlaceCamera(void);

    void dollyCamera(float dist);

    void truckCamera(float dx, float dy);

    void rotateCamera(float rx, float ry);

private:
    Vec3f center;
    Vec3f direction;
    Vec3f up;
    Vec3f horizontal;
    float imageSize;
};

#endif //ASSIGNMENT_1_ORTHOGRAPHICCAMERA_H
