//
// Created by 54179 on 2016/12/3.
//

#include <GL/gl.h>
#include "Grid.h"
#include "rayTree.h"
#include "phongMaterial.h"

//#define __DEBUG

Grid::Grid(BoundingBox *bb, int nx, int ny, int nz) {
    size = Vec3f(nx, ny, nz);
    setBoundingBox(bb);
    //initialize opaque array
    opaque = new Object3DVector **[nx];
    for (int i = 0; i < nx; ++i) {
        opaque[i] = new Object3DVector *[ny];
        for (int j = 0; j < ny; ++j) {
            opaque[i][j] = new Object3DVector[nz];
        }
    }
}

bool Grid::intersect(const Ray &r, Hit &h, float tmin) {
    MarchingInfo mi;
    if (!initializeRayMarch(mi, r, tmin)) return false;
    Vec3i index = mi.getIndex();
    int colorIndex = 0;
    bool isHit = false;
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
        Vec3f color[] = {
                Vec3f(1, 1, 1),
                Vec3f(0, 1, 1),
                Vec3f(1, 0, 1),
                Vec3f(1, 1, 0),
                Vec3f(0, 0, 1),
                Vec3f(0, 1, 0),
                Vec3f(1, 0, 0)
        };
        if (opaque[index.x()][index.y()][index.z()].getNumObjects() > 0) {
            //if hit
            Material *material = new PhongMaterial(color[colorIndex], Vec3f(0, 0, 0), 0, Vec3f(0, 0, 0), Vec3f(0, 0, 0),
                                                   0);

            if (!isHit) {
#ifdef __DEBUG
                h.set(1000, material, mi.getNormal(), r);
#else
                h.set(mi.getTmin(), material, mi.getNormal(), r);
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
                RayTree::AddHitCellFace(face[i][0], face[i][1], face[i][2], face[i][3], normal, material);
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
                                        mi.getNormal(), material);

        }

        mi.nextCell();
        index = mi.getIndex();


#ifdef __VERBOSE
        std::cout << "advance to " << mi.getTmin() << ", index: " << "(" << index.x() << ", " << index.y() << ", " << index.z() << ")" << std::endl;
#endif

        colorIndex++;
        if (colorIndex >= 7) colorIndex = 0;
    }
    return isHit;
}

void drawTriangles(const Vec3f &a, const Vec3f &b, const Vec3f &c) {
    Vec3f normal;
    Vec3f::Cross3(normal, b - a, c - a);
    normal.Normalize();
    glBegin(GL_TRIANGLES);
    glNormal3f(normal.x(), normal.y(), normal.z());
    glVertex3f(a.x(), a.y(), a.z());
    glVertex3f(b.x(), b.y(), c.z());
    glVertex3f(c.x(), c.y(), c.z());
    glEnd();
}

void drawQuads(const Vec3f &a, const Vec3f &b, const Vec3f &c, const Vec3f &d) {
    drawTriangles(a, b, c);
    drawTriangles(d, c, b);
}

void drawQuadsAtXAxis(const Vec3f &min, const Vec3f &gridSize) {
    Vec3f vecs[] = {
            min + gridSize * Vec3f(0, 0, 0),
            min + gridSize * Vec3f(0, 1, 0),
            min + gridSize * Vec3f(0, 0, 1),
            min + gridSize * Vec3f(0, 1, 1)
    };

    drawQuads(vecs[0], vecs[1], vecs[2], vecs[3]);
}

void drawQuadsAtYAxis(const Vec3f &min, const Vec3f &gridSize) {
    Vec3f vecs[] = {
            min + gridSize * Vec3f(0, 0, 0),
            min + gridSize * Vec3f(1, 0, 0),
            min + gridSize * Vec3f(0, 0, 1),
            min + gridSize * Vec3f(1, 0, 1)
    };

    drawQuads(vecs[0], vecs[1], vecs[2], vecs[3]);
}

void drawQuadsAtZAxis(const Vec3f &min, const Vec3f &gridSize) {
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
    BoundingBox *bb = getBoundingBox();
    Vec3f min = bb->getMin();
    Vec3f max = bb->getMax();
    Vec3f gridSize = (max - min) / size;

    Vec3f colors[] = {
            {.5, .5, .2},
            {.2, .5, .5},
            {.5, .2, .5},
            {.2, .2, .5},
            {.5, .2, .2},
            {.2, .5, .2}
    };

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


    //x-axis
    for (int i = 0; i < size.z(); ++i) {
        for (int j = 0; j < size.y(); ++j) {
            for (int k = 0; k < size.x(); ++k) {
                if (isOpaque(k, j, i)) {
                    Vec3f color = colors[opaque[k][j][i].getNumObjects() % 6];
                    glColor3f(color.r(), color.g(), color.b());
                    if (k == 0) {
                        Vec3f quadMin = getGridMin(min, gridSize, k, j, i);
                        drawQuadsAtXAxis(quadMin, gridSize);
                    }

                    if (k == size.x() - 1 || !isOpaque(k + 1, j, i)) {
                        Vec3f quadMin = getGridMin(min, gridSize, k + 1, j, i);
                        drawQuadsAtXAxis(quadMin, gridSize);
                    }
                } else {
                    if(k == size.x() - 1) continue;
                    else if (isOpaque(k + 1, j, i)) {
                        Vec3f color = colors[opaque[k + 1][j][i].getNumObjects() % 6];
                        glColor3f(color.r(), color.g(), color.b());
                        Vec3f quadMin = getGridMin(min, gridSize, k + 1, j, i);
                        drawQuadsAtXAxis(quadMin, gridSize);
                    }
                }
            }
        }
    }

    //y-axis
    for (int i = 0; i < size.x(); ++i) {
        for (int j = 0; j < size.z(); ++j) {
            for (int k = 0; k < size.y(); ++k) {
                if (isOpaque(i, k, j)) {
                    Vec3f color = colors[opaque[i][k][j].getNumObjects() % 6];
                    glColor3f(color.r(), color.g(), color.b());
                    if (k == 0) {
                        Vec3f quadMin = getGridMin(min, gridSize, i, k, j);
                        drawQuadsAtYAxis(quadMin, gridSize);
                    }

                    if (k == size.y() - 1 || !isOpaque(i, k + 1, j)) {
                        Vec3f quadMin = getGridMin(min, gridSize, i, k + 1, j);
                        drawQuadsAtYAxis(quadMin, gridSize);
                    }
                } else {
                    if(k == size.y() - 1) continue;
                    else if (isOpaque(i, k + 1, j)) {
                        Vec3f color = colors[opaque[i][k + 1][j].getNumObjects() % 6];
                        glColor3f(color.r(), color.g(), color.b());
                        Vec3f quadMin = getGridMin(min, gridSize, i, k + 1, j);
                        drawQuadsAtYAxis(quadMin, gridSize);
                    }
                }
            }
        }
    }

    //z-axis
    for (int i = 0; i < size.x(); ++i) {
        for (int j = 0; j < size.y(); ++j) {
            for (int k = 0; k < size.z(); ++k) {
                if (isOpaque(i, j, k)) {
                    Vec3f color = colors[opaque[i][j][k].getNumObjects() % 6];
                    glColor3f(color.r(), color.g(), color.b());
                    if (k == 0) {
                        Vec3f quadMin = getGridMin(min, gridSize, i, j, k);
                        drawQuadsAtZAxis(quadMin, gridSize);
                    }

                    if (k == size.z() - 1 || !isOpaque(i, j, k + 1)) {
                        Vec3f quadMin = getGridMin(min, gridSize, i, j, k + 1);
                        drawQuadsAtZAxis(quadMin, gridSize);
                    }
                } else {
                    if(k == size.z() - 1) continue;
                    else if (isOpaque(i, j, k + 1)) {
                        Vec3f color = colors[opaque[i][j][k + 1].getNumObjects() % 6];
                        glColor3f(color.r(), color.g(), color.b());
                        Vec3f quadMin = getGridMin(min, gridSize, i, j, k + 1);
                        drawQuadsAtZAxis(quadMin, gridSize);
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
    float eps = 0.001;
    while (diver >= eps) diver -= divee;
    return diver + divee;
}

float diff_distance(float dis, float step, float unitSize) {
    if (dis <= 0) dis = unitSize - findRemainder(-dis, unitSize);
    else dis = findRemainder(dis, unitSize);

    if (step > 0) return (unitSize - dis) / step;
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
    Vec3f gridSize = bbSize / size;
    Vec3f hitMinPoint = hit_min.getIntersectionPoint();

    mi.setNormal(hit_min.getNormal());

    if (hit_min.getT() >= 0) {
        //ray origin outside the grid
        float t = hit_min.getT();
        mi.setT_next(t + diff_distance(hitMinPoint.x() + bbSize.x() / 2, r.getDirection().x(), gridSize.x()),
                     t + diff_distance(hitMinPoint.y() + bbSize.y() / 2, r.getDirection().y(), gridSize.y()),
                     t + diff_distance(hitMinPoint.z() + bbSize.z() / 2, r.getDirection().z(), gridSize.z()));
        mi.setTmin(t);
//        std::cout << mi.getT_next().x() << ", " << mi.getT_next().y() << ", " << mi.getT_next().z() << std::endl;
//        std::cout << mi.getT_next().getMin() << std::endl;
    } else {
        //ray origin inside the grid TODO
        Vec3f relativeOrigin = r.getOrigin() - bb->getMin();

        Vec3f originIndex = relativeOrigin / gridSize;
        originIndex.Set((int) originIndex.x(), (int) originIndex.y(), (int) originIndex.z());

        Vec3f remainDistance = (originIndex + mi.getSign()) * gridSize - relativeOrigin;
        Vec3f t = remainDistance / r.getDirection();

        float t_min = min3(t.x(), t.y(), t.z());
        mi.setT_next(Vec3f(t_min, t_min, t_min));
        mi.setTmin(t_min);
    }

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
