//
// Created by 54179 on 2016/10/12.
//

#include <GL/gl.h>
#include "plane.h"
#include "RayTracingStats.h"

Plane::Plane(Vec3f &normal, float d, Material *m) : Object3D(m), normal(normal), d(d) {
}

bool Plane::intersect(const Ray &r, Hit &h, float tmin) {

    //update RayTracingStats
    RayTracingStats::IncrementNumIntersecstions();

    if(!r.getDirection().Dot3(normal)) return false;
    float t = -(-d + r.getOrigin().Dot3(normal)) / (r.getDirection().Dot3(normal));
    if(t < tmin) return false;
    h.set(t, getMaterial(), normal, r);
    return true;
}

void Plane::paint(void) {
    Vec3f v;
    if(normal.z() || normal.y()) v = Vec3f(1, 0, 0);
    else v = Vec3f(0, 1, 0);
    Vec3f b1, b2;
    Vec3f::Cross3(b1, normal, v);
    Vec3f::Cross3(b2, normal, b1);
    int BIG = 10000;
    b1 *= BIG;
    b2 *= BIG;

    Vec3f points[4] = {
            Vec3f(b1.x() + b2.x(), b1.y() + b2.y(), b1.z() + b2.z()) + d * normal,
            Vec3f(b1.x() - b2.x(), b1.y() - b2.y(), b1.z() - b2.z()) + d * normal,
            Vec3f(-b1.x() - b2.x(), -b1.y() - b2.y(), -b1.z() - b2.z()) + d * normal,
            Vec3f(b2.x() - b1.x(), b2.y() - b1.y(), b2.z() - b1.z()) + d * normal
    };
    getMaterial()->glSetMaterial();
    glBegin(GL_QUADS);
    glNormal3f(normal.x(), normal.y(), normal.z());
    for (int i = 0; i < 4; ++i) {
        glVertex3f(points[i].x(), points[i].y(), points[i].z());
    }
    glEnd();
}

void Plane::insertIntoGrid(Grid *g, Matrix *m) {
}
