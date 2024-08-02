#include "controller.h"
#include <string>
Runner runner;

// setting up lighting
static PointLightSource light;
static void setupLightSource() {
    //cloud
    // light.position = float3(150.0f, 150.0f, 100.0f);
    // light.wattage = float3(1000000.0f, 1000000.0f, 1000000.0f);

    //multisphere
    light.position = float3(0.0f, 300.0f, 100.0f);
    light.wattage = float3(3000000.0f, 3000000.0f, 3000000.0f);

    //normal
    // light.position = float3(3.0f, 3.0f, 20.0f);
    // light.wattage = float3(10000.0f, 10000.0f, 10000.0f);

    // //new cornellbox
    // light.position = float3(0.0f, 0.5f, 0.0f);
    // light.wattage = float3(60.0f, 60.0f, 60.0f);

    //final
    // light.position = float3(0.0f, 300.0f, 100.0f);
    // light.wattage = float3(9000000.0f, 9000000.0f, 9000000.0f);
    globalScene.addLight(&light);
}



// ======== you probably don't need to modify below in A1 to A3 ========
// loading .obj file from the command line arguments
static std::vector<TriangleMesh *> meshes;
static void setupScene(int argc, const char* argv[]) {
    if (argc > 1) {
        int index = 1;
        while(index<argc)
        {
            if(argv[index][0]=='-')
                break;

            TriangleMesh* mesh = new TriangleMesh();
            bool objLoadSucceed = mesh->load(argv[index]);
            if (!objLoadSucceed) {
                printf("Invalid .obj file.\n");
                printf("Making a single triangle instead.\n");
                mesh->createSingleTriangle();
                break;
            }
            index+=1;
            meshes.push_back(mesh);
            globalScene.addObject(mesh);
        }
    } else {
        printf("Specify .obj file in the command line arguments. Example: CS488.exe cornellbox.obj\n");
        printf("Making a single triangle instead.\n");
    }
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

    for (auto mesh : meshes) {
        delete mesh;
    }
}
