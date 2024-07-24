#include "cs488.h"
#include <string>
CS488Window CS488;

// setting up lighting
static PointLightSource light;
// static PointLightSource light2;
static void setupLightSource() {
    //light.position = float3(0.5f, 4.0f, 1.0f); // use this for sponza.obj
    light.position = float3(150.0f, 150.0f, 100.0f);
    //light.position = float3(0.0f, 5.0f, 3.0f);
    light.wattage = float3(1000000.0f, 1000000.0f, 1000000.0f);
    globalScene.addLight(&light);

    // light2.position = float3(3.0f, 3.0f, -10.0f);
    // light2.wattage = float3(1000.0f, 1000.0f, 1000.0f);
    // globalScene.addLight(&light2);
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
            CS488.load_image(argv[i+1]);
        else if(!strcmp(argv[i],"-apeture"))
            CS488.set_apeture(std::stof(argv[i+1]));
        else if(!strcmp(argv[i],"-ray_cast"))
            CS488.set_num_ray(atoi(argv[i+1]));
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

    CS488.start();
}
