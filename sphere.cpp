//
// Created by 54179 on 2016/10/2.
//

#include <GL/gl.h>
#include "sphere.h"
#include "Grid.h"
#define PI 3.1415926

int Sphere::thetaSteps = 1;

int Sphere::phiSteps = 1;

bool Sphere::gouraud = false;

Sphere::Sphere(Vec3f center, float radius, Material *material) : Object3D(material), center(center), radius(radius) {
    Vec3f max = center + Vec3f(radius, radius, radius);
    Vec3f min = center - Vec3f(radius, radius, radius);
    setBoundingBox(min, max);
}

bool Sphere::intersect(const Ray &r, Hit &h, float tmin) {

    Vec3f Ro = r.getOrigin() - center;
    double r2 = radius * radius;
    double Ro2 = Ro.Dot3(Ro);
    bool onSurface = fabs(Ro2 - r2) < tmin;
    bool outside = Ro2 - r2 > tmin;
    double t;

    Vec3f Rd = r.getDirection();
    double tp = -1 * Ro.Dot3(Rd);
    if (tp < tmin || onSurface) return false;

    double d2 = Ro2 - tp * tp;
    if (outside && d2 > r2) return false;

    double _t = sqrt(r2 - d2);
    t = outside ? (tp - _t) : (tp + _t);

    Vec3f hitPoint = r.pointAtParameter(t);
    Vec3f normal = hitPoint - center;
    if(normal.Dot3(r.getDirection()) > 0) normal *= -1;
    normal.Normalize();
    h.set(t, getMaterial(), normal, r);
    return true;
}

void Sphere::paint(void) {
    getMaterial()->glSetMaterial();
    glBegin(GL_QUADS);
    for (int iPhi = -90; iPhi <= 90; iPhi += 180 / phiSteps)
        for (int iTheta = 0; iTheta < 360; iTheta += 360 / thetaSteps) {
            // compute appropriate coordinates & normals
            int iPhi_next = iPhi + 180 / phiSteps;
            int iTheta_next = iTheta + 360 / thetaSteps;
            float iPhi_radian = iPhi * PI / 180.0;
            float iPhi_next_radian = iPhi_next * PI / 180.0;
            float iTheta_radian = iTheta * PI / 180.0;
            float iTheta_next_radian = iTheta_next * PI / 180.0;
            float x1 = center.x() + radius * sin(iTheta_radian) * sin(iPhi_radian),
                    x2 = center.x() + radius * sin(iTheta_radian) * sin(iPhi_next_radian),
                    x3 = center.x() + radius * sin(iTheta_next_radian) * sin(iPhi_radian),
                    x4 = center.x() + radius * sin(iTheta_next_radian) * sin(iPhi_next_radian);
            float y1 = center.y() + radius * sin(iTheta_radian) * cos(iPhi_radian),
                    y2 = center.y() + radius * sin(iTheta_radian) * cos(iPhi_next_radian),
                    y3 = center.y() + radius * sin(iTheta_next_radian) * cos(iPhi_radian),
                    y4 = center.y() + radius * sin(iTheta_next_radian) * cos(iPhi_next_radian);
            float z1 = center.z() + radius * cos(iTheta_radian),
                    z2 = center.z() + radius * cos(iTheta_next_radian);

            // send gl vertex commands
            Vec3f normal;
            if (!gouraud) {
                if (iTheta < 180)
                    Vec3f::Cross3(normal, Vec3f(x1 - x4, y1 - y4, z1 - z2), Vec3f(x2 - x3, y2 - y3, z1 - z2));
                else Vec3f::Cross3(normal, Vec3f(x2 - x3, y2 - y3, z1 - z2), Vec3f(x1 - x4, y1 - y4, z1 - z2));
                glNormal3f(normal.x(), normal.y(), normal.z());
            }
            Vec3f points[4] = {
                    Vec3f(x1, y1, z1),
                    Vec3f(x2, y2, z1),
                    Vec3f(x4, y4, z2),
                    Vec3f(x3, y3, z2)
            };

            for (int i = 0; i < 4; ++i) {
                if (gouraud) glNormal3f(points[i].x(), points[i].y(), points[i].z());
                glVertex3f(points[i].x(), points[i].y(), points[i].z());
            }
        }
    glEnd();

}

void Sphere::insertIntoGrid(Grid *g, Matrix *m) {
    BoundingBox *bb = g->getBoundingBox();
    Vec3f min = bb->getMin();
    Vec3f max = bb->getMax();

    Vec3f gridSize = (max - min) / g->getSize();

    for (int i = 0; i < g->getSize().x(); ++i) {
        for (int j = 0; j < g->getSize().y(); ++j) {
            for (int k = 0; k < g->getSize().z(); ++k) {
                Vec3f gridCenter = min + (Vec3f(i, j, k) + Vec3f(0.5, 0.5, 0.5)) * gridSize;

                if((gridCenter - center).Length() < radius)
                    g->setOpaque(i, j, k, this);
            }
        }
    }
}


