#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "vectors.h"
#include "hit.h"

// ====================================================================
// ====================================================================

// You will extend this class in later assignments

class Material {

public:

    // CONSTRUCTORS & DESTRUCTOR
    Material(const Vec3f &d_color,
             const Vec3f &reflectiveColor,
             const Vec3f &transparentColor,
             float indexOfRefraction) : diffuseColor(d_color),
                                        reflectiveColor(reflectiveColor),
                                        transparentColor(transparentColor),
                                        indexOfRefraction(indexOfRefraction) { };

    virtual ~Material() { }

    // ACCESSORS
    virtual Vec3f getDiffuseColor() const { return diffuseColor; }

    virtual Vec3f getReflectiveColor() const { return reflectiveColor; }

    virtual Vec3f getTransparentColor() const { return transparentColor; }

    virtual float getIndexOfRefraction() const { return indexOfRefraction; }

    virtual void glSetMaterial(void) const = 0;

    virtual Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor) const = 0;

    static bool shade_back;

protected:

    // REPRESENTATION
    Vec3f diffuseColor;
    Vec3f reflectiveColor;
    Vec3f transparentColor;
    float indexOfRefraction;
};

// ====================================================================
// ====================================================================

#endif
