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
