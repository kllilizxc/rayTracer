//
// Created by 54179 on 2016/10/2.
//

#ifndef ASSIGNMENT_5_GROUP_H
#define ASSIGNMENT_5_GROUP_H

#include <vector>
#include "object3D.h"

class Group : public Object3D {
public:
    Group(unsigned int size = 0) : size(size) {
        instances.resize(size);
    };

    virtual bool intersect(const Ray &r, Hit &h, float tmin) {
        bool hit = false;
        h.set(INFINITY, h.getMaterial(), h.getNormal(), r);
        for (int i = 0; i < size; ++i) {
            Hit _h;
            if (instances[i]->intersect(r, _h, tmin)) {
                hit = true;
                if (h.getT() > _h.getT()) h = _h;
            };
        }
        return hit;
    };

    bool shadowIntersect(const Ray &r, Hit &h, float tmin, float distanceToLight) {
        for (int i = 0; i < size; ++i) {
            if (instances[i]->intersect(r, h, tmin) && r.getDirection().Dot3(h.getNormal()) < 0 &&
                h.getT() < distanceToLight)
                return true;
        }
        return false;
    };

    void addObject(int index, Object3D *obj) {
        instances[index] = obj;
        BoundingBox *bb = obj->getBoundingBox();
        if(bb == nullptr) return;
        if (!getBoundingBox())
            setBoundingBox(bb);
        else
            getBoundingBox()->Extend(bb);
    };

    virtual void paint(void) {
        for (int i = 0; i < size; i++)
            instances[i]->paint();
    }

    void insertIntoGrid(Grid *g, Matrix *m) {
        for (int i = 0; i < size; ++i) {
            instances[i]->insertIntoGrid(g, m);
        }
    }

private:
    unsigned int size;
    std::vector<Object3D *> instances;
};


#endif //ASSIGNMENT_1_GROUP_H
