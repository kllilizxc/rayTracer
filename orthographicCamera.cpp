//
// Created by 54179 on 2016/10/2.

#include <GL/glu.h>
#include "orthographicCamera.h"
#include "matrix.h"

Ray OrthographicCamera::generateRay(Vec2f point) {
    Vec3f temp = center + up * (point.y() * imageSize / 2)  + horizontal * (point.x() * imageSize / 2);
    return Ray(temp, direction);
}

float OrthographicCamera::getTMin() const {
    return -INFINITY;
}

OrthographicCamera::OrthographicCamera(Vec3f center, Vec3f direction, Vec3f up, float imageSize) {
    this->center = center;
    direction.Normalize();
    this->direction = direction;
    up = up - up * direction;
    up.Normalize();
    this->up = up;
    Vec3f::Cross3(this->horizontal, this->direction, this->up);
    this->horizontal.Normalize();
    this->imageSize = imageSize;
}

// ====================================================================
// Create an orthographic camera with the appropriate dimensions that
// crops the screen in the narrowest dimension.
// ====================================================================
void OrthographicCamera::glInit(int w, int h) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w > h)
        glOrtho(-imageSize / 2.0, imageSize / 2.0, -imageSize * (float) h / (float) w / 2.0,
                imageSize * (float) h / (float) w / 2.0, 0.5, 40.0);
    else
        glOrtho(-imageSize * (float) w / (float) h / 2.0, imageSize * (float) w / (float) h / 2.0, -imageSize / 2.0,
                imageSize / 2.0, 0.5, 40.0);
}

// ====================================================================
// Place an orthographic camera within an OpenGL scene
// ====================================================================
void OrthographicCamera::glPlaceCamera(void) {
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

void OrthographicCamera::dollyCamera(float dist) {
    center += direction * dist;
// ===========================================
// ASSIGNMENT 3: Fix any other affected values
// ===========================================
}

// ====================================================================
// truckCamera: Translate camera perpendicular to the direction vector
// ====================================================================
void OrthographicCamera::truckCamera(float dx, float dy) {
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
void OrthographicCamera::rotateCamera(float rx, float ry) {
    // Don't let the model flip upside-down (There is a singularity
    // at the poles when 'up' and 'direction' are aligned)
    float tiltAngle = acos(up.Dot3(direction));
    if (tiltAngle - ry > 3.13) ry = tiltAngle - 3.13; else if (tiltAngle - ry < 0.01) ry = tiltAngle - 0.01;
    Matrix rotMat = Matrix::MakeAxisRotation(up, rx);
    rotMat *= Matrix::MakeAxisRotation(horizontal, ry);
    rotMat.Transform(center);
    rotMat.TransformDirection(direction);
    // ===========================================
    // ASSIGNMENT 3: Fix any other affected values
    // ===========================================
    rotMat.TransformDirection(up);
    rotMat.TransformDirection(horizontal);
    direction.Normalize();
    up.Normalize();
    horizontal.Normalize();
}