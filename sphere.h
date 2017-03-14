//
// Created by 54179 on 2016/10/2.
//

#ifndef ASSIGNMENT_5_SPHERE_H
#define ASSIGNMENT_5_SPHERE_H


#include "object3D.h"

class Sphere : public Object3D {
public:
    Sphere(Vec3f center = Vec3f(0, 0, 0), float radius = 0, Material *material = NULL);

    virtual bool intersect(const Ray &r, Hit &h, float tmin);

    virtual void paint(void);

    static int thetaSteps;

    static int phiSteps;

    static bool gouraud;

    virtual void insertIntoGrid(Grid *g, Matrix *m);

private:
    void calculateBoundingBox(const Vec3f& center, float radius);

    Vec3f center;
    float radius;
};


#endif //ASSIGNMENT_1_SPHERE_H
