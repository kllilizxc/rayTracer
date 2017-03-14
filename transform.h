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
        BoundingBox *bb = o->getBoundingBox();
        Vec3f max = bb->getMax();
        Vec3f min = bb->getMin();
        Vec3f size = max - min;

        //get untransformed vecs
        Vec3f vecs[] = {
                min,
                min + Vec3f(1, 0, 0) * size,
                min + Vec3f(0, 1, 0) * size,
                min + Vec3f(0, 0, 1) * size,
                max - Vec3f(1, 0, 0) * size,
                max - Vec3f(0, 1, 0) * size,
                max - Vec3f(0, 0, 1) * size,
                max
        };

        //get max and min of transformed vecs
        Vec3f _max, _min;
        Vec3f::Max(_max, vecs[0], vecs[1]);
        Vec3f::Min(_min, vecs[0], vecs[1]);
        for (int i = 0; i < 6; ++i) {
            Vec3f::Max(_max, _max, vecs[i + 2]);
            Vec3f::Min(_min, _min, vecs[i + 2]);
        }

        setBoundingBox(_min, _max);
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

    virtual void insertIntoGrid(Grid *g, Matrix *m) {
        *m *= this->m;
        o->insertIntoGrid(g, m);
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
