//
// Created by Rohan Vedula on 2024-07-25.
//

#ifndef SCENE_H
#define SCENE_H

#include "structures.h"
#include "trianglemesh.h"
#include "bvh.h"
#include "randomnum.h"
#include "constants.h"

extern Image I;
extern GLFWwindow* globalGLFWindow;

static float3 get_from_image(const float3& ray)
{
	float r = (1/PI)*acos(ray.z)/sqrt(pow(ray.x,2) + pow(ray.y, 2));
	float2 pixel = {r*ray.x, r*ray.y};
	pixel = pixel/2 + float2{0.5, 0.5};
	return I.pixel(int(pixel[0]*I.width), int(pixel[1]*I.height));
}

static float beerLambert(float absorb, float marchSize)
{
	return exp(-absorb * marchSize);
}

float3 lerp(const float3& a, const float3& b, float t) {
	return a * (1.0f - t) + b * t;
}

// scene definition
class Scene {
public:
	std::vector<TriangleMesh*> objects;
	std::vector<PointLightSource*> pointLightSources;
	std::vector<BVH> bvhs;

	void addObject(TriangleMesh* pObj) {
		objects.push_back(pObj);
	}
	void addLight(PointLightSource* pObj) {
		pointLightSources.push_back(pObj);
	}

	void preCalc() {
		bvhs.resize(objects.size());
		for (int i = 0; i < objects.size(); i++) {
			objects[i]->preCalc();
			bvhs[i].build(objects[i]);
		}
	}

	// ray-scene intersection
	bool intersect(HitInfo& minHit, const Ray& ray, float tMin = 0.0f, float tMax = FLT_MAX) const {
		bool hit = false;
		HitInfo tempMinHit;
		minHit.t = FLT_MAX;

		for (int i = 0, i_n = (int)objects.size(); i < i_n; i++) {
			//if (objects[i]->bruteforceIntersect(tempMinHit, ray, tMin, tMax)) { // for debugging
			if (bvhs[i].intersect(tempMinHit, ray, tMin, tMax)) {
				if (tempMinHit.t < minHit.t) {
					hit = true;
					minHit = tempMinHit;
				}
			}
		}
		return hit;
	}

	static float2 randomAperture(int itr)
	{
		if(itr==0)
			return float2(0.0f);

		float angle = PCG32::rand()*2.0f * PI;
		float radius = PCG32::rand();

		float2 offset = float2(cos(angle), sin(angle)) * radius * ApertureRadius;
		return offset;
	}

	// eye ray generation (given to you for A2)
	Ray eyeRay(int x, int y, int itr) const {
		// compute the camera coordinate system
		const float3 wDir = normalize(float3(-globalViewDir));
		const float3 uDir = normalize(cross(globalUp, wDir));
		const float3 vDir = cross(wDir, uDir);

		// compute the pixel location in the world coordinate system using the camera coordinate system
		// trace a ray through the center of each pixel
		const float imPlaneUPos = (x + 0.5f) / float(globalWidth) - 0.5f;
		const float imPlaneVPos = (y + 0.5f) / float(globalHeight) - 0.5f;

		const float3 pixelPos = globalEye + float(globalAspectRatio * globalFilmSize * imPlaneUPos) * uDir + float(globalFilmSize * imPlaneVPos) * vDir - globalDistanceToFilm * wDir;

		float2 apertureOffset = randomAperture(itr);
		float3 aperturePos = globalEye + apertureOffset.x * uDir + apertureOffset.y * vDir;

		return Ray(aperturePos, normalize(pixelPos - aperturePos));
	}

	void moveObjects(float timeDelta)
	{
		for(int i = 0; i<this->objects.size(); i++)
			this->objects[i]->incement_triangles(timeDelta);
		preCalc();
	}

	// ray tracing (you probably don't need to change it in A2)
	void Raytrace(){
		FrameBuffer.clear();
		// loop over all pixels in the image
		for(int k = 0; k < SAMPLES; ++k)
		{
			for (int j = 0; j < globalHeight; ++j)
			{
				for (int i = 0; i < globalWidth; ++i) {
					float3 pixel_value = float3(0.0f);
					for(int k = 0; k < NUM_RAYS; ++k)
					{
						const Ray ray = eyeRay(i, j,k);
						HitInfo hitInfo;
						if (intersect(hitInfo, ray)) {
							float fog_disperstion = beerLambert(fogAbsorbtion, hitInfo.t);
							float3 pointInFog = fog_disperstion * shade(hitInfo, -ray.d);
							pixel_value += lerp(backgroundColor, pointInFog, fog_disperstion);
						} else {
							pixel_value += (I.width != 0) ? get_from_image(ray.d) : backgroundColor;
						}
					}
					FrameBuffer.pixel(i, j) += pixel_value/NUM_RAYS;
				}

				// show intermediate process
				if (globalShowRaytraceProgress) {
					constexpr int scanlineNum = 64;
					if ((j % scanlineNum) == (scanlineNum - 1)) {
						glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, globalWidth, globalHeight, GL_RGB, GL_FLOAT, &FrameBuffer.pixels[0]);
						glRecti(1, 1, -1, -1);
						glfwSwapBuffers(globalGLFWindow);
						printf("Rendering Progress: %.3f%%\r", j / float(globalHeight - 1) * 100.0f);
						fflush(stdout);
					}
				}
			}

			if(k!=SAMPLES-1)
				moveObjects(timeDelta);
		}

		for (int j = 0; j < globalHeight; ++j)
			for (int i = 0; i < globalWidth; ++i)
				FrameBuffer.pixel(i, j)/= SAMPLES;
		std::cout<<"DONE\n";
	}

};

#endif //SCENE_H
