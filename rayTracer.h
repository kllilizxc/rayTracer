//
// Created by 54179 on 2016/11/1.
//

#ifndef ASSIGNMENT_3_RAYTRACER_H
#define ASSIGNMENT_3_RAYTRACER_H


#include "scene_parser.h"
#include "hit.h"
#include "Grid.h"

class RayTracer {
public:
    RayTracer(SceneParser *s, int max_bounces, float cutoff_weight, bool shadows, Grid *grid, bool visualize_grid);

    Vec3f traceRay(Ray &ray, float tmin, int bounces, float weight,
                   float indexOfRefraction, Hit &hit) const;
private:
    Vec3f rayCast(Ray &ray, float tmin, int bounces, float weight,
                  float indexOfRefraction, Hit &hit) const;

    Vec3f rayCastFast(Ray &ray, float tmin, int bounces, float weight,
                  float indexOfRefraction, Hit &hit) const;

    Vec3f getLightColor(Light *light, Ray &ray, Hit &hit) const ;

    Ray getShadowRayFromLight(Light *light, Ray &ray, Hit &hit) const ;

    Vec3f getDirectionToLight(Light *light, Hit &hit) const;

    Vec3f getLightColor(Light *light, Hit &hit) const;

    Vec3f rayCastShadow(Ray &ray, float tmin, Hit &hit, Group *group) const;

    Vec3f rayCastShadowFast(Ray &ray, float tmin, Hit &hit) const;

    Vec3f rayCastReflective(Ray &ray, float tmin, int bounces, float weight,
                            float indexOfRefraction, Hit &hit) const;

    Vec3f rayCastTransparent(Ray &ray, float tmin, int bounces, float weight,
                            float indexOfRefraction, Hit &hit) const;

    bool withinDistanceToLight(Light *light, Hit &hit, Hit &shadowHit) const;

    static Vec3f mirrorDirection(const Vec3f &normal, const Vec3f &incoming);

    static bool transmittedDirection(const Vec3f &normal, const Vec3f &incoming,
                                     float index_i, float index_t, Vec3f &transmitted);

    int max_bounces;
    float cutoff_weight;
    bool shadows;
    SceneParser *sp;
    Grid *grid;
    bool visualize_grid;
};


#endif //ASSIGNMENT_3_RAYTRACER_H
