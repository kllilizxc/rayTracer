#define __VERBOSE

#include <iostream>
#include <cstring>
#include <assert.h>
#include <cstdlib>
#include <GL/glut.h>
#include "scene_parser.h"
#include "glCanvas.h"
#include "image.h"
#include "rayTracer.h"
#include "RayTracingStats.h"

using namespace std;

char *input_file = NULL;
int width = 100;
int height = 100;
char *output_file = NULL;
double depth_min = 0;
double depth_max = 1;
char *depth_file = NULL;
char *normal_file = NULL;
bool gui = false;
int bounces;
float weight;
bool shadows = false;
SceneParser *sp;
Camera *camera;
bool Material::shade_back = false;
Grid *grid = NULL;
bool visualize_grid = false;
int nx, ny, nz;
bool stats = false;

void traceRay(float x, float y) {
    std::cout << "trace a new ray" << std::endl;

//    x = 0.08375; y = 0.59375; //debug //(0.54125, 0.49125)

    Ray ray = camera->generateRay(Vec2f((float) 2.0 * x - 1, (float) 2.0 * y - 1));
    Hit hit;

    //debug
#ifdef __VERBOSE
    cout << "(" << 2.0 * x - 1 << ", " << 2.0 * y - 1 << ")" << endl;
#endif

    //update ray tracing stats
    if (shadows)
        RayTracingStats::IncrementNumShadowRays();
    else
        RayTracingStats::IncrementNumNonShadowRays();

    //diffuse color
    RayTracer rayTracer(sp, bounces, weight, shadows, grid, visualize_grid);
    rayTracer.traceRay(ray, camera->getTMin(), 0, 1, 1, hit);
}

void render() {
    Image colorResult(width, height), depthResult(width, height), normalResult(width, height);
    Vec3f backgroundColor = sp->getBackgroundColor();
    float tMin = camera->getTMin();
    RayTracer rayTracer(sp, bounces, weight, shadows, grid, visualize_grid);

    colorResult.SetAllPixels(backgroundColor);
    depthResult.SetAllPixels(backgroundColor);
    normalResult.SetAllPixels(backgroundColor);

    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
//            i = 34, j = 29; //debug
            float x = (float) (2.0 * i / width - 1), y = (float) (2.0 * j / height - 1);

//            x = -0.8275; y = 0.2075; //debug

            Ray ray = camera->generateRay(Vec2f(x, y));
            Hit hit;

            //color
            Vec3f color = rayTracer.traceRay(ray, tMin, 0, 1, 1, hit);

            //color result
            if (output_file && color.Length() > 0)
                colorResult.SetPixel(i, j, color);
            //depth = depth > 1 ? 1 : depth < 0 ? 0 : depth;
            if (depth_file) {
                double depth = (hit.getT() - depth_min) / (depth_max - depth_min);
                depth = 1 - depth / 100; //adjust depth manually TODO
                depthResult.SetPixel(i, j, Vec3f(1, 1, 1) * depth);
            }
            //normal result
            if (normal_file) {
                Vec3f normal = hit.getNormal();
                normal = Vec3f(fabsf(normal.x()), fabsf(normal.y()), fabsf(normal.z()));
                normalResult.SetPixel(i, j, normal);
            }
        }
    }

    if(stats)
        RayTracingStats::PrintStatistics();

    if (output_file) colorResult.SaveTGA(output_file);
    if (depth_file) depthResult.SaveTGA(depth_file);
    if (normal_file) normalResult.SaveTGA(normal_file);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-input")) {
            i++;
            assert (i < argc);
            input_file = argv[i];
        } else if (!strcmp(argv[i], "-size")) {
            i++;
            assert (i < argc);
            width = atoi(argv[i]);
            i++;
            assert (i < argc);
            height = atoi(argv[i]);
        } else if (!strcmp(argv[i], "-output")) {
            i++;
            assert (i < argc);
            output_file = argv[i];
        } else if (!strcmp(argv[i], "-depth")) {
            i++;
            assert (i < argc);
            depth_min = atof(argv[i]);
            i++;
            assert (i < argc);
            depth_max = atof(argv[i]);
            i++;
            assert (i < argc);
            depth_file = argv[i];
        } else if (!strcmp(argv[i], "-normals")) {
            i++;
            assert (i < argc);
            normal_file = argv[i];
        } else if (!strcmp(argv[i], "-shade_back")) {
            Material::shade_back = true;
        } else if (!strcmp(argv[i], "-gui")) {
            gui = true;
        } else if (!strcmp(argv[i], "-tessellation")) {
            i++;
            assert (i < argc);
            Sphere::thetaSteps = atoi(argv[i]);
            i++;
            assert (i < argc);
            Sphere::phiSteps = atoi(argv[i]);
        } else if (!strcmp(argv[i], "-gouraud")) {
            Sphere::gouraud = true;
        } else if (!strcmp(argv[i], "-shadows")) {
            shadows = true;
        } else if (!strcmp(argv[i], "-bounces")) {
            i++;
            assert(i < argc);
            bounces = atoi(argv[i]);
        } else if (!strcmp(argv[i], "-weight")) {
            i++;
            assert(i < argc);
            weight = (float) atof(argv[i]);
        } else if (!strcmp(argv[i], "-grid")) {
            i++;
            assert(i < argc);
            nx = atoi(argv[i]);
            i++;
            assert(i < argc);
            ny = atoi(argv[i]);
            i++;
            assert(i < argc);
            nz = atoi(argv[i]);
        } else if (!strcmp(argv[i], "-visualize_grid")) {
            visualize_grid = true;
        } else if(!strcmp(argv[i], "-stats")) {
            stats = true;
        } else {
            cout << "whoops error with command line argument " << i << ": '" << argv[i] << "'" << endl;
            assert(0);
        }
    }

    sp = new SceneParser(input_file);
    camera = sp->getCamera();
    Group *group = sp->getGroup();
    if(visualize_grid) {
        grid = new Grid(group->getBoundingBox(), nx, ny, nz);
        RayTracingStats::Initialize(width, height, group->getBoundingBox(), nx, ny, nz);
    }

    if (gui) {
        GLCanvas canvas;
        canvas.initialize(sp, render, traceRay, grid, visualize_grid);
    }
}