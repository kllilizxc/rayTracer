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

    void paint(void);

    void print();

    const Vec3f &getSize() const;

    void setSize(const Vec3f &size);

    bool isOpaque(int i, int j, int k);

    bool initializeRayMarch(MarchingInfo &mi, const Ray &r, float tmin) const;

private:

    Object3DVector ***opaque;

    Vec3f size;

    void drawBox(const Vec3f &min, const Vec3f &max);
};


#endif //ASSIGNMENT_5_GRID_H
