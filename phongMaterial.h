//
// Created by 54179 on 2016/10/25.
//

#ifndef ASSIGNMENT_3_PHONGMATERIAL_H
#define ASSIGNMENT_3_PHONGMATERIAL_H

#include "material.h"

class PhongMaterial : public Material {
public:
    PhongMaterial(const Vec3f &diffuseColor,
                  const Vec3f &specularColor,
                  float exponent,
                  const Vec3f &reflectiveColor,
                  const Vec3f &transparentColor,
                  float indexOfRefraction);

    void glSetMaterial(void) const;

    Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor) const;

    Vec3f getSpecularColor() const;

private:
    Vec3f specularColor;
    float exponent;
};


#endif //ASSIGNMENT_3_PHONGMATERIAL_H
