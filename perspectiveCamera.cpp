//
// Created by 54179 on 2016/10/12.
//

#include <GL/gl.h>
#include <GL/glu.h>
#include "perspectiveCamera.h"
#include "matrix.h"

PerspectiveCamera::PerspectiveCamera(Vec3f center, Vec3f &direction, Vec3f &up, float angle) : center(center),
                                                                                               direction(direction),
                                                                                               angle(angle) {
    this->direction.Normalize();
    this->up = up - up * this->direction;
    this->up.Normalize();
    Vec3f::Cross3(this->horizontal, this->direction, this->up);
    this->horizontal.Normalize();
}

Ray PerspectiveCamera::generateRay(Vec2f point) {
    Vec3f dirX = point.x() * tan(angle / 2) * horizontal;
    Vec3f dirY = point.y() * tan(angle / 2) * up;
//    Vec3f dirX = tan(angle / 2 * point.x()) * horizontal;
//    Vec3f dirY = tan(angle / 2 * point.y()) * up;
    Vec3f dir = direction + dirX + dirY;
    dir.Normalize();

    return Ray(center, dir);
}

float PerspectiveCamera::getTMin() const {
    return 0;
}

// ====================================================================
// ====================================================================
// ====================================================================
// Create a perspective camera with the appropriate dimensions that
// crops or stretches in the x-dimension as necessary
// ====================================================================
void PerspectiveCamera::glInit(int w, int h) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(angle * 180.0 / 3.14159, (float) w / float(h), 0.5, 40.0);
}

// ====================================================================
// Place a perspective camera within an OpenGL scene
// ====================================================================
void PerspectiveCamera::glPlaceCamera(void) {
    gluLookAt(center.x(), center.y(), center.z(), center.x() + direction.x(), center.y() + direction.y(),
              center.z() + direction.z(), up.x(), up.y(), up.z());
}

// ====================================================================
// dollyCamera, truckCamera, and RotateCamera
//
// Asumptions:
// - up is really up (i.e., it hasn't been changed
// to point to "screen up")
// - up and direction are normalized
// Special considerations:
// - If your constructor precomputes any vectors for
// use in 'generateRay', you will likely to recompute those
// values at athe end of the these three routines
// ====================================================================

// ====================================================================
// dollyCamera: Move camera along the direction vector
// ====================================================================
void PerspectiveCamera::dollyCamera(float dist) {
    center += direction * dist;
    // ===========================================
    // ASSIGNMENT 3: Fix any other affected values
    // ===========================================
}

// ====================================================================
// truckCamera: Translate camera perpendicular to the direction vector
// ====================================================================
void PerspectiveCamera::truckCamera(float dx, float dy) {
    Vec3f screenUp;
    Vec3f::Cross3(screenUp, horizontal, direction);
    center += horizontal * dx + screenUp * dy;
    // ===========================================
    // ASSIGNMENT 3: Fix any other affected values
    // ===========================================
}

// ====================================================================
// rotateCamera: Rotate around the up and horizontal vectors
// ====================================================================
void PerspectiveCamera::rotateCamera(float rx, float ry) {
    // Don't let the model flip upside-down (There is a singularity
    // at the poles when 'up' and 'direction' are aligned)
    float tiltAngle = acos(up.Dot3(direction));
    if (tiltAngle - ry > 3.13) ry = tiltAngle - 3.13; else if (tiltAngle - ry < 0.01) ry = tiltAngle - 0.01;
    Matrix rotMat = Matrix::MakeAxisRotation(up, rx);
    rotMat *= Matrix::MakeAxisRotation(horizontal, ry);
    rotMat.Transform(center);
    rotMat.TransformDirection(direction);
    direction.Normalize();
    // ===========================================
    // ASSIGNMENT 3: Fix any other affected values
    // ===========================================
    rotMat.TransformDirection(up);
    rotMat.TransformDirection(horizontal);
    direction.Normalize();
    up.Normalize();
    horizontal.Normalize();
}

// ====================================================================
// ====================================================================
