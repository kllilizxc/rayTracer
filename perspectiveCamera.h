//
// Created by 54179 on 2016/10/12.
//

#ifndef ASSIGNMENT_1_PERSPECTIVECAMERA_H
#define ASSIGNMENT_1_PERSPECTIVECAMERA_H

#include "camera.h"

class PerspectiveCamera : public Camera {
public:
    PerspectiveCamera(Vec3f center, Vec3f &direction, Vec3f &up, float angle);

    Ray generateRay(Vec2f point);

    float getTMin() const;

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
    float angle;
};


#endif //ASSIGNMENT_1_PERSPECTIVECAMERA_H
