//
// Created by 54179 on 2016/12/3.
//

#include <GL/gl.h>
#include "Grid.h"
#include "phongMaterial.h"
#include "rayTree.h"
#include "RayTracingStats.h"

//#define __DEBUG

Vec3f Grid::colors[] = {
        {.5, .5, .2},
        {.2, .5, .5},
        {.5, .2, .5},
        {.2, .2, .5},
        {.5, .2, .2},
        {.2, .5, .2}
};

Grid::Grid(BoundingBox *bb, int nx, int ny, int nz) {
    size = Vec3f(nx, ny, nz);
    setBoundingBox(bb);
    gridSize = (bb->getMax() - bb->getMin()) / size;
    //initialize opaque array
    opaque = new Object3DVector **[nx];
    for (int i = 0; i < nx; ++i) {
        opaque[i] = new Object3DVector *[ny];
        for (int j = 0; j < ny; ++j) {
            opaque[i][j] = new Object3DVector[nz];
        }
    }

}

bool Grid::traverse(const Ray &r, Hit &h, float tmin) {
    //update RayTracingStats
    RayTracingStats::IncrementNumIntersecstions();

    MarchingInfo mi;
    if (!initializeRayMarch(mi, r, tmin)) return false;
    Vec3i index = mi.getIndex();
    bool isHit = false;
    int colorIndex = 0;
    Vec3f gridSize = (getBoundingBox()->getMax() - getBoundingBox()->getMin()) / getSize();
    mi.setGridSize(gridSize);

    while (index.x() >= 0 && index.x() < getSize().x() &&
           index.y() >= 0 && index.y() < getSize().y() &&
           index.z() >= 0 && index.z() < getSize().z()) {
        Vec3f min = getBoundingBox()->getMin() + index * gridSize;
        Vec3f vertex[] = {
                min,
                min + Vec3f(gridSize.x(), 0, 0),
                min + Vec3f(0, gridSize.y(), 0),
                min + Vec3f(0, 0, gridSize.z()),
                min + Vec3f(gridSize.x(), gridSize.y(), 0),
                min + Vec3f(gridSize.x(), 0, gridSize.z()),
                min + Vec3f(0, gridSize.y(), gridSize.z()),
                min + Vec3f(gridSize.x(), gridSize.y(), gridSize.z())
        };
        Vec3f face[][4] = {
                {vertex[4], vertex[1], vertex[0], vertex[2]},
                {vertex[6], vertex[2], vertex[0], vertex[3]},
                {vertex[5], vertex[3], vertex[0], vertex[1]},
                {vertex[1], vertex[4], vertex[7], vertex[5]},
                {vertex[2], vertex[6], vertex[7], vertex[4]},
                {vertex[3], vertex[5], vertex[7], vertex[6]}
        };
        if (opaque[index.x()][index.y()][index.z()].getNumObjects() > 0) {
            //if hit
            setCurrentColor(index.x(), index.y(), index.z());
            Material *traceMaterial = new PhongMaterial(colors[colorIndex], Vec3f(0, 0, 0), 0, Vec3f(0, 0, 0), Vec3f(0, 0, 0),
                                                   0);

            if (!isHit) {
#ifdef __DEBUG
                h.set(1000, traceMaterial, mi.getNormal(), r);
#else
                h.set(mi.getTmin(), traceMaterial, mi.getNormal(), r);
#endif
#ifdef __VERBOSE
                std::cout << "tMin is set to " << h.getT() << std::endl;
#endif
                isHit = true;
            }

            //add hit cell
            for (int i = 0; i < 6; ++i) {
                Vec3f normal;
                Vec3f::Cross3(normal, face[i][1] - face[i][0], face[i][2] - face[i][1]);
                RayTree::AddHitCellFace(face[i][0], face[i][1], face[i][2], face[i][3], normal, traceMaterial);
            }

            //add entered cell
//            std::cout << h.getNormal().x() << ", " << h.getNormal().y() << ", " << h.getNormal().z() << std::endl;
            int faceIndex = -1;
            if (mi.getNormal().Dot3(Vec3f(0, 1, 1)) == 0) {
                //x axis
                faceIndex = mi.getSign().x() >= 0 ? 1 : 3;
            } else if (mi.getNormal().Dot3(Vec3f(1, 0, 1)) == 0) {
                //y axis
                faceIndex = mi.getSign().y() >= 0 ? 2 : 4;
            } else if (mi.getNormal().Dot3(Vec3f(1, 1, 0)) == 0) {
                //z axis
                faceIndex = mi.getSign().z() >= 0 ? 0 : 5;
            }
            if (faceIndex >= 0)
                RayTree::AddEnteredFace(face[faceIndex][0], face[faceIndex][1], face[faceIndex][2], face[faceIndex][3],
                                        mi.getNormal(), traceMaterial);

        }

        mi.nextCell();
        index = mi.getIndex();

        colorIndex = (++colorIndex) % 6;


#ifdef __VERBOSE
        std::cout << "advance to " << mi.getTmin() << ", index: " << "(" << index.x() << ", " << index.y() << ", " << index.z() << ")" << std::endl;
#endif
    }
    return isHit;
}

void Grid::drawTriangles(const Vec3f &a, const Vec3f &b, const Vec3f &c) {
    Vec3f normal;
    Vec3f::Cross3(normal, b - a, c - a);
    normal.Normalize();
    glColor3f(currentColor.r(), currentColor.g(), currentColor.b());
    glEnable(GL_COLOR_MATERIAL);
    glBegin(GL_TRIANGLES);
    glNormal3f(normal.x(), normal.y(), normal.z());
    glVertex3f(a.x(), a.y(), a.z());
    glVertex3f(b.x(), b.y(), c.z());
    glVertex3f(c.x(), c.y(), c.z());
    glEnd();
    glDisable(GL_COLOR_MATERIAL);
}

void Grid::drawQuads(const Vec3f &a, const Vec3f &b, const Vec3f &c, const Vec3f &d) {
    drawTriangles(a, b, c);
    drawTriangles(d, c, b);
}

void Grid::drawQuadsAtXAxis(const Vec3f &min) {
    Vec3f vecs[] = {
            min + gridSize * Vec3f(0, 0, 0),
            min + gridSize * Vec3f(0, 1, 0),
            min + gridSize * Vec3f(0, 0, 1),
            min + gridSize * Vec3f(0, 1, 1)
    };

    drawQuads(vecs[0], vecs[1], vecs[2], vecs[3]);
}

void Grid::drawQuadsAtYAxis(const Vec3f &min) {
    Vec3f vecs[] = {
            min + gridSize * Vec3f(0, 0, 0),
            min + gridSize * Vec3f(1, 0, 0),
            min + gridSize * Vec3f(0, 0, 1),
            min + gridSize * Vec3f(1, 0, 1)
    };

    drawQuads(vecs[0], vecs[1], vecs[2], vecs[3]);
}

void Grid::drawQuadsAtZAxis(const Vec3f &min) {
    Vec3f vecs[] = {
            min + gridSize * Vec3f(0, 0, 0),
            min + gridSize * Vec3f(1, 0, 0),
            min + gridSize * Vec3f(0, 1, 0),
            min + gridSize * Vec3f(1, 1, 0)
    };

    drawQuads(vecs[0], vecs[1], vecs[2], vecs[3]);
}

Vec3f getGridMin(const Vec3f &min, const Vec3f &gridSize, int i, int j, int k) {
    return min + gridSize * Vec3f(i, j, k);
}

void Grid::paint(void) {

//    for (int i = 0; i < size.x(); ++i) {
//        for (int j = 0; j < size.y(); ++j) {
//            for (int k = 0; k < size.z(); ++k) {
//                if (opaque[i][j][k].getNumObjects() == 0) continue;
//                Vec3f gridStart = min + gridSize * Vec3f(i, j, k);
//                Vec3f color = colors[opaque[i][j][k].getNumObjects() % 6];
//                glColor3f(color.r(), color.g(), color.b());
//                drawBox(gridStart, gridStart + gridSize);
//            }
//        }
//    }

    for (int i = 0; i < size.x(); ++i) {
        for (int j = 0; j < size.y(); ++j) {
            for (int k = 0; k < size.z(); ++k) {
                if (isOpaque(i, j, k)) {
                    setCurrentColor(i, j, k);

                    if (i == 0)
                        drawQuadsAtXAxis(at(i, j, k));
                    if (j == 0)
                        drawQuadsAtYAxis(at(i, j, k));
                    if (k == 0)
                        drawQuadsAtZAxis(at(i, j, k));

                    if (i == size.x() - 1 || !isOpaque(i + 1, j, k)) {
                        drawQuadsAtXAxis(at(i + 1, j, k));
                    }

                    if (j == size.y() - 1 || !isOpaque(i, j + 1, k)) {
                        drawQuadsAtYAxis(at(i, j + 1, k));
                    }

                    if (k == size.z() - 1 || !isOpaque(i, j, k + 1)) {
                        drawQuadsAtZAxis(at(i, j, k + 1));
                    }
                } else {
                    if (i != size.x() - 1 && isOpaque(i + 1, j, k)) {
                        setCurrentColor(i + 1, j, k);
                        drawQuadsAtXAxis(at(i + 1, j, k));
                    }

                    if (j != size.y() - 1 && isOpaque(i, j + 1, k)) {
                        setCurrentColor(i, j + 1, k);
                        drawQuadsAtYAxis(at(i, j + 1, k));
                    }

                    if (k != size.z() - 1 && isOpaque(i, j, k + 1)) {
                        setCurrentColor(i, j, k + 1);
                        drawQuadsAtZAxis(at(i, j, k + 1));
                    }
                }
            }
        }
    }
}

void Grid::setOpaque(int x, int y, int z, Object3D *val) {
    opaque[x][y][z].addObject(val);
}

void Grid::print() {
    for (int i = 0; i < size.x(); ++i) {
        for (int j = 0; j < size.y(); ++j) {
            for (int k = 0; k < size.z(); ++k) {
                std::cout << opaque[i][j][k].getNumObjects() ? "■" : "□";
            }
            std::cout << endl;
        }
        std::cout << endl;
    }
}

float findRemainder(float diver, float divee) {
    while (diver >= 0) diver -= divee;
    return diver + divee;
}

float diff_distance(float dis, float step, float unitSize) {
    if (dis <= 0) dis = unitSize - findRemainder(-dis, unitSize);
    else dis = findRemainder(dis, unitSize);

    if(step == 0) return INFINITY;
    else if (step > 0) return (unitSize - dis) / step;
    else return dis / -step;
}

bool Grid::initializeRayMarch(MarchingInfo &mi, const Ray &r, float tmin) const {
    BoundingBox *bb = getBoundingBox();
    Hit hit_min, hit_max;
    if (!bb->intersect(r, hit_min, hit_max, tmin)) return false;

    mi.setSign(Vec3f(r.getDirection().x() >= 0 ? 1 : -1,
                     r.getDirection().y() >= 0 ? 1 : -1,
                     r.getDirection().z() >= 0 ? 1 : -1));

    Vec3f bbSize = bb->getMax() - bb->getMin();
    Vec3f hitMinPoint = hit_min.getIntersectionPoint();

    mi.setNormal(hit_min.getNormal());

    //if ray origin inside the grid
    if (hit_min.getT() < 0)
        while (hitMinPoint < r.getOrigin()) hitMinPoint += gridSize * mi.getSign();

    //ray origin outside the grid
    float t = hit_min.getT();
    mi.setT_next(t + diff_distance(hitMinPoint.x() + bbSize.x() / 2, r.getDirection().x(), gridSize.x()),
                 t + diff_distance(hitMinPoint.y() + bbSize.y() / 2, r.getDirection().y(), gridSize.y()),
                 t + diff_distance(hitMinPoint.z() + bbSize.z() / 2, r.getDirection().z(), gridSize.z()));
    mi.setTmin(t);

    mi.setD_t(mi.getSign() * gridSize / r.getDirection());

    Vec3i _index = (hitMinPoint - bb->getMin()) / gridSize;
    mi.setIndex(Vec3i(
            _index.x() == size.x() ? _index.x() - 1 : _index.x(),
            _index.y() == size.y() ? _index.y() - 1 : _index.y(),
            _index.z() == size.z() ? _index.z() - 1 : _index.z()
    ));

#ifdef __VERBOSE
    std::cout << "start at " << mi.getTmin() << ", index : (" << mi.getIndex().x() << ", " << mi.getIndex().y() << ", " << mi.getIndex().z() << ")" << std::endl;
#endif
    return true;
}

const Vec3f &Grid::getSize() const {
    return size;
}

void Grid::setSize(const Vec3f &size) {
    Grid::size = size;
}

Grid::~Grid() {
    for (int i = 0; i < size.x(); i++)
        for (int j = 0; j < size.y(); j++)
            delete opaque[i][j];

}

void Grid::drawBox(const Vec3f &min, const Vec3f &max) {
    float length = (float) sqrt(pow((max - min).Length(), 2) / 3);
    Vec3f vertices[] = {
            min,
            min + Vec3f(length, 0, 0),
            min + Vec3f(0, length, 0),
            min + Vec3f(0, 0, length),
            min + Vec3f(length, length, 0),
            min + Vec3f(length, 0, length),
            min + Vec3f(0, length, length),
            min + Vec3f(length, length, length)
    };
//
//    for (int i = 0; i < 8; ++i) {
//        std::cout << "vertex[" << i << "]: (" << vertices[i].x() << ", " << vertices[i].y() << ", " << vertices[i].z() << ")" << std::endl;
//    }

    drawQuads(vertices[0], vertices[1], vertices[2], vertices[4]);
    drawQuads(vertices[0], vertices[1], vertices[3], vertices[5]);
    drawQuads(vertices[0], vertices[2], vertices[3], vertices[6]);
    drawQuads(vertices[7], vertices[5], vertices[4], vertices[1]);
    drawQuads(vertices[7], vertices[6], vertices[4], vertices[2]);
    drawQuads(vertices[7], vertices[5], vertices[6], vertices[3]);
}

bool Grid::isOpaque(int i, int j, int k) {
    return opaque[i][j][k].getNumObjects() != 0;
}

Vec3f Grid::at(int i, int j, int k) {
    return getBoundingBox()->getMin() + gridSize * Vec3f(i, j, k);
}

void Grid::setCurrentColor(int i, int j, int k) {
    currentColor = colors[opaque[i][j][k].getNumObjects() % 6];
}

Vec3f &Grid::getCurrentColor() {
    return currentColor;
}

bool Grid::intersect(const Ray &r, Hit &h, float tmin) {
    //update RayTracingStats
    RayTracingStats::IncrementNumIntersecstions();

    MarchingInfo mi;
    if (!initializeRayMarch(mi, r, tmin)) return false;
    Vec3i index = mi.getIndex();

    while (index.x() >= 0 && index.x() < getSize().x() &&
           index.y() >= 0 && index.y() < getSize().y() &&
           index.z() >= 0 && index.z() < getSize().z()) {
        Object3DVector objects = opaque[index.x()][index.y()][index.z()];
        if (objects.getNumObjects() > 0) {
            //if hit
            bool hit = false;
            h.set(INFINITY, h.getMaterial(), h.getNormal(), r);
            for (int i = 0; i <  objects.getNumObjects(); ++i) {
                Hit _h;
                if (objects.getObject(i)->intersect(r, _h, tmin)) {
                    hit = true;
                    if (h.getT() > _h.getT()) h = _h;
                };
            }
            return hit;
        }

        mi.nextCell();
        index = mi.getIndex();

    }
    return false;
}
