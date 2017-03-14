//
// Created by 54179 on 2016/10/12.
//

#ifndef ASSIGNMENT_1_TRIANGLE_H
#define ASSIGNMENT_1_TRIANGLE_H


#include "object3D.h"

class Triangle : public Object3D {
public:
    Triangle(Vec3f &a, Vec3f &b, Vec3f &c, Material *m);

    virtual bool intersect(const Ray &r, Hit &h, float tmin);

    virtual void paint(void);

    virtual void insertIntoGrid(Grid *g, Matrix *m);

private:

    void calculateBoundingBox(const Vec3f &a, const Vec3f &b, const Vec3f &c);

    Vec3f a;
    Vec3f b;
    Vec3f c;
};


#endif //ASSIGNMENT_1_TRIANGLE_H
