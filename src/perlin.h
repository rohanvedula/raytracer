//
// Created by Rohan Vedula on 2024-07-24.
//

#ifndef PERLIN_H
#define PERLIN_H


float2 randomGradient(int ix, int iy) {
    // No precomputed gradients mean this works for any number of grid coordinates
    const unsigned w = 8 * sizeof(unsigned);
    const unsigned s = w / 2;
    unsigned a = ix, b = iy;
    a *= 3284157443;

    b ^= a << s | a >> w - s;
    b *= 1911520717;

    a ^= b << s | b >> w - s;
    a *= 2048419325;
    float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]

    // Create the vector from the angle
    float2 v;
    v.x = sin(random);
    v.y = cos(random);

    return v;
}

float dotGridGradient(int ix, int iy, float x, float y) {
    // Get gradient from integer coordinates
    float2 gradient = randomGradient(ix, iy);

    // Compute the distance vector
    float dx = x - (float)ix;
    float dy = y - (float)iy;

    // Compute the dot-product
    return (dx * gradient.x + dy * gradient.y);
}

float interpolate(float a0, float a1, float w)
{
    return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
}

float perlin(float x, float y) {

    // Determine grid cell corner coordinates
    int x0 = (int)x;
    int y0 = (int)y;
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    // Compute Interpolation weights
    float sx = x - (float)x0;
    float sy = y - (float)y0;

    // Compute and interpolate top two corners
    float n0 = dotGridGradient(x0, y0, x, y);
    float n1 = dotGridGradient(x1, y0, x, y);
    float ix0 = interpolate(n0, n1, sx);

    // Compute and interpolate bottom two corners
    n0 = dotGridGradient(x0, y1, x, y);
    n1 = dotGridGradient(x1, y1, x, y);
    float ix1 = interpolate(n0, n1, sx);

    // Final step: interpolate between the two previously interpolated values, now in y
    float value = interpolate(ix0, ix1, sy);

    return value;
}


float mod289(float x){return x - floor(x * (1.0f / 289.0f)) * 289.0f;}
float4 mod289(float4 x){return x - floor(x * (1.0f / 289.0f)) * 289.0f;}
float4 perm(float4 x){return mod289(((x * 34.0f) + 1.0f) * x);}
float4 fract4(float4 x) {return x - floor(x);}

float noise2(float3 p){
    float3 a = floor(p);
    float3 d = p - a;
    d = d * d * (3.0f - 2.0f * d);

    float4 b = float4(a.x, a.x, a.y, a.y) + float4(0.0, 1.0, 0.0, 1.0);
    float4 k1 = perm(float4(b.x, b.y, b.x, b.y));
    float4 k2 = perm(float4(k1.x, k1.y, k1.x, k1.y) + float4(b.z, b.z, b.w, b.w));

    float4 c = k2 + float4(a.z, a.z, a.z, a.z);
    float4 k3 = perm(c);
    float4 k4 = perm(c + 1.0f);

    float4 o1 = fract4(k3 * (1.0f / 41.0f));
    float4 o2 = fract4(k4 * (1.0f / 41.0f));

    float4 o3 = o2 * d.z + o1 * (1.0f - d.z);
    float2 o4 = float2(o3.y, o3.w) * d.x + float2(o3.x, o3.z) * (1.0f - d.x);

    return o4.y * d.y + o4.x * (1.0 - d.y);
}

float fbm2(float3 p) {
    p/= 10.0f;
    float total = 0.0;
    float amplitude = 0.5;
    float frequency = 1.0;
    const int octaves = 4;

    for (int i = 0; i < octaves; i++) {
        total += noise2(p * frequency) * amplitude;
        frequency *= 2.0;
        amplitude *= 0.5;
    }

    return total/4+0.25f;
}

#endif //PERLIN_H
