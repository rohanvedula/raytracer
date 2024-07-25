//
// Created by Rohan Vedula on 2024-07-25.
//

#ifndef AABB_H
#define AABB_H
#include "structures.h"

class AABB {
private:
    float3 minp, maxp, size;

public:
    float3 get_minp() { return minp; };
    float3 get_maxp() { return maxp; };
    float3 get_size() { return size; };


    AABB() {
        minp = float3(FLT_MAX);
        maxp = float3(-FLT_MAX);
        size = float3(0.0f);
    }

    void reset() {
        minp = float3(FLT_MAX);
        maxp = float3(-FLT_MAX);
        size = float3(0.0f);
    }

    int getLargestAxis() const {
        if ((size.x > size.y) && (size.x > size.z)) {
            return 0;
        } else if (size.y > size.z) {
            return 1;
        } else {
            return 2;
        }
    }

    void fit(const float3& v) {
        if (minp.x > v.x) minp.x = v.x;
        if (minp.y > v.y) minp.y = v.y;
        if (minp.z > v.z) minp.z = v.z;

        if (maxp.x < v.x) maxp.x = v.x;
        if (maxp.y < v.y) maxp.y = v.y;
        if (maxp.z < v.z) maxp.z = v.z;

        size = maxp - minp;
    }

    float area() const {
        return (2.0f * (size.x * size.y + size.y * size.z + size.z * size.x));
    }


    bool intersect(HitInfo& minHit, const Ray& ray) const {
        // set minHit.t as the distance to the intersection point
        // return true/false if the ray hits or not
        float tx1 = (minp.x - ray.o.x) / ray.d.x;
        float ty1 = (minp.y - ray.o.y) / ray.d.y;
        float tz1 = (minp.z - ray.o.z) / ray.d.z;

        float tx2 = (maxp.x - ray.o.x) / ray.d.x;
        float ty2 = (maxp.y - ray.o.y) / ray.d.y;
        float tz2 = (maxp.z - ray.o.z) / ray.d.z;

        if (tx1 > tx2) {
            const float temp = tx1;
            tx1 = tx2;
            tx2 = temp;
        }

        if (ty1 > ty2) {
            const float temp = ty1;
            ty1 = ty2;
            ty2 = temp;
        }

        if (tz1 > tz2) {
            const float temp = tz1;
            tz1 = tz2;
            tz2 = temp;
        }

        float t1 = tx1; if (t1 < ty1) t1 = ty1; if (t1 < tz1) t1 = tz1;
        float t2 = tx2; if (t2 > ty2) t2 = ty2; if (t2 > tz2) t2 = tz2;

        if (t1 > t2) return false;
        if ((t1 < 0.0) && (t2 < 0.0)) return false;

        minHit.t = t1;
        return true;
    }
};

#endif //AABB_H
