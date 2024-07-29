//
// Created by Rohan Vedula on 2024-07-14.
//

#ifndef MATERIAL_H
#define MATERIAL_H

#include "perlin.h"
#include "constants.h"

enum enumMaterialType {
	MAT_LAMBERTIAN,
	MAT_METAL,
	MAT_GLASS,
	MAT_CLOUD
};

class Material {
public:
	std::string name;

	enumMaterialType type = MAT_LAMBERTIAN;
	float eta = 1.0f;
	float glossiness = 1.0f;

	float3 Ka = float3(0.0f);
	float3 Kd = float3(0.9f);
	float3 Ks = float3(0.0f);
	float Ns = 0.0;
	float opacity = -1.0;

	// support 8-bit texture
	bool isTextured = false;
	unsigned char* texture = nullptr;
	int textureWidth = 0;
	int textureHeight = 0;

	bool hasBumpMap = false;
	unsigned char* bumpMap = nullptr;
	int bumpWidth = 0;
	int bumpHeight = 0;

	bool perlinTexture = false;
	bool hasVel = false;
	float3 velocity = float3(1.0f,0.0f,0.0f);

	Material() {};
	virtual ~Material() {};

	void setReflectance(const float3& c) {
		if (type == MAT_LAMBERTIAN) {
			Kd = c;
		} else if (type == MAT_METAL) {
			// empty
		} else if (type == MAT_GLASS) {
			// empty
		}
	}

	float3 fetchTexture(const float2& tex) const {
		// repeating
		int x = int(tex.x * textureWidth) % textureWidth;
		int y = int(tex.y * textureHeight) % textureHeight;
		if (x < 0) x += textureWidth;
		if (y < 0) y += textureHeight;

		int pix = (x + y * textureWidth) * 3;
		const unsigned char r = texture[pix + 0];
		const unsigned char g = texture[pix + 1];
		const unsigned char b = texture[pix + 2];
		return float3(r, g, b) / 255.0f;
	}

	float3 fetchNormal(const float2& nor) const {
		// repeating
		int x = int(nor.x * bumpWidth) % bumpWidth;
		int y = int(nor.y * bumpHeight) % bumpHeight;
		if (x < 0) x += bumpWidth;
		if (y < 0) y += bumpHeight;

		int pix = (x + y * bumpWidth) * 3;
		const unsigned char r = bumpMap[pix + 0];
		const unsigned char g = bumpMap[pix + 1];
		const unsigned char b = bumpMap[pix + 2];
		return normalize(float3(r, g, b)/127.5f+float3(1.0f));
	}

	float3 BRDF(const float3& wi, const float3& wo, const float3& n) const {
		float3 brdfValue = float3(0.0f);
		if (type == MAT_LAMBERTIAN || type == MAT_CLOUD) {
			// BRDF
			brdfValue = Kd / PI;
		} else if (type == MAT_METAL) {
			// empty
		} else if (type == MAT_GLASS) {
			// empty
		}
		return brdfValue;
	};

	float PDF(const float3& wGiven, const float3& wSample) const {
		// probability density function for a given direction and a given sample
		// it has to be consistent with the sampler
		float pdfValue = 0.0f;
		if (type == MAT_LAMBERTIAN) {
			// empty
		} else if (type == MAT_METAL) {
			// empty
		} else if (type == MAT_GLASS) {
			// empty
		}
		return pdfValue;
	}

	float3 sampler(const float3& wGiven, float& pdfValue) const {
		// sample a vector and record its probability density as pdfValue
		float3 smp = float3(0.0f);
		if (type == MAT_LAMBERTIAN) {
			// empty
		} else if (type == MAT_METAL) {
			// empty
		} else if (type == MAT_GLASS) {
			// empty
		}

		pdfValue = PDF(wGiven, smp);
		return smp;
	}

	float3 fetchPerlinTexture(const float2& coords) const
	{
		int x = int(coords.x * 512) % 512;
		int y = int(coords.y * 512) % 512;

		float val = 0;

		float freq = 1;
		float amp = 1;

		for (int i = 0; i < 12; i++)
		{
			val += perlin(x * freq / GRID_SIZE, y * freq / GRID_SIZE) * amp;

			freq *= 2;
			amp /= 2;

		}

		val *= 1.2;

		if (val > 1.0f)
			val = 1.0f;
		else if (val < -1.0f)
			val = -1.0f;

		// Convert 1 to -1 into 255 to 0
		return float3((val + 1.0f) * 0.5f);
	}

};

#endif //MATERIAL_H
