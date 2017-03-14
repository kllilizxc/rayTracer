//
// Created by 54179 on 2016/10/12.
//

#ifndef ASSIGNMENT_1_PLANE_H
#define ASSIGNMENT_1_PLANE_H


#include "object3D.h"

class Plane : Object3D {
public:
    Plane(Vec3f &normal, float d, Material *m);

    virtual bool intersect(const Ray &r, Hit &h, float tmin);

    virtual void paint(void);

    virtual void insertIntoGrid(Grid *g, Matrix *m);

private:
    Vec3f normal;
    float d;
};


#endif //ASSIGNMENT_1_PLANE_H
