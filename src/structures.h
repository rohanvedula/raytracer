//
// Created by Rohan Vedula on 2024-07-25.
//

#ifndef STRUCTURES_H
#define STRUCTURES_H

#include "material.h"

class HitInfo {
public:
    float t = FLT_MAX; // distance
    float3 P; // location
    float3 N; // shading normal vector
    float2 T; // texture coordinate
    float3x3 pertubeMatrix;
    const Material* material; // const pointer to the material of the intersected object
};


class PointLightSource {
public:
    float3 position, wattage;
};



class Ray {
public:
    float3 o, d;
    Ray() : o(), d(float3(0.0f, 0.0f, 1.0f)) {}
    Ray(const float3& o, const float3& d) : o(o), d(d) {}
};


#endif //STRUCTURES_H
