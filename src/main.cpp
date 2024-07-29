#include "controller.h"
#include <string>
Runner runner;

// setting up lighting
static PointLightSource light;
static void setupLightSource() {
    //cloud
    //light.position = float3(150.0f, 150.0f, 100.0f);
    //light.wattage = float3(1000000.0f, 1000000.0f, 1000000.0f);

    //multisphere
    // light.position = float3(0.0f, 300.0f, 100.0f);
    // light.wattage = float3(3000000.0f, 3000000.0f, 3000000.0f);

    //normal
    light.position = float3(3.0f, 3.0f, 3.0f);
    light.wattage = float3(1000.0f, 1000.0f, 1000.0f);
    globalScene.addLight(&light);
}



// ======== you probably don't need to modify below in A1 to A3 ========
// loading .obj file from the command line arguments
static TriangleMesh mesh;
static void setupScene(int argc, const char* argv[]) {
    if (argc > 1) {
        bool objLoadSucceed = mesh.load(argv[1]);
        if (!objLoadSucceed) {
            printf("Invalid .obj file.\n");
            printf("Making a single triangle instead.\n");
            mesh.createSingleTriangle();
        }
    } else {
        printf("Specify .obj file in the command line arguments. Example: CS488.exe cornellbox.obj\n");
        printf("Making a single triangle instead.\n");
        mesh.createSingleTriangle();
    }
    globalScene.addObject(&mesh);
}

static void parseCommandLine(int argc, const char* argv[])
{
    for(int i = 0; i<argc-1; i++)
    {
        if(!strcmp(argv[i],"-load_back"))
            runner.load_image(argv[i+1]);
        else if(!strcmp(argv[i],"-apeture"))
            runner.set_apeture(std::stof(argv[i+1]));
        else if(!strcmp(argv[i],"-ray_cast"))
            runner.set_num_ray(atoi(argv[i+1]));
        else if(!strcmp(argv[i],"-fog"))
            runner.set_fog(std::stof(argv[i+1]));
        else if(!strcmp(argv[i],"-samples"))
            runner.set_samples(atoi(argv[i+1]), std::stof(argv[i+2]));
    }
}


static void setup(int argc, const char* argv[]) {
    setupScene(argc, argv);
    setupLightSource();
    globalRenderType = RENDER_RAYTRACE;
}

int main(int argc, const char* argv[]) {
    setup(argc, argv);
    parseCommandLine(argc, argv);

    runner.start();
}
