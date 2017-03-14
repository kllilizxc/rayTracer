#ifndef _BOUNDING_BOX_H_
#define _BOUNDING_BOX_H_

#include "vectors.h"
#include "hit.h"

#include <assert.h>

// ====================================================================
// ====================================================================

class BoundingBox {

public:

    // CONSTRUCTOR & DESTRUCTOR
    BoundingBox(Vec3f _min, Vec3f _max) {
        Set(_min,_max); }
    ~BoundingBox() {}

    // ACCESSORS
    void Get(Vec3f &_min, Vec3f &_max) const {
        _min = min;
        _max = max; }
    Vec3f getMin() const { return min; }
    Vec3f getMax() const { return max; }

    // MODIFIERS
    void Set(BoundingBox *bb) {
        assert(bb != NULL);
        min = bb->min;
        max = bb->max; }
    void Set(Vec3f _min, Vec3f _max) {
        assert (min.x() <= max.x() &&
                min.y() <= max.y() &&
                min.z() <= max.z());
        min = _min;
        max = _max; }
    void Extend(const Vec3f v) {
        min = Vec3f(min2(min.x(),v.x()),
                    min2(min.y(),v.y()),
                    min2(min.z(),v.z()));
        max = Vec3f(max2(max.x(),v.x()),
                    max2(max.y(),v.y()),
                    max2(max.z(),v.z())); }
    void Extend(BoundingBox *bb) {
        assert (bb != NULL);
        Extend(bb->min);
        Extend(bb->max); }

    bool intersect(const Ray &r, Hit &h_min, Hit &h_max, float tmin) {

        Vec3f size = max - min;
        Vec3f boundsMin = min;
        Vec3f boundsMax = min;
        Vec3f rayDir = r.getDirection();

        if(rayDir.x() >= 0)
            boundsMax.SetX(boundsMax.x() + size.x());
        else
            boundsMin.SetX(boundsMin.x() + size.x());

        if(rayDir.y() >= 0)
            boundsMax.SetY(boundsMax.y() + size.y());
        else
            boundsMin.SetY(boundsMin.y() + size.y());

        if(rayDir.z() >= 0)
            boundsMax.SetZ(boundsMax.z() + size.z());
        else
            boundsMin.SetZ(boundsMin.z() + size.z());

        Vec3f t_min_v = (boundsMin - r.getOrigin()) / rayDir;
        Vec3f t_max_v = (boundsMax - r.getOrigin()) / rayDir;

        float t_min = max3(t_min_v.x(), t_min_v.y(), t_min_v.z());
        float t_max = min3(t_max_v.x(), t_max_v.y(), t_max_v.z());

        if(t_min == t_min_v.x()) h_min.setNormal(Vec3f(-rayDir.x(), 0, 0));
        else if(t_min == t_min_v.y()) h_min.setNormal(Vec3f(0, -rayDir.y(), 0));
        else if(t_min == t_min_v.z()) h_min.setNormal(Vec3f(0, 0, -rayDir.z()));

        if(t_max == t_max_v.x()) h_max.setNormal(Vec3f(-rayDir.x(), 0, 0));
        else if(t_max == t_max_v.y()) h_max.setNormal(Vec3f(0, -rayDir.y(), 0));
        else if(t_max == t_max_v.z()) h_max.setNormal(Vec3f(0, 0, -rayDir.z()));

        float eps = 0.01;

        if(t_max - t_min > eps && t_max > eps) {
            h_min.setT(t_min, r);
            h_max.setT(t_max, r);
            return true;
        } else
            return false;

    }

    // DEBUGGING
    void Print() const {
        printf ("%f %f %f  -> %f %f %f\n", min.x(),min.y(),min.z(),
                max.x(),max.y(),max.z()); }
    void paint() const;

private:

    BoundingBox() { assert(0); } // don't use this constructor

    // REPRESENTATION
    Vec3f min;
    Vec3f max;

};

// ====================================================================
// ====================================================================

#endif
