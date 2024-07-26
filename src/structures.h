//
// Created by Rohan Vedula on 2024-07-25.
//

#ifndef STRUCTURES_H
#define STRUCTURES_H

#include "material.h"
#include "randomnum.h"

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
    float radius = 50.0f;

    float3 getRandomPoint(int i)
    {
        if(i==0)
            return position;

        float theta = PCG32::rand()* 2.0f * M_PI; // Azimuthal angle
        float phi = acos(2.0f * PCG32::rand() - 1.0f); // Polar angle

        // Convert spherical coordinates to Cartesian coordinates
        float x = radius * sin(phi) * cos(theta);
        float y = radius * sin(phi) * sin(theta);
        float z = radius * cos(phi);

        // Return the sampled point on the sphere translated by the sphere's center position P
        return float3(x, y, z) + position;
    }
};



class Ray {
public:
    float3 o, d;
    Ray() : o(), d(float3(0.0f, 0.0f, 1.0f)) {}
    Ray(const float3& o, const float3& d) : o(o), d(d) {}
};


#endif //STRUCTURES_H
