//
// Created by Rohan Vedula on 2024-07-14.
//

#ifndef MATERIAL_H
#define MATERIAL_H

enum enumMaterialType {
	MAT_LAMBERTIAN,
	MAT_METAL,
	MAT_GLASS
};

const int GRID_SIZE = 400;

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
		if (type == MAT_LAMBERTIAN) {
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
		// std::cout<<x<<" "<<y<<"\n";

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
