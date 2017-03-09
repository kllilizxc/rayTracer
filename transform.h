//
// Created by 54179 on 2016/10/12.
//

#ifndef ASSIGNMENT_1_TRANSFORM_H
#define ASSIGNMENT_1_TRANSFORM_H

#include <GL/gl.h>
#include "object3D.h"
#include "matrix.h"

class Transform : Object3D {
public:
    Transform(Matrix &m, Object3D *o) : m(m), o(o) {
        //TODO compute bounding box
    }

    bool intersect(const Ray &r, Hit &h, float tmin) {
        //get rayOS (Object System)
        Matrix inversedMatrix;
        m.Inverse(inversedMatrix, 0);
        Vec3f originOS = r.getOrigin();
        inversedMatrix.Transform(originOS);
        Vec3f directionOS = r.getDirection();
        inversedMatrix.TransformDirection(directionOS);
        directionOS.Normalize();
        Ray rOS = Ray(originOS, directionOS);
        //intersect
        if (!o->intersect(rOS, h, tmin)) return false;
        //scale d to get the true d
        float t = h.getT();
        Vec3f d = directionOS;
        d.Normalize();
        d *= t;
        m.TransformDirection(d);
        //transform the normal
        Matrix transposedMatrix;
        inversedMatrix.Transpose(transposedMatrix);
        Vec3f normal = h.getNormal();
        transposedMatrix.Transform(normal);
        normal.Normalize();
        h.set(d.Length(), h.getMaterial(), normal, r);
        return true;
    }

    virtual void paint(void) {
        glPushMatrix();
        GLfloat *glMatrix = m.glGet();
        glMultMatrixf(glMatrix);
        delete[] glMatrix;
        o->paint();
        glPopMatrix();
    }

private:
    Matrix m;
    Object3D *o;
};

#endif //ASSIGNMENT_1_TRANSFORM_H
