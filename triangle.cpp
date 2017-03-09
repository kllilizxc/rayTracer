//
// Created by 54179 on 2016/10/12.
//

#include <GL/gl.h>
#include "triangle.h"
#include "matrix.h"
#include "Grid.h"

Triangle::Triangle(Vec3f &a, Vec3f &b, Vec3f &c, Material *m) : Object3D(m), a(a), b(b), c(c) {
    Vec3f max, min;
    Vec3f::Max(max, a, b);
    Vec3f::Max(max, max, c);
    Vec3f::Min(min, a, b);
    Vec3f::Min(min, max, c);

    setBoundingBox(min, max);
}

bool Triangle::intersect(const Ray &r, Hit &h, float tmin) {
    float col1[3] = { a.x() - b.x(), a.y() - b.y(), a.z() - b.z() };
    float col2[3] = { a.x() - c.x(), a.y() - c.y(), a.z() - c.z() };
    float col3[3] = { r.getDirection().x(), r.getDirection().y(), r.getDirection().z() };
    float col4[3] = { a.x() - r.getOrigin().x(), a.y() - r.getOrigin().y(), a.z() - r.getOrigin().z() };

    float A = Matrix::det3x3(col1[0], col2[0], col3[0],
                             col1[1], col2[1], col3[1],
                             col1[2], col2[2], col3[2]);

    float belta = Matrix::det3x3(col4[0], col2[0], col3[0],
                             col4[1], col2[1], col3[1],
                             col4[2], col2[2], col3[2]) / A;

    float gama = Matrix::det3x3(col1[0], col4[0], col3[0],
                             col1[1], col4[1], col3[1],
                             col1[2], col4[2], col3[2]) / A;

    if(belta >= 0 && belta <= 1 && gama >= 0 && gama <= 1 && (belta + gama) >= 0 && (belta + gama) <= 1) {
        float t = Matrix::det3x3(col1[0], col2[0], col4[0],
                                 col1[1], col2[1], col4[1],
                                 col1[2], col2[2], col4[2]) / A;
        if(t < tmin) return false;
        Vec3f normal;
        Vec3f::Cross3(normal, a - c, b - c);
        normal.Normalize();
        //normal = Vec3f(fabsf(normal.x()), fabsf(normal.y()), fabsf(normal.z()));
        h.set(t, getMaterial(), normal, r);
        return true;
    } else return false;
}

void Triangle::paint(void) {
    Vec3f normal;
    Vec3f::Cross3(normal, a - c, b - c);
    getMaterial()->glSetMaterial();
    glBegin(GL_TRIANGLES);
    glNormal3f(normal.x(), normal.y(), normal.z());
    glVertex3f(a.x(), a.y(), a.z());
    glVertex3f(b.x(), b.y(), b.z());
    glVertex3f(c.x(), c.y(), c.z());
    glEnd();
}

void Triangle::insertIntoGrid(Grid *g, Matrix *m) {
    BoundingBox *bb = g->getBoundingBox();
    Vec3f min = bb->getMin();
    Vec3f max = bb->getMax();

    Vec3f gridSize = (max - min) / g->getSize();

    for (int i = 0; i < g->getSize().x(); ++i) {
        for (int j = 0; j < g->getSize().y(); ++j) {
            for (int k = 0; k < g->getSize().z(); ++k) {
                Vec3f gridMin = min + Vec3f(i, j, k) * gridSize;
                Vec3f gridMax = min + Vec3f(i + 1, j + 1, k + 1) * gridSize;
                Vec3f bbMin = getBoundingBox()->getMin();
                Vec3f bbMax = getBoundingBox()->getMax();

                if((gridMin <= bbMin && gridMax >= bbMin) ||
                        (gridMin <= bbMax && gridMax >= bbMax) ||
                        (gridMin >= bbMin && gridMax <= bbMax))
                    g->setOpaque(i, j, k, this);
            }
        }
    }
}
