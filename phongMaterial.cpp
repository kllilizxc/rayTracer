//
// Created by 54179 on 2016/10/25.
//

#include <GL/gl.h>
#include "phongMaterial.h"

// include glCanvas.h to access the preprocessor variable SPECULAR_FIX
#include "glCanvas.h"
#ifdef SPECULAR_FIX
// OPTIONAL:  global variable allows (hacky) communication
// with glCanvas::display
extern int SPECULAR_FIX_WHICH_PASS;
#endif

#define SPECULAR_COEFFICIENT 0.5

PhongMaterial::PhongMaterial(const Vec3f &diffuseColor,
                             const Vec3f &specularColor,
                             float exponent,
                             const Vec3f &reflectiveColor,
                             const Vec3f &transparentColor,
                             float indexOfRefraction) : Material(diffuseColor, reflectiveColor, transparentColor, indexOfRefraction),
                                                        specularColor(specularColor),
                                                        exponent(exponent) { }

Vec3f PhongMaterial::Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor) const {
    Vec3f h = dirToLight - ray.getDirection();
    h.Normalize();
    Vec3f normal = hit.getNormal();
    if (shade_back && normal.Dot3(dirToLight) < 0) normal *= -1;
    float diffuseFactor = dirToLight.Dot3(normal);
    if(diffuseFactor < 0) diffuseFactor = 0;
    float specularFactor = (float)pow(h.Dot3(normal), exponent);
    if(specularFactor < 0) specularFactor = 0;
    return diffuseFactor * diffuseColor * lightColor + specularFactor * specularColor * lightColor; //TODO not sure
}

void PhongMaterial::glSetMaterial(void) const {

    GLfloat one[4] = {1.0, 1.0, 1.0, 1.0};
    GLfloat zero[4] = {0.0, 0.0, 0.0, 0.0};
    GLfloat specular[4] = {
            getSpecularColor().r(),
            getSpecularColor().g(),
            getSpecularColor().b(),
            1.0};
    GLfloat diffuse[4] = {
            getDiffuseColor().r(),
            getDiffuseColor().g(),
            getDiffuseColor().b(),
            1.0};

    // NOTE: GL uses the Blinn Torrance version of Phong...
    float glexponent = exponent;
    if (glexponent < 0) glexponent = 0;
    if (glexponent > 128) glexponent = 128;

#if !SPECULAR_FIX

    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &glexponent);

#else

    // OPTIONAL: 3 pass rendering to fix the specular highlight
  // artifact for small specular exponents (wide specular lobe)

  if (SPECULAR_FIX_WHICH_PASS == 0) {
    // First pass, draw only the specular highlights
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, zero);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, zero);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &glexponent);

  } else if (SPECULAR_FIX_WHICH_PASS == 1) {
    // Second pass, compute normal dot light
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, one);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, zero);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);
  } else {
    // Third pass, add ambient & diffuse terms
    assert (SPECULAR_FIX_WHICH_PASS == 2);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);
  }

#endif
}

Vec3f PhongMaterial::getSpecularColor() const {
    return specularColor;
}







