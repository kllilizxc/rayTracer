//
// Created by 54179 on 2016/11/1.
//

#include "rayTracer.h"
#include "Group.h"
#include "light.h"
#include "rayTree.h"
#include "RayTracingStats.h"

#define EPSILON 0.001

RayTracer::RayTracer(SceneParser *s, int max_bounces, float cutoff_weight, bool shadows, Grid *grid, bool visualizeGrid)
        : sp(s),
          max_bounces(max_bounces),
          cutoff_weight(cutoff_weight),
          shadows(shadows),
          grid(grid),
          visualize_grid(visualizeGrid) {
    Group *group = sp->getGroup();
    if (visualizeGrid) {
        auto *transformMatrix = new Matrix();
        transformMatrix->SetToIdentity();
        grid->setBoundingBox(group->getBoundingBox());
        group->insertIntoGrid(grid, transformMatrix);
    }
}

Vec3f RayTracer::traceRay(Ray &ray, float tmin, int bounces, float weight, float indexOfRefraction, Hit &hit) const {
    //if too deep or too small
    if (bounces > max_bounces || weight < cutoff_weight)
        return Vec3f(0, 0, 0);

    //get hit point
    Group *group = sp->getGroup();
    if (visualize_grid) {
        if (!grid->intersect(ray, hit, tmin)) {
            return sp->getBackgroundColor();
        }
    } else {
        if (!group->intersect(ray, hit, tmin)) {
            return sp->getBackgroundColor();
        }
    }
    Material *material = hit.getMaterial();
    Vec3f hitPoint = hit.getIntersectionPoint();
    Vec3f color = material->getDiffuseColor() * sp->getAmbientLight();

    //main segment
    if (bounces == 0) {
        RayTree::SetMainSegment(ray, 0, hit.getT());
    }

    //shadows
    if (shadows)
        //for every light
        for (int i = 0; i < sp->getNumLights(); ++i) {
            Light *light = sp->getLight(i);
            Vec3f dir, col;
            float distanceToLight;
            //get direction and color
            light->getIllumination(hitPoint, dir, col, distanceToLight);

            //a ray point to light
            Ray shadowRay(hitPoint, dir);
            Hit shadowHit;
            shadowHit.set(INFINITY, shadowHit.getMaterial(), shadowHit.getNormal(), shadowRay);

            //if intersect, discard the contribution
            if (!group->shadowIntersect(shadowRay, shadowHit, EPSILON, distanceToLight)) {
                //add contribution
                color += material->Shade(ray, hit, dir, col);
            } else {
                //shadow segment
                RayTree::AddShadowSegment(shadowRay, 0, shadowHit.getT());
            }
        }


    //reflect
    Vec3f reflectiveColor = material->getReflectiveColor();
    if (reflectiveColor.Length() > 0) {
        Ray reflectRay(hitPoint, mirrorDirection(hit.getNormal(), ray.getDirection()));
        Hit reflectHit;
        Vec3f _color;
        reflectHit.set(INFINITY, reflectHit.getMaterial(), reflectHit.getNormal(), reflectRay);

        //add contribution
        _color = reflectiveColor * weight *
                 traceRay(reflectRay, EPSILON, ++bounces, weight * reflectiveColor.Length(), indexOfRefraction,
                          reflectHit);

        if (_color.Length() > 0) {
            color += _color;
            //reflected segment
            RayTree::AddReflectedSegment(reflectRay, 0, reflectHit.getT());
        }
    }

    //if grid mode, return
    if (visualize_grid) return color * 2; //TODO for debug

    //refraction
    Vec3f transparentColor = material->getTransparentColor();
    if (transparentColor.Length() > 0) {
        Vec3f refractionDirection;
        Hit refractionHit;
        Ray refractionRay;
        refractionHit.set(INFINITY, refractionHit.getMaterial(), refractionHit.getNormal(), refractionRay);

        //if the ray is inside the object or outside it
        float inIndex, outIndex;
        Vec3f _color;
        if (ray.getDirection().Dot3(hit.getNormal()) > 0) {
            //inside
            inIndex = material->getIndexOfRefraction();
            outIndex = 1; //TODO vacuum
        } else {
            //outside
            inIndex = indexOfRefraction;
            outIndex = material->getIndexOfRefraction();
        }
        //trace recursively
        if (transmittedDirection(hit.getNormal(), ray.getDirection(), inIndex, outIndex, refractionDirection)) {
            refractionRay = Ray(hitPoint, refractionDirection);

            //add contribution
            _color = transparentColor *
                     traceRay(refractionRay, EPSILON, ++bounces, weight * reflectiveColor.Length(), outIndex,
                              refractionHit);
            if (_color.Length() > 0) {
                color += _color;
                //transmitted segment
                RayTree::AddTransmittedSegment(refractionRay, 0, refractionHit.getT());
            }
        }

    }

    return color;
}

Vec3f RayTracer::mirrorDirection(const Vec3f &normal, const Vec3f &incoming) {
    Vec3f out = incoming - 2 * normal.Dot3(incoming) * normal;
    out.Normalize();
    return out;
}

bool RayTracer::transmittedDirection(const Vec3f &normal, const Vec3f &incoming, float index_i, float index_t,
                                     Vec3f &transmitted) {
    float index_r = index_i / index_t;
    Vec3f I = incoming * -1;

    double temp = pow(index_r, 2) * (1 - pow(normal.Dot3(I), 2));
    if (temp > 1) return false;
    transmitted = (index_r * normal.Dot3(I) - sqrt(1 - temp)) * normal - index_r * I;
    transmitted.Normalize();
    return true;
}







