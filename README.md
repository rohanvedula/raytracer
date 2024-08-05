# Raytracer
### Compilation
At command line, provide path to file(s) with the following optional arguments
- `-apeture` sets the aperture radius, next input is the radius of the aperture
- `-fog` sets the background fog absorption (default is 0), next input is absorption
- `-samples` used for motion blur, first argument is number of samples and next is time delta
- `-ray_cast` sets number of random samples of light source, next input is number of rays cast
- `-load_back` sets background image, next input is path to background image

The final scene loads 4 separate files into separate meshes, to run, put path to  all 4 files listed below: `texSpheres3.obj, finalClouds.obj, finalPlane.obj, finalSphere2.obj`
### Objectives
**Fresnel:**
To implement Fresnel reflection, we can create a function to approximate the fresnel reflection amount, `R`, using Schlick's approximation. Then, we can modify the refraction function to account for the reflected amount by calling shade again for the refracted ray. Notice the extra reflection on the second image (top of left marble we see the extra red square)
<div style="text-align: center;"><img src="/renders/prefresenl2.png" alt="drawing1" width="300" style="display: inline-block;"><img src="/renders/fersnel2.png" alt="drawing2" width="300" style="display: inline-block;"></div>

**Volumetric rendering:**
To implement volumetric rendering for clouds, we first need to know the absorption coefficient of the cloud. We can modify the `.mtl` file of the object and add a line called "opacity" which stores the coefficient of absorption. From there, we need to have a function for beer lambert law and the phase function of a cloud. The colour visible in a cloud in related to the distance we travel in the cloud and the coefficient of absorption as well. The more distance travelled, the more photons are absorbed and by proxy less of the background colour is visible. To implement this, we define a new material `MAT_CLOUD`, which sets the material of the object to be a cloud. Then, in our shading function, when we intersect with a cloud, we can use a ray marching approach to approximate the amount of light absorbed by the ray passing through the cloud. In areas where there is more of a material, then we will have more of the cloud colour present. We can configure the max amount of steps in our marching function to be 64 (standard amount for most ray marchers). We can find the exiting point of the ray in our ray march and find the amount of the ray that is in the cloud. If the ray march ever exceeds this distance, then we can exit the function. This is an approximation for finding if a point is inside the object or not. On top of this, we can account of the scattering of light through the cloud by approximating the value with a phase function which uses the angle in between the view direction and the ray from the hit point to the light source. In addition to this, we can add some randomness in the appearance of the cloud by integrating Perlin noise into our calculation of the beer-lambert's law. Lastly, we still need to account for the shadows present in the cloud, to do this, at each point P in our ray march, if we have a value inside the cloud, we can find the ray towards the light source and approximate the amount of light that arrives at P by using beer lambert's law for the amount of that ray which is in the cloud. The final consideration we have here is the fact that for any shadows that are casted by the clouds, the light may not be full obstructed. Therefore, in our shadow function, we can again use beer-lamberts law with the absorption of the cloud and the distance in the cloud to calculate the actual shadow at a point.
<div style="text-align: center;"><img src="/renders/cloudBasic.png" alt="drawing1" width="300" style="display: inline-block;"><img src="/renders/cloud2.png" alt="drawing2" width="300" style="display: inline-block;"></div>

**Bump mapping:**
A bump map is used to perturb the normal of an object. It's used similarly to a texture, the points are first defined and at each intersection of a ray and object with a normal map, we can call the get normal function to get the normal object and the normal perturbation from the map to calculate the new normal. In part of these process, we need to create a matrix to find a new normal. This matrix is precomputed for each triangle in an object so there is no need to create the same matrix repeatedly for multiple rays intersecting he same triangle.
<div style="text-align: center;"><img src="/renders/pretexture.png" alt="drawing1" width="300" style="display: inline-block;"><img src="/renders/textured.png" alt="drawing2" width="300" style="display: inline-block;"></div>

**Perlin noise:**
Perlin noise is a randomized noise following the Perlin algorithm for random noise, using this algorithm, we can compute textures for objects that have the property that points on a texture which are adjacent have similar noise values. In the `perlin.h` file, we have an implementation of Perlin noise for 2D and 3D coordinates. Each of these have different use cases; the 3D version can be used for randomized densities while the 2D version can be used for depth map for specific textures. Multiple octaves of Perlin noise can stacked on top of each other using Fractal Brownian Motion for a more even pattern.
<div style="text-align: center;"><img src="/renders/cloudPerlin.png" alt="drawing1" width="300" style="display: inline-block;"><img src="/renders/perlinPlane.png" alt="drawing2" width="300" style="display: inline-block;"></div>

**Depth of field blur:**
To implement depth of field blur, we consider a circular aperture and randomly sample a series of points along the aperture. The first point will always be through the centre of the aperture and the remaining will be random points. We can then draw a ray from the virtual pixel screen to the point on the aperture. We then average the value received from calling the shade function along all of the rays.
<div style="text-align: center;"><img src="/renders/noblur.png" alt="drawing1" width="300" style="display: inline-block;"><img src="/renders/blur.png" alt="drawing2" width="300" style="display: inline-block;"></div>

**Fog effects:**
Using the beer-lambert law, we can implement fog absorption based on the distance until the first object hit when running our ray tracer. Each time we call the shade function, we store the distance until the hit object in `HitInfo`. From there, we use beer-lambert's law to find the absorption and linearly interpolate between the value of the fog colour and the colour received from calling shade.
<div style="text-align: center;"><img src="/renders/nofog.png" alt="drawing1" width="300" style="display: inline-block;"><img src="/renders/fog.png" alt="drawing2" width="300" style="display: inline-block;"></div>

**Motion blur:**
To implement motion blur, we set a time delta for each time iteration and we take a series of N samples. At each time, we sample the value for each pixel, then move each object which has a velocity and then take a sample again. After averaging the value of each pixel for each N sample, we get the final value with motion blur.
<div style="text-align: center;"><img src="/renders/nomotion2.png" alt="drawing1" width="300" style="display: inline-block;"><img src="/renders/motion2.png" alt="drawing2" width="300" style="display: inline-block;"></div>

**Extra: soft shadows:**
As an extra, I also implemented soft shadows for objects. For each point which intersects an object, we randomly sample points on the light source and draw a ray from the light source to our hit point. If the ray is obstructed, then we set the value to 0 otherwise we set it to 1. We then average the values of each of the rays to find how much of the point is in a shadow.
<div style="text-align: center;"><img src="/renders/nosoft.png" alt="drawing1" width="300" style="display: inline-block;"><img src="/renders/soft.png" alt="drawing2" width="300" style="display: inline-block;"></div>

### Final Scene
The final scene I chose to render is a series of marbles on a flat metal distorted metal plane with clouds in the sky and a background image. The Fresnel reflection is present in the marble in the foreground, we can see the volumetric rendering with the clouds, bump mapping is used for the distortion on the metal plane (notice the reflections are distorted) and the marble on the right, perlin noise is implemented as a height map for the plane and in the clouds, depth of field is present as the marble in the background is more blurred, and motion blur is present in leftmost marble.

<img src="/renders/finalScene.png" alt="drawing" width="500" style="display: block; margin-left: auto; margin-right: auto;">

### Sources
Information references, code samples used for the Perlin noise, and external models are from the following sources:


J. Bender, M. Müller, M. Macklin. "Position-Based Simulation Methods in Computer Graphics." _TU Darmstadt, and NVIDIA PhysX Research_, EUROGRAPHICS 2015

J. Blinn, "A Generalization of Algebraic Surface Drawing", _ACM Transactions on Graphics_, Vol. 1, No. 3, pp. 235-256, July, 1982.

B.A. Barsky, T.J. Kosloff. "Algorithms for Rendering Depth of Field Effects in Computer Graphics." _University of California Berkeley_, 2015.

T. Akenine-Möller, J. Nilsson, M. Andersson, C. Barré-Brisebois, R. Toth, T. Karras. "Texture Level of Detail Strategies for Real-Time Ray Tracing." _NVIDIA, Electronic Arts_, Apress 2019.

M. Ikits, J. Kniss, A. Lefohn, C. Hansen. "Volume Rendering Techniques." _PU Gems_, Vol 1. pp. 290-196 

A. Tatarinov. "Perlin Noise in Real-time Computer Graphics." _Moscow State University_

K Perlin. 1985. An image synthesizer. In Proceedings of the 12th annual conference on Computer graphics and interactive techniques (SIGGRAPH '85). ACM, New York, NY, USA, 287-296.

S Hillaire. 2016. "Physically Based Sku, Atmosphere and Cloud Rendering", _EA Frostbite_

A. Bouthors et Al. February 2008. "Interactive multiple anisotropic scattering in clouds". _ACM Digital Library_

M. McGuire. July 2017. "Computer Graphics Archive", https://casual-effects.com/data/

S. Gustavson. n.d. "Worley Noise Implementations" https://stegu.github.io/webgl-noise/webdemo/cellular.html

P. Shirley. March 2020. "Schlick's Approximation" https://psgraphics.blogspot.com/2020/03/fresnel-equations-schlick-approximation.html

S. Majboroda. March 2024. "Autumn Field (Pure Sky) HDRI" https://polyhaven.com/a/autumn_field_puresky

Katsukagi. July 2024. "Stylized Tiles" https://3dtextures.me/2024/07/06/stylized-tiles-003/

