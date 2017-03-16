#ifndef _HIT_H
#define _HIT_H

#include "vectors.h"
#include "ray.h"

class Material;

// ====================================================================
// ====================================================================

class Hit {

public:

    // CONSTRUCTOR & DESTRUCTOR
    Hit() { material = NULL; }

    Hit(float _t, Material *m, Vec3f n) {
        t = _t;
        material = m;
        normal = n;
    }

    Hit(const Hit &h) {
        t = h.t;
        material = h.material;
        normal = h.normal;
        intersectionPoint = h.intersectionPoint;
    }

    Hit(const Ray ray, float t) {
        setT(t, ray);
    }

    ~Hit() {}

    // ACCESSORS
    float getT() const { return t; }

    Material *getMaterial() const { return material; }

    void setMaterial(Material *material) {
        this->material = material;
    }

    Vec3f getNormal() const { return normal; }

    Vec3f getIntersectionPoint() const { return intersectionPoint; }

    // MODIFIER
    void set(float _t, Material *m, Vec3f n, const Ray &ray) {
        t = _t;
        material = m;
        normal = n;
        intersectionPoint = ray.pointAtParameter(t);
    }

    void setT(float _t, const Ray &ray) {
        t = _t;
        intersectionPoint = ray.pointAtParameter(t);
    }

    void setNormal(Vec3f normal) {
        normal.Normalize();
        this->normal = normal;
    }

private:

    // REPRESENTATION
    float t;
    Material *material;
    Vec3f normal;
    Vec3f intersectionPoint;

};

inline ostream &operator<<(ostream &os, const Hit &h) {
    os << "Hit <" << h.getT() << ", " << h.getNormal() << ">";
    return os;
}
// ====================================================================
// ====================================================================

#endif
