//
// Created by 54179 on 2016/12/4.
//

#include "MarchingInfo.h"
#include "RayTracingStats.h"

float MarchingInfo::getTmin() const {
    return tmin;
}

void MarchingInfo::setTmin(float tmin) {
    this->tmin = tmin;
}

void MarchingInfo::nextCell() {

    //update RayTracingStats
    RayTracingStats::IncrementNumGridCellsTraversed();

    if(t_next.x() < t_next.y()) {
        if(t_next.x() < t_next.z()) {
            setTmin(t_next.x());
            t_next.SetX(t_next.x() + d_t.x());
            index.SetX(index.x() + sign.x());
            normal.Set(-sign.x(), 0, 0);
        } else {
            setTmin(t_next.z());
            t_next.SetZ(t_next.z() + d_t.z());
            index.SetZ(index.z() + sign.z());
            normal.Set(0, 0, -sign.z());
        }
    } else  {
        if (t_next.y() < t_next.z()) {
            setTmin(t_next.y());
            t_next.SetY(t_next.y() + d_t.y());
            index.SetY(index.y() + sign.y());
            normal.Set(0, -sign.y(), 0);
        } else {
            setTmin(t_next.z());
            t_next.SetZ(t_next.z() + d_t.z());
            index.SetZ(index.z() + sign.z());
            normal.Set(0, 0, -sign.z());
        }
    }
}

const Vec3f &MarchingInfo::getSign() const {
    return sign;
}

void MarchingInfo::setSign(const Vec3f &sign) {
    this->sign = sign;
}

const Vec3i &MarchingInfo::getIndex() const {
    return index;
}

void MarchingInfo::setIndex(const Vec3i &index) {
    this->index = index;
}

const Vec3f &MarchingInfo::getT_next() const {
    return t_next;
}

void MarchingInfo::setT_next(const Vec3f &t_next) {
    this->t_next = t_next;
}

const Vec3f &MarchingInfo::getD_t() const {
    return d_t;
}

void MarchingInfo::setD_t(const Vec3f &d_t) {
    this->d_t = d_t;
}

const Vec3f &MarchingInfo::getNormal() const {
    return normal;
}

void MarchingInfo::setT_next(const float x, const float y, const float z) {
    t_next.Set(x, y, z);
}

void MarchingInfo::setNormal(const Vec3f &normal) {
    this->normal = normal;
}

void MarchingInfo::setGridSize(const Vec3f &gridSize) {
    this->gridSize = gridSize;
}
