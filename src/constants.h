//
// Created by Rohan Vedula on 2024-07-14.
//

#ifndef CONSTANTS_H
#define CONSTANTS_H

constexpr int globalWidth = 512;
constexpr int globalHeight = 384;


// degree and radian
constexpr float PI = 3.14159265358979f;
constexpr float DegToRad = PI / 180.0f;
constexpr float RadToDeg = 180.0f / PI;


// for ray tracing
constexpr float Epsilon = 5e-5f;


// amount the camera moves with a mouse and a keyboard
constexpr float ANGFACT = 0.2f;
constexpr float SCLFACT = 0.1f;


// fixed camera parameters
constexpr float globalAspectRatio = float(globalWidth / float(globalHeight));
constexpr float globalFOV = 45.0f; // vertical field of view
constexpr float globalDepthMin = Epsilon; // for rasterization
constexpr float globalDepthMax = 100.0f; // for rasterization
constexpr float globalFilmSize = 0.032f; //for ray tracing
const float globalDistanceToFilm = globalFilmSize / (2.0f * tan(globalFOV * DegToRad * 0.5f)); // for ray tracing


// particle system related
bool globalEnableParticles = false;
constexpr float deltaT = 0.002f; // changed from 0.002f
constexpr float globalParticleMass = 0.2;
constexpr float3 globalGravity = float3(0.0f, -9.8f, 0.0f);
constexpr int globalNumParticles = 20;


// dynamic camera parameters
float3 globalEye = float3(0.0f, 0.0f, 1.5f);
float3 globalLookat = float3(0.0f, 0.0f, 0.0f);
float3 globalUp = normalize(float3(0.0f, 1.0f, 0.0f));
float3 globalViewDir; // should always be normalize(globalLookat - globalEye)
float3 globalRight; // should always be normalize(cross(globalViewDir, globalUp));
bool globalShowRaytraceProgress = false; // for ray tracing


// mouse event
static bool mouseLeftPressed;
static double m_mouseX = 0.0;
static double m_mouseY = 0.0;


// rendering algorithm
enum enumRenderType {
    RENDER_RASTERIZE,
    RENDER_RAYTRACE,
    RENDER_IMAGE,
};
enumRenderType globalRenderType = RENDER_IMAGE;
int globalFrameCount = 0;
static bool globalRecording = false;
static GifWriter globalGIFfile;
constexpr int globalGIFdelay = 1;

#endif //CONSTANTS_H