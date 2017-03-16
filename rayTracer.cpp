//
// Created by 54179 on 2016/11/1.
//

#include "rayTracer.h"
#include "rayTree.h"


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

    if (visualize_grid) {
        if (!grid->traverse(ray, hit, tmin)) {
            return sp->getBackgroundColor();
        } else {
           return rayCastShadowFast(ray, tmin, hit);
        }
    }

    if (grid->getSize() > Vec3f(0, 0, 0)) {
        return rayCastFast(ray, tmin, bounces, weight, indexOfRefraction, hit);
    } else {
        return rayCast(ray, tmin, bounces, weight, indexOfRefraction, hit);
    }
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

Vec3f RayTracer::rayCast(Ray &ray, float tmin, int bounces, float weight, float indexOfRefraction, Hit &hit) const {
    //if too deep or too small
    if (bounces > max_bounces || weight < cutoff_weight)
        return Vec3f(0, 0, 0);

    //get hit point
    Group *group = sp->getGroup();

    if (!group->intersect(ray, hit, tmin)) {
        return sp->getBackgroundColor();
    }

    Vec3f color = hit.getMaterial()->getDiffuseColor() * sp->getAmbientLight();

    //main segment
    if (bounces == 0) {
        RayTree::SetMainSegment(ray, 0, hit.getT());
    }

    //shadows
    if (shadows)
        color += rayCastShadow(ray, tmin, hit, group);

    //reflect
    color += rayCastReflective(ray, tmin, bounces, weight, indexOfRefraction, hit);

    //refraction
    color += rayCastTransparent(ray, tmin, bounces, weight, indexOfRefraction, hit);

    return color;
}

Vec3f RayTracer::rayCastFast(Ray &ray, float tmin, int bounces, float weight, float indexOfRefraction, Hit &hit) const {
    //if too deep or too small
    if (bounces > max_bounces || weight < cutoff_weight)
        return Vec3f(0, 0, 0);

    //get hit point
    if (!grid->intersect(ray, hit, tmin)) {
        return sp->getBackgroundColor();
    }

    Vec3f color = hit.getMaterial()->getDiffuseColor() * sp->getAmbientLight();

    //main segment
    if (bounces == 0) {
        RayTree::SetMainSegment(ray, 0, hit.getT());
    }

    //shadows
    if (shadows)
        color += rayCastShadowFast(ray, tmin, hit);

    //reflect
    color += rayCastReflective(ray, tmin, bounces, weight, indexOfRefraction, hit);

    //refraction
    color += rayCastTransparent(ray, tmin, bounces, weight, indexOfRefraction, hit);

    return color;
}

Vec3f RayTracer::getLightColor(Light *light, Ray &ray, Hit &hit) const {
    Vec3f hitPoint = hit.getIntersectionPoint();
    Material *material = hit.getMaterial();

    return material->Shade(ray, hit, getDirectionToLight(light, hit), getLightColor(light, hit));
}

Ray RayTracer::getShadowRayFromLight(Light *light, Ray &ray, Hit &hit) const {
    //a ray point to light
    return Ray(hit.getIntersectionPoint(), getDirectionToLight(light, hit));
}

Vec3f RayTracer::getDirectionToLight(Light *light, Hit &hit) const {
    Vec3f dir, col;
    float distanceToLight;
    Vec3f hitPoint = hit.getIntersectionPoint();

    //get direction and color
    light->getIllumination(hitPoint, dir, col, distanceToLight);
    return dir;
}

Vec3f RayTracer::getLightColor(Light *light, Hit &hit) const {
    Vec3f dir, col;
    float distanceToLight;
    Vec3f hitPoint = hit.getIntersectionPoint();

    //get direction and color
    light->getIllumination(hitPoint, dir, col, distanceToLight);
    return col;
}

bool RayTracer::withinDistanceToLight(Light *light, Hit &hit, Hit &shadowHit) const {
    Vec3f dir, col;
    float distanceToLight;
    Vec3f hitPoint = hit.getIntersectionPoint();

    //get direction and color
    light->getIllumination(hitPoint, dir, col, distanceToLight);

    return distanceToLight <= shadowHit.getT();
}

Vec3f RayTracer::rayCastShadow(Ray &ray, float tmin, Hit &hit, Group *group) const {
    //for every light
    Vec3f color;
    for (int i = 0; i < sp->getNumLights(); ++i) {
        //get light
        Light *light = sp->getLight(i);

        //a ray point to light
        Ray shadowRay = getShadowRayFromLight(light, ray, hit);
        Hit shadowHit(shadowRay, INFINITY);

        //if traverse, discard the contribution
        if (!group->shadowIntersect(shadowRay, shadowHit, tmin) && withinDistanceToLight(light, hit, shadowHit)) {
            //add contribution
            color += hit.getMaterial()->Shade(ray, hit, getDirectionToLight(light, hit), getLightColor(light, hit));
        } else {
            //shadow segment
            RayTree::AddShadowSegment(shadowRay, 0, shadowHit.getT());
        }
    }

    return color;
}

Vec3f RayTracer::rayCastShadowFast(Ray &ray, float tmin, Hit &hit) const {
    //for every light
    Vec3f color;
    for (int i = 0; i < sp->getNumLights(); ++i) {
        //get light
        Light *light = sp->getLight(i);

        //a ray point to light
        Ray shadowRay = getShadowRayFromLight(light, ray, hit);
        Hit shadowHit(shadowRay, INFINITY);

        //if traverse, discard the contribution
        if (!grid->intersect(shadowRay, shadowHit, tmin) && withinDistanceToLight(light, hit, shadowHit)) {
            //add contribution
            color += hit.getMaterial()->Shade(ray, hit, getDirectionToLight(light, hit), getLightColor(light, hit));
        } else {
            //shadow segment
            RayTree::AddShadowSegment(shadowRay, 0, shadowHit.getT());
        }
    }

    return color;
}

Vec3f
RayTracer::rayCastReflective(Ray &ray, float tmin, int bounces, float weight, float indexOfRefraction, Hit &hit) const {
    Vec3f color;
    Vec3f reflectiveColor = hit.getMaterial()->getReflectiveColor();
    if (reflectiveColor.Length() > 0) {
        Ray reflectRay(hit.getIntersectionPoint(), mirrorDirection(hit.getNormal(), ray.getDirection()));
        Hit reflectHit(reflectRay, INFINITY);

        //add contribution
        color = reflectiveColor * weight *
                 traceRay(reflectRay, tmin, ++bounces, weight * reflectiveColor.Length(), indexOfRefraction,
                          reflectHit);

        if (color.Length() > 0) {
            //reflected segment
            RayTree::AddReflectedSegment(reflectRay, 0, reflectHit.getT());
        }
    }

    return color;
}

Vec3f RayTracer::rayCastTransparent(Ray &ray, float tmin, int bounces, float weight, float indexOfRefraction,
                                    Hit &hit) const {
    Material *material = hit.getMaterial();
    Vec3f transparentColor = material->getTransparentColor();
    Vec3f color;

    if (transparentColor.Length() > 0) {
        Vec3f refractionDirection;

        //if the ray is inside the object or outside it
        float inIndex, outIndex;
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
            Ray refractionRay = Ray(hit.getIntersectionPoint(), refractionDirection);
            Hit refractionHit(refractionRay, INFINITY);

            //add contribution
            color = transparentColor * weight *
                     traceRay(refractionRay, tmin, ++bounces, weight * transparentColor.Length(), outIndex,
                              refractionHit);
            if (color.Length() > 0) {
                //transmitted segment
                RayTree::AddTransmittedSegment(refractionRay, 0, refractionHit.getT());
            }
        }

    }

    return color;
}







