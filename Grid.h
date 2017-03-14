//
// Created by 54179 on 2016/12/3.
//

#ifndef ASSIGNMENT_5_GRID_H
#define ASSIGNMENT_5_GRID_H


#include "object3D.h"
#include "MarchingInfo.h"
#include "object3dvector.h"

class Grid : public Object3D {
public:
    Grid(BoundingBox *bb, int nx, int ny, int nz);

    ~Grid();

    void setOpaque(int x, int y, int z, Object3D *val);

    bool intersect(const Ray &r, Hit &h, float tmin);

    Vec3f at(int i, int j, int k);

    void paint(void);

    void print();

    const Vec3f &getSize() const;

    void setSize(const Vec3f &size);

    bool isOpaque(int i, int j, int k);

    bool initializeRayMarch(MarchingInfo &mi, const Ray &r, float tmin) const;

private:

    void setCurrentColor(int i, int j, int k);

    Vec3f& getCurrentColor();

    void drawQuadsAtXAxis(const Vec3f &min);

    void drawQuadsAtYAxis(const Vec3f &min);

    void drawQuadsAtZAxis(const Vec3f &min);

    void drawQuads(const Vec3f &a, const Vec3f &b, const Vec3f &c, const Vec3f &d);

    void drawTriangles(const Vec3f &a, const Vec3f &b, const Vec3f &c);

    void drawBox(const Vec3f &min, const Vec3f &max);

    static Vec3f colors[];

    Object3DVector ***opaque;

    Vec3f size;

    Vec3f gridSize;

    Vec3f currentColor;
};


#endif //ASSIGNMENT_5_GRID_H
