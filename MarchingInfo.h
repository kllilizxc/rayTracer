//
// Created by 54179 on 2016/12/4.
//

#ifndef ASSIGNMENT_5_MARCHINGINFO_H
#define ASSIGNMENT_5_MARCHINGINFO_H


#include "vectors.h"

class MarchingInfo {
public:
    const Vec3f &getSign() const;

    void setSign(const Vec3f &sign);

    float getTmin() const;

    void setTmin(float tmin);

    const Vec3i &getIndex() const;

    void setIndex(const Vec3i &index);

    const Vec3f &getT_next() const;

    void setT_next(const Vec3f &t_next);

    void setT_next(const float x, const float y, const float z);

    const Vec3f &getD_t() const;

    void setD_t(const Vec3f &d_t);

    void nextCell();

    void setGridSize(const Vec3f &gridSize);

    void setNormal(const Vec3f &normal);

    const Vec3f &getNormal() const;

private:
    Vec3f sign; //sign of direction
    float tmin;
    Vec3i index; //current point
    Vec3f t_next; // the next values of intersection along each axis
    Vec3f d_t; // the marching increments along each axis
    Vec3f normal;
    Vec3f gridSize;
};


#endif //ASSIGNMENT_5_MARCHINGINFO_H
