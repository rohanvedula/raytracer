// =======================================
// CS488/688 base code
// (written by Toshiya Hachisuka)
// =======================================
#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX


// OpenGL
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>


// image loader and writer
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"


// linear algebra 
#include <complex>

#include "linalg.h"
using namespace linalg::aliases;


// animated GIF writer
#include "gif.h"


// misc
#include <iostream>
#include <vector>
#include <cfloat>

// .h files
#include "constants.h"
#include "image.h"
#include "material.h"
#include "perlin.h"
#include "scene.h"
#include "trianglemesh.h"
#include "structures.h"

// main window
GLFWwindow* globalGLFWindow;


// window size and resolution
// (do not make it too large - will be slow!)



// OpenGL related data (do not modify it if it is working)
static GLuint GLFrameBufferTexture;
static GLuint FSDraw;
static const std::string FSDrawSource = R"(
    #version 120

    uniform sampler2D input_tex;
    uniform vec4 BufInfo;

    void main()
    {
        gl_FragColor = texture2D(input_tex, gl_FragCoord.st * BufInfo.zw);
    }
)";
static const char* PFSDrawSource = FSDrawSource.c_str();


// main image buffer to be displayed

unsigned int sampleCount = 0;



// keyboard events (you do not need to modify it unless you want to)
void keyFunc(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		switch (key) {
			case GLFW_KEY_R: {
				if (globalRenderType == RENDER_RAYTRACE) {
					printf("(Switched to rasterization)\n");
					glfwSetWindowTitle(window, "Rasterization mode");
					globalRenderType = RENDER_RASTERIZE;
				} else if (globalRenderType == RENDER_RASTERIZE) {
					printf("(Switched to ray tracing)\n");
					AccumulationBuffer.clear();
					sampleCount = 0;
					glfwSetWindowTitle(window, "Ray tracing mode");
					globalRenderType = RENDER_RAYTRACE;
				}
			break;}

			case GLFW_KEY_ESCAPE: {
				glfwSetWindowShouldClose(window, GL_TRUE);
			break;}

			case GLFW_KEY_I: {
				char fileName[1024];
				sprintf(fileName, "output%d.png", int(1000.0 * PCG32::rand()));
				FrameBuffer.save(fileName);
			break;}

			case GLFW_KEY_F: {
				if (!globalRecording) {
					char fileName[1024];
					sprintf(fileName, "output%d.gif", int(1000.0 * PCG32::rand()));
					printf("Saving \"%s\"...\n", fileName);
					GifBegin(&globalGIFfile, fileName, globalWidth, globalHeight, globalGIFdelay);
					globalRecording = true;
					printf("(Recording started)\n");
				} else {
					GifEnd(&globalGIFfile);
					globalRecording = false;
					printf("(Recording done)\n");
				}
			break;}

			case GLFW_KEY_W: {
				globalEye += SCLFACT * globalViewDir;
				globalLookat += SCLFACT * globalViewDir;
			break;}

			case GLFW_KEY_S: {
				globalEye -= SCLFACT * globalViewDir;
				globalLookat -= SCLFACT * globalViewDir;
			break;}

			case GLFW_KEY_Q: {
				globalEye += SCLFACT * globalUp;
				globalLookat += SCLFACT * globalUp;
			break;}

			case GLFW_KEY_Z: {
				globalEye -= SCLFACT * globalUp;
				globalLookat -= SCLFACT * globalUp;
			break;}

			case GLFW_KEY_A: {
				globalEye -= SCLFACT * globalRight;
				globalLookat -= SCLFACT * globalRight;
			break;}

			case GLFW_KEY_D: {
				globalEye += SCLFACT * globalRight;
				globalLookat += SCLFACT * globalRight;
			break;}

			default: break;
		}
	}
}



// mouse button events (you do not need to modify it unless you want to)
void mouseButtonFunc(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			mouseLeftPressed = true;
		} else if (action == GLFW_RELEASE) {
			mouseLeftPressed = false;
			if (globalRenderType == RENDER_RAYTRACE) {
				AccumulationBuffer.clear();
				sampleCount = 0;
			}
		}
	}
}



// mouse button events (you do not need to modify it unless you want to)
void cursorPosFunc(GLFWwindow* window, double mouse_x, double mouse_y) {
	if (mouseLeftPressed) {
		const float xfact = -ANGFACT * float(mouse_y - m_mouseY);
		const float yfact = -ANGFACT * float(mouse_x - m_mouseX);
		float3 v = globalViewDir;

		// local function in C++...
		struct {
			float3 operator()(float theta, const float3& v, const float3& w) {
				const float c = cosf(theta);
				const float s = sinf(theta);

				const float3 v0 = dot(v, w) * w;
				const float3 v1 = v - v0;
				const float3 v2 = cross(w, v1);

				return v0 + c * v1 + s * v2;
			}
		} rotateVector;

		v = rotateVector(xfact * DegToRad, v, globalRight);
		v = rotateVector(yfact * DegToRad, v, globalUp);
		globalViewDir = v;
		globalLookat = globalEye + globalViewDir;
		globalRight = cross(globalViewDir, globalUp);

		m_mouseX = mouse_x;
		m_mouseY = mouse_y;

		if (globalRenderType == RENDER_RAYTRACE) {
			AccumulationBuffer.clear();
			sampleCount = 0;
		}
	} else {
		m_mouseX = mouse_x;
		m_mouseY = mouse_y;
	}
}



Image I;
static Scene globalScene;


static float3 reflection_shade(const HitInfo& hit, const float3& viewDir, const int level, const bool negate_normal = false)
{
	//flipping normal for refracted ray since we can be hitting the back-face of the triangle instead of front face
	HitInfo reflectedHit;
	float3 normal = hit.N;
	if(negate_normal)
		normal = -normal;

	// find ray direction using formula from notes
	float3 reflectedRayDir = normalize(2*dot(viewDir,normal)*normal - viewDir);
	Ray reflectedRay;
	reflectedRay.o = hit.P + Epsilon*normal;
	reflectedRay.d = reflectedRayDir;
	bool intersection = globalScene.intersect(reflectedHit, reflectedRay);

	// if no intersection, we know that it doesn't hit object so if we have a background image,
	// we load from background else return black
	if(!intersection)
	{
		if(I.width==0 || I.height==0)
			return float3(0.0f);
		return get_from_image(reflectedRayDir);
	}
	//recursive call, increasing the depth by 1 and multiplying by Ks, reflect ray direction to stanardize
	// since viewDir is pointing out of hitObject, reflectedRay should be negated
	return hit.material->Ks*shade(reflectedHit, -reflectedRayDir, level+1);
}

// Function to fade the input value

static float beerLambert(float absorb, float marchSize)
{
	return exp(-absorb * marchSize);
}

static float phase(float3 v, float3 l, float g) {
	float cosTheta = dot(v, l);
	float g2 = g * g;
	float denominator = pow(1.0 + g2 - 2.0 * g * cosTheta, 1.5);
	return (1.0 - g2) / (4.0 * PI * denominator);
}


static float3 cloud_shade(HitInfo& hit, const float3& viewDir, const int level)
{
	float absorptionCoefficient = hit.material->opacity;
	float opaqueVisiblity = 1.0f;
	const float marchSize = 1.1f;
	float volumeDepth = 0.0f;

	Ray newRay(hit.P - hit.N*Epsilon, -viewDir);
	HitInfo nextHit;
	globalScene.intersect(nextHit, newRay);

	float totalDepth = nextHit.t;
	float3 volumetricC = float3(0.0f);
	int itr = 0;

	for(int i = 0; i < maxMarchSteps; i++)
	{
		volumeDepth += marchSize;
		if(volumeDepth > totalDepth)
			break;

		float3 position = hit.P - hit.N*Epsilon - viewDir*volumeDepth;
		itr+=1;
		float p = opaqueVisiblity;
		opaqueVisiblity *= beerLambert(absorptionCoefficient, marchSize);
		float d = p - opaqueVisiblity;
		for (int i = 0; i < globalScene.pointLightSources.size(); i++)
		{
			float3 l = normalize(globalScene.pointLightSources[i]->position - position);
			float m = 1;// phase(viewDir, -l, -0.85);
			float3 lightC = hit.material->BRDF(l, viewDir, hit.N) * PI * m;

			Ray r(position, l);
			HitInfo tempHit;
			// find intersection between light ray
			bool haveHit = globalScene.intersect(tempHit, r);
			float distanceInCloud = tempHit.t;
			if(haveHit){
				lightC *= beerLambert(absorptionCoefficient, distanceInCloud);
			}
			volumetricC += d * lightC;
		}
	}
	volumetricC *= 1.2f;
	volumetricC = clamp(volumetricC, 0.0f, 1.0f);
	return backgroundColor * (1-volumetricC.x) + float3(1.0f) * volumetricC.x;
	//return backgroundColor * (opaqueVisiblity) + volumetricC * (1-opaqueVisiblity);
}

static float fresnel(HitInfo& hit, const float3& ray, const float3& normal) {
	float cosTheta = dot(ray, normal);
	float eta = hit.material->eta;

	float R0 = pow((1.0f - eta) / (1.0f + eta), 2.0f);
	// Use Schlick's approximation
	float R = R0 + (1.0f - R0) * pow(1.0f + cosTheta, 5.0f);

	return R;
}


static float inShadow(const HitInfo& hit, const float3& lightPosition)
{
	float3 l = lightPosition - hit.P;
	Ray r;
	r.o = lightPosition + 3*Epsilon*hit.N; //moving point by small amount to avoid self-intersection
	r.d = -normalize(l);
	HitInfo tempHit;
	float m = length(l);
	//find intersection between light ray
	globalScene.intersect(tempHit, r);

	//boolean for if there is a hit before our current hit (which is distance m away)
	bool is_hit = tempHit.t<m;
	return not is_hit;
}


static float getAverageShadow(const HitInfo& hit, PointLightSource& light)
{
	float total_light = 0.0f;
	int iterations = 1;

	for(int i = 0; i<iterations; i++)
	{
		total_light+=inShadow(hit, light.getRandomPoint(i));
	}
	return total_light/iterations;
}


float3 lerp(const float3& a, const float3& b, float t) {
	return a * (1.0f - t) + b * t;
}

static float3 shade(HitInfo& hit, const float3& viewDir, const int level) {
	if(level==6)
		return float3(0.0f, 0.0f, 0.0f);


	if (hit.material->hasBumpMap) {
		hit.N = mul(hit.pertubeMatrix, hit.material->fetchNormal(hit.T));;
	}

	if (hit.material->type == MAT_LAMBERTIAN) {
		// you may want to add shadow ray tracing here in A2
		float3 L = float3(0.0f);
		float3 brdf, irradiance;

		// loop over all of the point light sources
		for (int i = 0; i < globalScene.pointLightSources.size(); i++) {
			// calculating the ray between light position and object hit point
			float3 l = globalScene.pointLightSources[i]->position - hit.P;
			//return hit.material->BRDF(l, viewDir, hit.N) * PI;
			float shadow = getAverageShadow(hit, *globalScene.pointLightSources[i]);

			// the inverse-squared falloff
			const float falloff = length2(l);

			// normalize the light direction
			l /= sqrtf(falloff);

			// get the irradiance
			irradiance = float(std::max(0.0f, dot(hit.N, l)) / (4.0 * PI * falloff)) * globalScene.pointLightSources[i]->wattage;
			brdf = hit.material->BRDF(l, viewDir, hit.N);

			if (hit.material->isTextured) {
				brdf *= hit.material->fetchTexture(hit.T);
			}

			if (hit.material->perlinTexture) {
				brdf *= hit.material->fetchPerlinTexture(hit.T);
			}
			// return brdf * PI; //debug output
			float fog_disperstion = beerLambert(fogAbsorbtion, hit.t);

			//multiply irridecence which is 0 if there is no earlier hit and 1 otherwise
			float3 pointInFog = fog_disperstion*shadow*irradiance * brdf;
			L += lerp(backgroundColor, pointInFog, fog_disperstion);
		}
		return L;
	} else if (hit.material->type == MAT_METAL) {
		return reflection_shade(hit, viewDir, level);
	} else if (hit.material->type == MAT_GLASS) {
		// pre caclulate the eta ratio and incident ray, normal product
		float eta_ratio = 1/hit.material->eta;
		float vector_product = dot(-viewDir, hit.N);
		float3 normal = hit.N;

		// we know that if the vector product is >0 then we are hitting backface of triangle so reflect the normal
		if(vector_product>0)
		{
			eta_ratio = hit.material->eta;
			normal = -normal;
			vector_product = dot(-viewDir, normal);
		}

		//finding the interval term and if <0, we have total internal reflection so calling reflection function
		float internal_term = 1 - pow(eta_ratio,2)*(1-pow(vector_product,2));

		if(internal_term<0)
			return reflection_shade(hit, viewDir, level, dot(-viewDir, hit.N)>=0);

		//find refracted ray direction
		float3 wt = eta_ratio * (-viewDir - vector_product * normal) - sqrt(internal_term) * normal;
		wt = normalize(wt);

		Ray refractedRay;
		HitInfo refractedHit;
		refractedRay.o = hit.P - normal*Epsilon;
		refractedRay.d = wt;
		bool intersection = globalScene.intersect(refractedHit, refractedRay);

		// if no intersection, handle like above and return black or background image
		if(!intersection)
		{
			if(I.width==0 || I.height==0)
				return float3(0.0f, 0.0f, 0.0f);
			return get_from_image(wt);
		}

		//recusive call, increase depth
		float R = fresnel(hit, -viewDir, normal);
		return R*reflection_shade(hit, viewDir, level, dot(-viewDir, hit.N)>=0) + (1-R)*hit.material->Ks*shade(refractedHit, -refractedRay.d, level+1);
	} else if(hit.material->type == MAT_CLOUD)
	{
		return cloud_shade(hit, viewDir, level);
	} else {
		return float3(100.0f, 0.0f, 100.0f);
	}
}







// OpenGL initialization (you will not use any OpenGL/Vulkan/DirectX... APIs to render 3D objects!)
// you probably do not need to modify this in A0 to A3.
class OpenGLInit {
public:
	OpenGLInit() {
		// initialize GLFW
		if (!glfwInit()) {
			std::cerr << "Failed to initialize GLFW." << std::endl;
			exit(-1);
		}

		// create a window
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
		globalGLFWindow = glfwCreateWindow(globalWidth, globalHeight, "Welcome to CS488/688!", NULL, NULL);
		if (globalGLFWindow == NULL) {
			std::cerr << "Failed to open GLFW window." << std::endl;
			glfwTerminate();
			exit(-1);
		}

		// make OpenGL context for the window
		glfwMakeContextCurrent(globalGLFWindow);

		// initialize GLEW
		glewExperimental = true;
		if (glewInit() != GLEW_OK) {
			std::cerr << "Failed to initialize GLEW." << std::endl;
			glfwTerminate();
			exit(-1);
		}

		// set callback functions for events
		glfwSetKeyCallback(globalGLFWindow, keyFunc);
		glfwSetMouseButtonCallback(globalGLFWindow, mouseButtonFunc);
		glfwSetCursorPosCallback(globalGLFWindow, cursorPosFunc);

		// create shader
		FSDraw = glCreateProgram();
		GLuint s = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(s, 1, &PFSDrawSource, 0);
		glCompileShader(s);
		glAttachShader(FSDraw, s);
		glLinkProgram(FSDraw);

		// create texture
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &GLFrameBufferTexture);
		glBindTexture(GL_TEXTURE_2D, GLFrameBufferTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F_ARB, globalWidth, globalHeight, 0, GL_LUMINANCE, GL_FLOAT, 0);

		// initialize some OpenGL state (will not change)
		glDisable(GL_DEPTH_TEST);

		glUseProgram(FSDraw);
		glUniform1i(glGetUniformLocation(FSDraw, "input_tex"), 0);

		GLint dims[4];
		glGetIntegerv(GL_VIEWPORT, dims);
		const float BufInfo[4] = { float(dims[2]), float(dims[3]), 1.0f / float(dims[2]), 1.0f / float(dims[3]) };
		glUniform4fv(glGetUniformLocation(FSDraw, "BufInfo"), 1, BufInfo);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, GLFrameBufferTexture);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	virtual ~OpenGLInit() {
		glfwTerminate();
	}
};



// main window
// you probably do not need to modify this in A0 to A3.
class Runner {
public:
	// put this first to make sure that the glInit's constructor is called before the one for CS488Window
	OpenGLInit glInit;

	Runner()
	{
	}
	virtual ~Runner() {}

	void(*process)() = NULL;

	void load_image(const char* filepath)
	{
		I.load(filepath);
	}

	void set_apeture(const float s)
	{
		ApertureRadius = s;
	}

	void set_num_ray(const int n)
	{
		NUM_RAYS = n;
	}

	void set_fog(const float f)
	{
		fogAbsorbtion = f;
	}

	void start() const {
		globalScene.preCalc();

		// main loop
		while (glfwWindowShouldClose(globalGLFWindow) == GL_FALSE) {
			glfwPollEvents();
			globalViewDir = normalize(globalLookat - globalEye);
			globalRight = normalize(cross(globalViewDir, globalUp));

			if (globalRenderType == RENDER_RAYTRACE) {
				globalScene.Raytrace();
			} else if (globalRenderType == RENDER_IMAGE) {
				if (process) process();
			}

			if (globalRecording) {
				unsigned char* buf = new unsigned char[FrameBuffer.width * FrameBuffer.height * 4];
				int k = 0;
				for (int j = FrameBuffer.height - 1; j >= 0; j--) {
					for (int i = 0; i < FrameBuffer.width; i++) {
						buf[k++] = (unsigned char)(255.0f * Image::toneMapping(FrameBuffer.pixel(i, j).x));
						buf[k++] = (unsigned char)(255.0f * Image::toneMapping(FrameBuffer.pixel(i, j).y));
						buf[k++] = (unsigned char)(255.0f * Image::toneMapping(FrameBuffer.pixel(i, j).z));
						buf[k++] = 255;
					}
				}
				GifWriteFrame(&globalGIFfile, buf, globalWidth, globalHeight, globalGIFdelay);
				delete[] buf;
			}

			// drawing the frame buffer via OpenGL (you don't need to touch this)
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, globalWidth, globalHeight, GL_RGB, GL_FLOAT, &FrameBuffer.pixels[0][0]);
			glRecti(1, 1, -1, -1);
			glfwSwapBuffers(globalGLFWindow);
			globalFrameCount++;
			PCG32::rand();
		}
	}
};


