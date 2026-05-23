#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <cmath>

#include "shader.h"
#include "camera.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Constants
unsigned int SCR_WIDTH = 1400;
unsigned int SCR_HEIGHT = 800;
const float PI = 3.14159265359f;

// ==================== CAMERA STATE ====================
glm::vec3 cameraPos = glm::vec3(0.0f, 20.0f, 50.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, -0.3f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);

float cameraPitch = -15.0f;
float cameraYaw = -90.0f;
float cameraRoll = 0.0f;

glm::vec3 lookAtPoint = glm::vec3(0.0f, 0.0f, 0.0f);
float orbitRadius = 50.0f;
float orbitAngle = 0.0f;
float orbitHeight = 25.0f;

const float CAMERA_SPEED = 15.0f;
const float ROTATION_SPEED = 50.0f;
const float ORBIT_SPEED = 30.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// ==================== SCENE STATE ====================
bool isNightMode = false;
bool orbitMode = false;
bool nKeyPressed = false;
bool fKeyPressed = false;
bool bKeyPressed = false;

// ==================== TEXTURE STATE ====================
int textureMode = 2;
unsigned int texFrontWall, texBrickWall, texRoof, texRoad, texGrass, texDefault, texFloorTiles;
unsigned int texSky, texRiver, texLeaf, texTreeBody, texFootball, texSun;
bool mKeyPressed = false;
bool oKeyPressed = false;
bool pKeyPressed = false;
bool vKeyPressed = false;
bool uKeyPressed = false;
bool isFourViewportMode = false;

// ==================== NEW SCHOOL STATE & CONSTANTS ====================
bool doorOpen = false;
bool windowsOpen = false;

// ===== GEOMETRY CONSTANTS =====
const float BLD_X_START = -10.0f, BLD_X_END = 10.0f;
const float BLD_Z_START = -15.0f, BLD_Z_END = 15.0f;
const float HALL_X_START = -2.0f, HALL_X_END = 2.0f;
const float FL_H = 3.6f;   // Total Floor-to-Floor height
const float WT = 0.2f;     // Slab and Wall thickness
const float IN_H = FL_H - WT; // Inner ceiling height = 3.4f
const float CORR_W = 3.0f;             // Corridor width (Z depth)
const float CORR_Z_FRONT = 3.0f;       // New exterior front wall
const float CORR_Z_BACK  = -0.1f;      // Corridor ends at old front wall
const float CORR_Z_CENTER = 1.45f;     // Corridor center Z
const float DOOR_W = 1.0f;             // Door width
const float DOOR_H = 2.6f;             // Door height

// ===== COLORS =====
glm::vec3 extWallCol = glm::vec3(0.6f, 0.6f, 0.65f); // solid exterior concrete (Grey)
glm::vec3 brickCol   = glm::vec3(0.85f, 0.40f, 0.30f); // red/orange brick
glm::vec3 intWallCol = glm::vec3(0.90f, 0.90f, 0.85f);
glm::vec3 hallFloor  = glm::vec3(0.80f, 0.82f, 0.85f);
glm::vec3 corrFloor  = glm::vec3(0.75f, 0.75f, 0.80f);  // Corridor tile color
glm::vec3 doorFrame  = glm::vec3(0.35f, 0.22f, 0.12f);  // Dark wood door frame
glm::vec3 classFloor = glm::vec3(0.65f, 0.50f, 0.35f);
glm::vec3 ceilCol    = glm::vec3(0.95f, 0.95f, 0.95f);
glm::vec3 glassCol   = glm::vec3(0.30f, 0.65f, 0.85f);
glm::vec3 doorCol    = glm::vec3(0.40f, 0.25f, 0.15f);
glm::vec3 deskWood   = glm::vec3(0.60f, 0.40f, 0.20f);
glm::vec3 deskMetal  = glm::vec3(0.40f, 0.40f, 0.40f);
glm::vec3 boardCol   = glm::vec3(0.95f, 0.95f, 0.95f);
glm::vec3 lockerCol  = glm::vec3(0.15f, 0.35f, 0.60f);
glm::vec3 pillarCol  = glm::vec3(1.0f, 1.0f, 1.0f);   // White pillars



// ==================== LIGHTING TOGGLES ====================
bool enableDirectionalLight = true;
bool enablePointLights = true;
bool enableSpotlight = false;
bool enableAmbient = true;
bool enableDiffuse = true;
bool enableSpecular = true;

// Key press tracking for lighting toggles
bool key1Pressed = false;
bool key2Pressed = false;
bool key3Pressed = false;
bool key5Pressed = false;
bool key6Pressed = false;
bool key7Pressed = false;

// ==================== SPOTLIGHT SETTINGS ====================
glm::vec3 spotlightPos = glm::vec3(0.0f, 4.8f, 12.0f);
glm::vec3 spotlightDir = glm::normalize(glm::vec3(0.0f, 4.0f, 0.0f) - glm::vec3(0.0f, 4.8f, 12.0f));
glm::vec3 spotlightColor = glm::vec3(1.0f, 1.0f, 0.9f);
float spotlightCutOff = glm::cos(glm::radians(10.0f));
float spotlightOuterCutOff = glm::cos(glm::radians(15.0f));
float spotlightIntensity = 4.0f;

float boatPosition = -15.0f;

// VAOs and vertex counts
unsigned int cubeVAO, coneVAO, cylinderVAO, trapezoidVAO, prismVAO, sphereVAO, footballSphereVAO, rowboatVAO;
int coneVertexCount, cylinderVertexCount, trapezoidVertexCount, prismVertexCount, sphereVertexCount, footballSphereVertexCount, rowboatVertexCount;

// ==================== POINT LIGHTS ====================
std::vector<glm::vec3> streetLampPositions = {
    glm::vec3(0.0f, 5.0f, 12.0f),
    glm::vec3(-20.0f, 5.0f, -10.0f),
    glm::vec3(20.0f, 5.0f, -10.0f),
    glm::vec3(-15.0f, 5.0f, 18.0f),
    glm::vec3(15.0f, 5.0f, 18.0f),
    glm::vec3(-30.0f, 5.0f, 0.0f),
    glm::vec3(30.0f, 5.0f, 0.0f),
    glm::vec3(0.0f, 5.0f, -20.0f),
};

// ==================== FUNCTION PROTOTYPES ====================
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void printControls();
void updateCameraVectors();
glm::mat4 calculateViewMatrix();
void setBirdsEyeView();
glm::mat4 myProjection(float fovY, float aspect, float nearPlane, float farPlane);
void setLightingUniforms(Shader& shader);

unsigned int createCube();
unsigned int createCone(int segments);
unsigned int createCylinder(int segments);
unsigned int createTrapezoid();
unsigned int createTriangularPrism();
unsigned int createSphere(int stacks, int slices, float uvScale = 1.0f);
unsigned int createRowboatMesh();

void drawObject(Shader& shader, unsigned int VAO, int vertexCount, glm::mat4 model, glm::vec3 color, float shine, bool emissive = false);
void drawCube(Shader& shader, glm::vec3 pos, glm::vec3 scale, glm::vec3 color, float shine, bool emissive = false);
void drawCone(Shader& shader, glm::vec3 pos, glm::vec3 scale, glm::vec3 color, float shine, bool emissive = false);
void drawCylinder(Shader& shader, glm::vec3 pos, glm::vec3 scale, glm::vec3 color, float shine, bool emissive = false);
void drawPrism(Shader& shader, glm::vec3 pos, glm::vec3 scale, glm::vec3 color, float shine, bool emissive = false);

void drawCylinderModel(Shader& shader, glm::mat4 model, glm::vec3 color, float shine, bool emissive = false);
void drawConeModel(Shader& shader, glm::mat4 model, glm::vec3 color, float shine, bool emissive = false);
void drawSphereModel(Shader& shader, glm::mat4 model, glm::vec3 color, float shine, bool emissive = false);
void drawFractalBranch(Shader& shader, glm::mat4 model, float length, float radius, int depth);

void drawGround(Shader& shader);
// void drawSchool(Shader& shader);

void drawRealisticSchool(Shader& lightingShader);
void drawLeftWing(Shader& lightingShader);
void drawCenterEntrance(Shader& lightingShader);
void drawRightWing(Shader& lightingShader);
void drawFlagpole(Shader& lightingShader);
void drawWindow(Shader& lightingShader, glm::vec3 pos, glm::vec3 scale, bool isBackWindow = false);
void drawClassroomInterior(Shader& lightingShader, float cX, float zCenter, float yBase);
void drawDesk(Shader& lightingShader, glm::vec3 pos, float rotY = 0.0f);
void drawCorridor(Shader& lightingShader);
void drawDoor(Shader& lightingShader, glm::vec3 pos, float doorWidth, float doorHeight, bool opens);
void drawCubeRotated(Shader& lightingShader, glm::vec3 color, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale);

void drawHouse(Shader& shader, glm::vec3 pos, glm::vec3 wallColor, glm::vec3 roofColor, float scale);
void drawWindmill(Shader& shader, glm::vec3 pos, float time);
void drawTree(Shader& shader, glm::vec3 pos, float scale);
void drawRiver(Shader& shader);
void drawBoat(Shader& shader, float xPos);
void drawMountains(Shader& shader);
void drawSun(Shader& shader);
void drawSky(Shader& shader);
void drawClouds(Shader& shader, float time);
void drawPath(Shader& shader);
void drawRocks(Shader& shader, glm::vec3 pos, float scale);
void drawGrassTuft(Shader& shader, glm::vec3 pos);
void drawStreetLamps(Shader& shader);
void drawSpotlightSource(Shader& shader);
void drawFootball(Shader& shader);
void drawSlide(Shader& shader, glm::vec3 base);
void drawPicnicSpot(Shader& shader, glm::vec3 base);
unsigned int loadTexture(const char* path);
unsigned int createDefaultTexture();
void drawSceneGeometry(Shader& shader, float currentFrame);

// ==================== CUSTOM PROJECTION MATRIX ====================
glm::mat4 myProjection(float fovY, float aspect, float nearPlane, float farPlane)
{
    float fovRad = glm::radians(fovY);
    float tanHalfFov = tan(fovRad / 2.0f);

    glm::mat4 projection = glm::mat4(0.0f);

    projection[0][0] = 1.0f / (aspect * tanHalfFov);
    projection[1][1] = 1.0f / tanHalfFov;
    projection[2][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
    projection[2][3] = -1.0f;
    projection[3][2] = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane);

    return projection;
}

// ==================== SET LIGHTING UNIFORMS ====================
void setLightingUniforms(Shader& shader)
{
    shader.setBool("enableDirectionalLight", enableDirectionalLight);
    shader.setBool("enablePointLights", enablePointLights);
    shader.setBool("enableSpotlight", enableSpotlight);
    shader.setBool("enableAmbient", enableAmbient);
    shader.setBool("enableDiffuse", enableDiffuse);
    shader.setBool("enableSpecular", enableSpecular);

    if (isNightMode) {
        shader.setVec3("sunDirection", glm::vec3(0.2f, -0.8f, 0.3f));
        shader.setVec3("sunAmbient", glm::vec3(0.05f, 0.05f, 0.1f));
        shader.setVec3("sunDiffuse", glm::vec3(0.1f, 0.1f, 0.2f));
        shader.setVec3("sunSpecular", glm::vec3(0.15f, 0.15f, 0.25f));
    }
    else {
        shader.setVec3("sunDirection", glm::vec3(-0.4f, -0.8f, -0.3f));
        shader.setVec3("sunAmbient", glm::vec3(0.35f, 0.35f, 0.32f));
        shader.setVec3("sunDiffuse", glm::vec3(0.95f, 0.95f, 0.9f));
        shader.setVec3("sunSpecular", glm::vec3(1.0f, 1.0f, 0.95f));
    }

    // Point lights - active based on toggle, visible effect mainly at night
    int numActiveLights = enablePointLights ? static_cast<int>(streetLampPositions.size()) : 0;
    shader.setInt("numPointLights", numActiveLights);

    for (size_t i = 0; i < streetLampPositions.size(); i++) {
        shader.setVec3("pointLightPos[" + std::to_string(i) + "]", streetLampPositions[i]);
        shader.setVec3("pointLightColor[" + std::to_string(i) + "]", glm::vec3(1.0f, 0.85f, 0.5f));
    }

    shader.setFloat("pointLightConstant", 1.0f);
    shader.setFloat("pointLightLinear", 0.09f);
    shader.setFloat("pointLightQuadratic", 0.032f);

    shader.setBool("spotlightActive", enableSpotlight);
    shader.setVec3("spotlightPos", spotlightPos);
    shader.setVec3("spotlightDir", spotlightDir);
    shader.setVec3("spotlightColor", spotlightColor);
    shader.setFloat("spotlightCutOff", spotlightCutOff);
    shader.setFloat("spotlightOuterCutOff", spotlightOuterCutOff);
    shader.setFloat("spotlightIntensity", spotlightIntensity);
}

// ==================== TEXTURE LOADING ====================
unsigned int loadTexture(const char* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = GL_RGB;
        if (nrChannels == 1) format = GL_RED;
        else if (nrChannels == 3) format = GL_RGB;
        else if (nrChannels == 4) format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        std::cout << ">> Loaded texture: " << path << " (" << width << "x" << height << ")" << std::endl;
    }
    else {
        std::cerr << "ERROR: Failed to load texture: " << path << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}

unsigned int createDefaultTexture()
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    unsigned char whitePixel[] = { 255, 255, 255 };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, whitePixel);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return textureID;
}

void drawSceneGeometry(Shader& shader, float currentFrame) {
    drawSky(shader);
    drawGround(shader);

    if (!isNightMode) {
        drawSun(shader);
        drawClouds(shader, currentFrame);
    }

    drawRealisticSchool(shader);
    drawFootball(shader);
    drawSlide(shader, glm::vec3(-8.0f, 0.0f, 20.0f));
    drawPicnicSpot(shader, glm::vec3(4.0f, 0.0f, 20.0f));

    // Houses - Behind school
    drawHouse(shader, glm::vec3(-5.0f, 0.0f, -25.0f), glm::vec3(0.9f, 0.9f, 0.85f), glm::vec3(0.7f, 0.2f, 0.2f), 0.9f);
    drawHouse(shader, glm::vec3(5.0f, 0.0f, -25.0f), glm::vec3(0.9f, 0.9f, 0.85f), glm::vec3(0.7f, 0.2f, 0.2f), 0.9f);
    drawHouse(shader, glm::vec3(0.0f, 0.0f, -18.0f), glm::vec3(0.95f, 0.85f, 0.7f), glm::vec3(0.9f, 0.6f, 0.2f), 1.0f);

    //// Houses - Sides
    drawHouse(shader, glm::vec3(-22.0f, 0.0f, 0.0f), glm::vec3(0.9f, 0.9f, 0.85f), glm::vec3(0.6f, 0.3f, 0.5f), 1.0f);
    drawHouse(shader, glm::vec3(-32.0f, 0.0f, 2.0f), glm::vec3(0.85f, 0.85f, 0.8f), glm::vec3(0.4f, 0.2f, 0.1f), 0.85f);
    drawHouse(shader, glm::vec3(22.0f, 0.0f, 0.0f), glm::vec3(0.9f, 0.9f, 0.85f), glm::vec3(0.7f, 0.2f, 0.2f), 1.0f);
    drawHouse(shader, glm::vec3(32.0f, 0.0f, -2.0f), glm::vec3(0.95f, 0.95f, 0.9f), glm::vec3(0.2f, 0.4f, 0.3f), 0.9f);

    //// Houses - Front
    drawHouse(shader, glm::vec3(-18.0f, 0.0f, 20.0f), glm::vec3(0.95f, 0.85f, 0.7f), glm::vec3(0.9f, 0.6f, 0.2f), 1.1f);
    drawHouse(shader, glm::vec3(-28.0f, 0.0f, 20.0f), glm::vec3(0.9f, 0.9f, 0.85f), glm::vec3(0.7f, 0.2f, 0.2f), 0.95f);
    drawHouse(shader, glm::vec3(18.0f, 0.0f, 20.0f), glm::vec3(0.9f, 0.9f, 0.85f), glm::vec3(0.6f, 0.3f, 0.5f), 1.1f);
    drawHouse(shader, glm::vec3(28.0f, 0.0f, 20.0f), glm::vec3(0.95f, 0.85f, 0.7f), glm::vec3(0.9f, 0.6f, 0.2f), 0.95f);

    //// Windmills
    drawWindmill(shader, glm::vec3(-35.0f, 0.0f, 10.0f), currentFrame);
    drawWindmill(shader, glm::vec3(38.0f, 0.0f, -30.0f), currentFrame * 0.8f);
    drawWindmill(shader, glm::vec3(-40.0f, 0.0f, -35.0f), currentFrame * 1.2f);

    //// Trees
    drawTree(shader, glm::vec3(-8.0f, 0.0f, -28.0f), 0.8f);
    drawTree(shader, glm::vec3(-3.0f, 0.0f, -30.0f), 1.1f);
    drawTree(shader, glm::vec3(-6.0f, 0.0f, -32.0f), 0.7f);
    drawTree(shader, glm::vec3(-35.0f, 0.0f, -5.0f), 1.2f);
    drawTree(shader, glm::vec3(-38.0f, 0.0f, 0.0f), 0.9f);
    drawTree(shader, glm::vec3(-36.0f, 0.0f, 5.0f), 1.0f);
    drawTree(shader, glm::vec3(15.0f, 0.0f, 25.0f), 0.85f);
    drawTree(shader, glm::vec3(20.0f, 0.0f, 28.0f), 1.2f);
    drawTree(shader, glm::vec3(-45.0f, 0.0f, -40.0f), 1.5f);
    drawTree(shader, glm::vec3(-48.0f, 0.0f, -35.0f), 0.9f);
    drawTree(shader, glm::vec3(-50.0f, 0.0f, -42.0f), 1.1f);
    drawTree(shader, glm::vec3(-42.0f, 0.0f, -45.0f), 0.75f);
    drawTree(shader, glm::vec3(45.0f, 0.0f, -35.0f), 1.4f);
    drawTree(shader, glm::vec3(48.0f, 0.0f, -38.0f), 1.0f);
    //drawTree(shader, glm::vec3(52.0f, 0.0f, -32.0f), 1.15f);
   /* drawTree(shader, glm::vec3(-42.0f, 0.0f, 38.0f), 1.1f);
    drawTree(shader, glm::vec3(38.0f, 0.0f, 42.0f), 0.95f);
    drawTree(shader, glm::vec3(15.0f, 0.0f, 45.0f), 1.3f);
    drawTree(shader, glm::vec3(22.0f, 0.0f, 5.0f), 0.8f);
    drawTree(shader, glm::vec3(-22.0f, 0.0f, 5.0f), 0.8f);*/

    drawPath(shader);
    drawRiver(shader);
    drawBoat(shader, 2.0f);

    drawRocks(shader, glm::vec3(-40.0f, 0.0f, 25.0f), 1.0f);
    drawRocks(shader, glm::vec3(35.0f, 0.0f, 30.0f), 0.7f);

    drawGrassTuft(shader, glm::vec3(-12.0f, 0.0f, 30.0f));
    drawGrassTuft(shader, glm::vec3(5.0f, 0.0f, 32.0f));

    drawStreetLamps(shader);

    if (enableSpotlight) {
        drawSpotlightSource(shader);
    }
}

// ==================== MAIN FUNCTION ====================
int main()
{
    printControls();

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Smart Village - Advanced Lighting", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    Shader shader("vertex_shader.vs", "fragment_shader.fs");

    cubeVAO = createCube();
    coneVAO = createCone(24);
    cylinderVAO = createCylinder(24);
    trapezoidVAO = createTrapezoid();
    prismVAO = createTriangularPrism();
    sphereVAO = createSphere(32, 64);
    footballSphereVAO = createSphere(32, 64, 3.0f);
    rowboatVAO = createRowboatMesh();

    // Load textures
    texFrontWall = loadTexture("window.png");
    texBrickWall = loadTexture("brick_wall.jpg");
    texFloorTiles = loadTexture("floor_tiles.jpg");
    texRoof = loadTexture("roof_tali.jpg");
    texRoad = loadTexture("road.jpg");
    texGrass = loadTexture("grass2.jpeg");
    texSky = loadTexture("sky.jpg");
    texRiver = loadTexture("river.jpg");
    texLeaf = loadTexture("leaf.png");
    texTreeBody = loadTexture("tree_body.png");
    texFootball = loadTexture("football2.jpg");
    texSun = loadTexture("sun.jpg");
    texDefault = createDefaultTexture();

    // Enable blending for alpha textures (leaves)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    updateCameraVectors();

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        float boatSpeed = 0.1f;
        float boatRange = 55.0f;
        boatPosition = sin(currentFrame * boatSpeed) * boatRange;

        if (orbitMode) {
            orbitAngle += ORBIT_SPEED * deltaTime;
            if (orbitAngle > 360.0f) orbitAngle -= 360.0f;
            cameraPos.x = lookAtPoint.x + orbitRadius * cos(glm::radians(orbitAngle));
            cameraPos.z = lookAtPoint.z + orbitRadius * sin(glm::radians(orbitAngle));
            cameraPos.y = orbitHeight;
            cameraFront = glm::normalize(lookAtPoint - cameraPos);
        }

        glm::vec3 skyColor = isNightMode ? glm::vec3(0.02f, 0.02f, 0.08f) : glm::vec3(0.5f, 0.75f, 0.95f);
        glClearColor(skyColor.r, skyColor.g, skyColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        if (!isFourViewportMode) {
            glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
            shader.use();
            glm::mat4 view = calculateViewMatrix();
            glm::mat4 projection = myProjection(45.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 500.0f);
            shader.setMat4("view", view);
            shader.setMat4("projection", projection);
            shader.setVec3("viewPos", cameraPos);
            shader.setBool("isNightMode", isNightMode);
            setLightingUniforms(shader);
            shader.setInt("ourTexture", 0);
            shader.setInt("textureMode", textureMode);
            shader.setBool("useTriplanar", false);
            shader.setFloat("triplanarScale", 0.5f);
            shader.setFloat("uvScale", 1.0f);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texDefault);

            drawSceneGeometry(shader, currentFrame);
        } else {
            int halfW = SCR_WIDTH / 2;
            int halfH = SCR_HEIGHT / 2;
            shader.use();

            for (int i = 0; i < 4; ++i) {
                glm::mat4 view;
                glm::mat4 projection = myProjection(45.0f, (float)halfW / (float)halfH, 0.1f, 500.0f);
                glm::vec3 currentPosStr;

                if (i == 0) {
                    // Top Left: Top View (Bird's Eye over school)
                    glViewport(0, halfH, halfW, halfH);
                    currentPosStr = glm::vec3(0.0f, 80.0f, 0.1f);
                    view = glm::lookAt(currentPosStr, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                } else if (i == 1) {
                    // Top Right: Free Camera
                    glViewport(halfW, halfH, halfW, halfH);
                    currentPosStr = cameraPos;
                    view = calculateViewMatrix();
                } else if (i == 2) {
                    // Bottom Left: Front View
                    glViewport(0, 0, halfW, halfH);
                    currentPosStr = glm::vec3(0.0f, 10.0f, 45.0f);
                    view = glm::lookAt(currentPosStr, glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                } else if (i == 3) {
                    // Bottom Right: Side View
                    glViewport(halfW, 0, halfW, halfH);
                    currentPosStr = glm::vec3(-45.0f, 10.0f, 0.0f);
                    view = glm::lookAt(currentPosStr, glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                }

                shader.setMat4("view", view);
                shader.setMat4("projection", projection);
                shader.setVec3("viewPos", currentPosStr);
                shader.setBool("isNightMode", isNightMode);
                setLightingUniforms(shader);
                shader.setInt("ourTexture", 0);
                shader.setInt("textureMode", textureMode);
                shader.setBool("useTriplanar", false);
                shader.setFloat("triplanarScale", 0.5f);
                shader.setFloat("uvScale", 1.0f);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texDefault);

                drawSceneGeometry(shader, currentFrame);
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &coneVAO);
    glDeleteVertexArrays(1, &cylinderVAO);
    glDeleteVertexArrays(1, &trapezoidVAO);
    glDeleteVertexArrays(1, &prismVAO);
    glDeleteVertexArrays(1, &sphereVAO);
    glDeleteVertexArrays(1, &footballSphereVAO);

    glDeleteTextures(1, &texFrontWall);
    glDeleteTextures(1, &texBrickWall);
    glDeleteTextures(1, &texRoof);
    glDeleteTextures(1, &texRoad);
    glDeleteTextures(1, &texGrass);
    glDeleteTextures(1, &texSky);
    glDeleteTextures(1, &texRiver);
    glDeleteTextures(1, &texLeaf);
    glDeleteTextures(1, &texTreeBody);
    glDeleteTextures(1, &texFootball);
    glDeleteTextures(1, &texSun);
    glDeleteTextures(1, &texDefault);

    glfwTerminate();
    return 0;
}

// ==================== CAMERA FUNCTIONS ====================

void updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
    front.y = sin(glm::radians(cameraPitch));
    front.z = sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
    cameraFront = glm::normalize(front);
    cameraRight = glm::normalize(glm::cross(cameraFront, glm::vec3(0.0f, 1.0f, 0.0f)));
    cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
}

glm::mat4 calculateViewMatrix() {
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    if (cameraRoll != 0.0f) {
        view = glm::rotate(view, glm::radians(cameraRoll), glm::vec3(0.0f, 0.0f, 1.0f));
    }
    return view;
}

void setBirdsEyeView() {
    cameraPos = glm::vec3(0.0f, 80.0f, 0.1f);
    cameraPitch = -89.0f;
    cameraYaw = -90.0f;
    cameraRoll = 0.0f;
    orbitMode = false;
    updateCameraVectors();
    std::cout << ">> Bird's Eye View Activated" << std::endl;
}

// ==================== INPUT HANDLING ====================

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float velocity = CAMERA_SPEED * deltaTime;
    float rotVelocity = ROTATION_SPEED * deltaTime;

    if (!orbitMode) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cameraPos += cameraFront * velocity;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cameraPos -= cameraFront * velocity;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cameraPos -= cameraRight * velocity;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPos += cameraRight * velocity;
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            cameraPos += glm::vec3(0.0f, 1.0f, 0.0f) * velocity;
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
            cameraPos -= glm::vec3(0.0f, 1.0f, 0.0f) * velocity;

        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
            float direction = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ? -1.0f : 1.0f;
            cameraPitch = glm::clamp(cameraPitch + rotVelocity * direction, -89.0f, 89.0f);
            updateCameraVectors();
        }
        if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
            float direction = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ? -1.0f : 1.0f;
            cameraYaw += rotVelocity * direction;
            updateCameraVectors();
        }
        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
            float direction = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ? -1.0f : 1.0f;
            cameraRoll = glm::clamp(cameraRoll + rotVelocity * direction, -45.0f, 45.0f);
        }
    }

    // Key 1: Toggle Directional Light
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !key1Pressed) {
        enableDirectionalLight = !enableDirectionalLight;
        key1Pressed = true;
        std::cout << ">> Directional Light: " << (enableDirectionalLight ? "ON" : "OFF") << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) key1Pressed = false;

    // Key 2: Toggle Point Lights
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !key2Pressed) {
        enablePointLights = !enablePointLights;
        key2Pressed = true;
        std::cout << ">> Point Lights: " << (enablePointLights ? "ON" : "OFF") << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE) key2Pressed = false;

    // Key 3: Toggle Spotlight
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && !key3Pressed) {
        enableSpotlight = !enableSpotlight;
        key3Pressed = true;
        std::cout << ">> Spotlight: " << (enableSpotlight ? "ON" : "OFF") << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE) key3Pressed = false;

    // Key 5: Toggle Ambient
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS && !key5Pressed) {
        enableAmbient = !enableAmbient;
        key5Pressed = true;
        std::cout << ">> Ambient Component: " << (enableAmbient ? "ON" : "OFF") << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_RELEASE) key5Pressed = false;

    // Key 6: Toggle Diffuse
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS && !key6Pressed) {
        enableDiffuse = !enableDiffuse;
        key6Pressed = true;
        std::cout << ">> Diffuse Component: " << (enableDiffuse ? "ON" : "OFF") << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_RELEASE) key6Pressed = false;

    // Key 7: Toggle Specular
    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS && !key7Pressed) {
        enableSpecular = !enableSpecular;
        key7Pressed = true;
        std::cout << ">> Specular Component: " << (enableSpecular ? "ON" : "OFF") << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_RELEASE) key7Pressed = false;

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !fKeyPressed) {
        orbitMode = !orbitMode;
        fKeyPressed = true;
        if (orbitMode) {
            orbitAngle = glm::degrees(atan2(cameraPos.z - lookAtPoint.z, cameraPos.x - lookAtPoint.x));
            orbitRadius = glm::max(20.0f, glm::length(glm::vec2(cameraPos.x - lookAtPoint.x, cameraPos.z - lookAtPoint.z)));
            orbitHeight = cameraPos.y;
            std::cout << ">> Orbit Mode ON" << std::endl;
        }
        else {
            updateCameraVectors();
            std::cout << ">> Orbit Mode OFF" << std::endl;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) fKeyPressed = false;

    // N - Mode 1: Simple Texture + Phong Lighting
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS && !nKeyPressed) {
        textureMode = 1;
        nKeyPressed = true;
        std::cout << ">> Mode 1: Simple Texture " << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_RELEASE) nKeyPressed = false;

    // M - Mode 2: Blended Gouraud (Vertex Shading)
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && !mKeyPressed) {
        textureMode = 2;
        mKeyPressed = true;
        std::cout << ">> Mode 2: Blended (Vertex/Gouraud Shading)" << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE) mKeyPressed = false;

    // O - Mode 3: Blended Phong (Fragment Shading)
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && !oKeyPressed) {
        textureMode = 3;
        oKeyPressed = true;
        std::cout << ">> Mode 3: Blended (Fragment/Phong Shading)" << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_RELEASE) oKeyPressed = false;

    
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && !pKeyPressed) {
        doorOpen = !doorOpen;
        pKeyPressed = true;
        std::cout << ">> Doors: " << (doorOpen ? "OPEN" : "CLOSED") << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE) pKeyPressed = false;

    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS && !uKeyPressed) {
        windowsOpen = !windowsOpen;
        uKeyPressed = true;
        std::cout << ">> Windows: " << (windowsOpen ? "OPEN" : "CLOSED") << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_RELEASE) uKeyPressed = false;

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !bKeyPressed) {
        setBirdsEyeView();
        bKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE) bKeyPressed = false;

    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS && !vKeyPressed) {
        isFourViewportMode = !isFourViewportMode;
        vKeyPressed = true;
        std::cout << ">> 4 Viewport Mode: " << (isFourViewportMode ? "ON" : "OFF") << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_RELEASE) vKeyPressed = false;

    if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS) {
        cameraPos = glm::vec3(0.0f, 20.0f, 50.0f);
        cameraPitch = -15.0f;
        cameraYaw = -90.0f;
        cameraRoll = 0.0f;
        orbitMode = false;
        updateCameraVectors();
        std::cout << ">> Camera Reset" << std::endl;
    }
}

// ==================== PRINT CONTROLS ====================

void printControls()
{
    std::cout << "\n========== 3D SMART VILLAGE - ADVANCED LIGHTING ==========\n\n";
    std::cout << "MOVEMENT (Flying Camera):\n";
    std::cout << "  W / S      - Move Forward / Backward\n";
    std::cout << "  A / D      - Strafe Left / Right\n";
    std::cout << "  E / R      - Move Up / Down\n\n";
    std::cout << "ROTATION (Hold SHIFT for reverse):\n";
    std::cout << "  X          - Pitch (Look Up/Down)\n";
    std::cout << "  Y          - Yaw (Look Left/Right)\n";
    std::cout << "  Z          - Roll (Tilt Camera)\n\n";
    std::cout << "CAMERA MODES:\n";
    std::cout << "  F          - Toggle Orbit Mode\n";
    // std::cout << "  B          - Bird's Eye View\n";
    std::cout << "  HOME       - Reset Camera\n\n";
    std::cout << "LIGHTING CONTROLS:\n";
    std::cout << "  1          - Toggle Directional Light (Sun/Moon)\n";
    std::cout << "  2          - Toggle Point Lights (Street Lamps)\n";
    std::cout << "  3          - Toggle Spotlight\n";
    std::cout << "  5          - Toggle Ambient Component\n";
    std::cout << "  6          - Toggle Diffuse Component\n";
    std::cout << "  7          - Toggle Specular Component\n\n";
    std::cout << "TEXTURE MODES:\n";
    std::cout << "  N           - Mode 1: Simple Texture + Lighting (Phong)\n";
    std::cout << "  M           - Mode 2: Blended (Vertex/Gouraud Shading)\n";
    std::cout << "  O           - Mode 3: Blended (Fragment/Phong Shading)\n\n";
    std::cout << "  V           - Toggle 4-Viewport Split Screen\n";
    std::cout << "  P           - Open/Close Doors\n";
    std::cout << "  U           - Open/Close Windows\n\n";
    std::cout << "  ESC        - Exit Application\n\n";
    std::cout << "============================================================\n";
    std::cout << ">> Starting in DAY MODE with all lights enabled...\n\n";
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

// ==================== DRAWING HELPER FUNCTIONS ====================

void drawObject(Shader& shader, unsigned int VAO, int vertexCount, glm::mat4 model, glm::vec3 color, float shine, bool emissive) {
    shader.setMat4("model", model);
    shader.setVec3("objectColor", color);
    shader.setFloat("shininess", shine);
    shader.setBool("isEmissive", emissive);
    if (emissive) {
        shader.setVec3("emissiveColor", color);
    }
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    if (emissive) {
        shader.setBool("isEmissive", false);
    }
}

void drawCube(Shader& shader, glm::vec3 pos, glm::vec3 scale, glm::vec3 color, float shine, bool emissive) {
    glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
    model = glm::scale(model, scale);
    drawObject(shader, cubeVAO, 36, model, color, shine, emissive);
}

void drawCone(Shader& shader, glm::vec3 pos, glm::vec3 scale, glm::vec3 color, float shine, bool emissive) {
    glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
    model = glm::scale(model, scale);
    drawObject(shader, coneVAO, coneVertexCount, model, color, shine, emissive);
}

void drawCylinder(Shader& shader, glm::vec3 pos, glm::vec3 scale, glm::vec3 color, float shine, bool emissive) {
    glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
    model = glm::scale(model, scale);
    drawObject(shader, cylinderVAO, cylinderVertexCount, model, color, shine, emissive);
}

void drawCylinderModel(Shader& shader, glm::mat4 model, glm::vec3 color, float shine, bool emissive) {
    drawObject(shader, cylinderVAO, cylinderVertexCount, model, color, shine, emissive);
}

void drawConeModel(Shader& shader, glm::mat4 model, glm::vec3 color, float shine, bool emissive) {
    drawObject(shader, coneVAO, coneVertexCount, model, color, shine, emissive);
}

void drawSphereModel(Shader& shader, glm::mat4 model, glm::vec3 color, float shine, bool emissive) {
    drawObject(shader, sphereVAO, sphereVertexCount, model, color, shine, emissive);
}

void drawFractalBranch(Shader& shader, glm::mat4 model, float length, float radius, int depth) {
    // Draw current branch
    glBindTexture(GL_TEXTURE_2D, texTreeBody);
    
    glm::mat4 branchModel = glm::translate(model, glm::vec3(0.0f, length / 2.0f, 0.0f));
    branchModel = glm::scale(branchModel, glm::vec3(radius, length, radius));
    
    // Very dark silhouette trunk to match the first image
    drawCylinderModel(shader, branchModel, glm::vec3(0.02f, 0.02f, 0.02f), 8.0f, false);
    
    if (depth == 0) {
        glBindTexture(GL_TEXTURE_2D, texLeaf);
        // Voluminous, cloud-like leaf mass using clustering spheres
        
        // Base bright emerald center
        glm::vec3 centerCol(0.15f, 0.85f, 0.25f);
        glm::mat4 centerLeaf = glm::translate(model, glm::vec3(0.0f, length * 1.1f, 0.0f));
        centerLeaf = glm::scale(centerLeaf, glm::vec3(radius * 18.0f, radius * 14.0f, radius * 18.0f));
        drawSphereModel(shader, centerLeaf, centerCol, 8.0f, false);
        
        // Outer mossy, pale yellow/green shifting edges
        for(int x = -1; x <= 1; x += 2) {
            for(int z = -1; z <= 1; z += 2) {
                glm::mat4 leafModel = glm::translate(model, glm::vec3(x * radius * 9.0f, length * 0.85f, z * radius * 9.0f));
                leafModel = glm::scale(leafModel, glm::vec3(radius * 14.0f, radius * 11.0f, radius * 14.0f));
                
                // Dynamically shift colors towards pale yellow-green for the outer edges
                glm::vec3 outerCol = glm::vec3(0.35f + abs(x)*0.15f, 0.95f, 0.15f + abs(z)*0.1f);
                drawSphereModel(shader, leafModel, outerCol, 8.0f, false);
            }
        }
        return;
    }
    
    glm::mat4 endModel = glm::translate(model, glm::vec3(0.0f, length, 0.0f));
    
    float newLength = length * 0.75f;
    float newRadius = radius * 0.65f;
    
    // Classic 2-branch Y shape
    // Alternating rotation locally via Y axis guarantees 3D completeness without losing the 2D structure.
    glm::mat4 m1 = glm::rotate(endModel, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    m1 = glm::rotate(m1, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    drawFractalBranch(shader, m1, newLength, newRadius, depth - 1);
    
    glm::mat4 m2 = glm::rotate(endModel, glm::radians(-30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    m2 = glm::rotate(m2, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    drawFractalBranch(shader, m2, newLength, newRadius, depth - 1);
}

void drawPrism(Shader& shader, glm::vec3 pos, glm::vec3 scale, glm::vec3 color, float shine, bool emissive) {
    glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
    model = glm::scale(model, scale);
    drawObject(shader, prismVAO, prismVertexCount, model, color, shine, emissive);
}

// ==================== SCENE DRAWING FUNCTIONS ====================

void drawGround(Shader& shader) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texGrass);
    shader.setFloat("uvScale", 40.0f);
    drawCube(shader, glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(120.0f, 1.0f, 120.0f), glm::vec3(0.35f, 0.65f, 0.25f), 8.0f);
    shader.setFloat("uvScale", 1.0f);
    glBindTexture(GL_TEXTURE_2D, texDefault);
}

void drawMountains(Shader& shader) {
    drawCone(shader, glm::vec3(-30.0f, 0.0f, -70.0f), glm::vec3(40.0f, 35.0f, 40.0f), glm::vec3(0.45f, 0.4f, 0.6f), 8.0f);
    drawCone(shader, glm::vec3(10.0f, 0.0f, -80.0f), glm::vec3(50.0f, 45.0f, 50.0f), glm::vec3(0.5f, 0.45f, 0.65f), 8.0f);
    drawCone(shader, glm::vec3(50.0f, 0.0f, -75.0f), glm::vec3(35.0f, 30.0f, 35.0f), glm::vec3(0.4f, 0.38f, 0.55f), 8.0f);
    drawCone(shader, glm::vec3(-60.0f, 0.0f, -85.0f), glm::vec3(45.0f, 38.0f, 45.0f), glm::vec3(0.48f, 0.42f, 0.62f), 8.0f);
}

void drawSky(Shader& shader) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texSky);
    glDepthMask(GL_FALSE);

    // Large sky sphere centered on camera - emissive, no lighting
    glm::mat4 model = glm::translate(glm::mat4(1.0f), cameraPos);
    model = glm::scale(model, glm::vec3(450.0f, 450.0f, 450.0f));
    drawObject(shader, sphereVAO, sphereVertexCount, model, glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, true);

    glDepthMask(GL_TRUE);
    glBindTexture(GL_TEXTURE_2D, texDefault);
}

void drawSun(Shader& shader) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texSun);
    // Sun positioned slightly lower
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(12.0f, 27.0f, -70.0f));
    model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
    drawObject(shader, sphereVAO, sphereVertexCount, model, glm::vec3(1.0f, 1.0f, 1.0f), 256.0f, true);
    glBindTexture(GL_TEXTURE_2D, texDefault);
}

void drawClouds(Shader& shader, float time) {
    // Cloud drift moving continuously from left to right
    float baseDrift = time * 8.0f;

    // Cloud 1
    float x1 = fmod(baseDrift, 240.0f) - 120.0f;
    glm::vec3 c1 = glm::vec3(x1, 25.0f, -65.0f);
    glm::mat4 model = glm::scale(glm::translate(glm::mat4(1.0f), c1), glm::vec3(9.0f, 4.5f, 4.5f));
    drawObject(shader, sphereVAO, sphereVertexCount, model, glm::vec3(0.98f, 0.98f, 1.0f), 8.0f);

    model = glm::scale(glm::translate(glm::mat4(1.0f), c1 + glm::vec3(6.0f, 0.5f, 0.0f)), glm::vec3(6.0f, 3.5f, 3.5f));
    drawObject(shader, sphereVAO, sphereVertexCount, model, glm::vec3(0.98f, 0.98f, 1.0f), 8.0f);

    model = glm::scale(glm::translate(glm::mat4(1.0f), c1 + glm::vec3(-5.0f, -0.5f, 0.0f)), glm::vec3(5.0f, 3.0f, 3.0f));
    drawObject(shader, sphereVAO, sphereVertexCount, model, glm::vec3(0.98f, 0.98f, 1.0f), 8.0f);

    // Cloud 2
    float x2 = fmod(baseDrift * 0.8f + 80.0f, 240.0f) - 120.0f;
    glm::vec3 c2 = glm::vec3(x2, 30.0f, -70.0f);
    model = glm::scale(glm::translate(glm::mat4(1.0f), c2), glm::vec3(10.0f, 5.0f, 5.0f));
    drawObject(shader, sphereVAO, sphereVertexCount, model, glm::vec3(0.98f, 0.98f, 1.0f), 8.0f);

    model = glm::scale(glm::translate(glm::mat4(1.0f), c2 + glm::vec3(7.0f, 0.0f, 0.0f)), glm::vec3(6.0f, 3.5f, 3.5f));
    drawObject(shader, sphereVAO, sphereVertexCount, model, glm::vec3(0.98f, 0.98f, 1.0f), 8.0f);

    // Cloud 3
    float x3 = fmod(baseDrift * 1.2f + 160.0f, 240.0f) - 120.0f;
    glm::vec3 c3 = glm::vec3(x3, 22.0f, -60.0f);
    model = glm::scale(glm::translate(glm::mat4(1.0f), c3), glm::vec3(8.0f, 4.0f, 4.0f));
    drawObject(shader, sphereVAO, sphereVertexCount, model, glm::vec3(0.98f, 0.98f, 1.0f), 8.0f);

    model = glm::scale(glm::translate(glm::mat4(1.0f), c3 + glm::vec3(-5.0f, 1.0f, 0.0f)), glm::vec3(5.0f, 3.0f, 3.0f));
    drawObject(shader, sphereVAO, sphereVertexCount, model, glm::vec3(0.98f, 0.98f, 1.0f), 8.0f);
}

void drawSchool(Shader& shader) {
    // Main building body - per-face texturing (front untextured, sides/back brick)
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 4.0f, 0.0f));
    model = glm::scale(model, glm::vec3(6.0f, 8.0f, 5.0f));
    shader.setMat4("model", model);
    shader.setVec3("objectColor", glm::vec3(1.0f, 0.9f, 0.3f));
    shader.setFloat("shininess", 16.0f);
    shader.setBool("isEmissive", false);
    glBindVertexArray(cubeVAO);
    glActiveTexture(GL_TEXTURE0);

    // Back face (-Z) with brick
    glBindTexture(GL_TEXTURE_2D, texBrickWall);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Front face (+Z) untextured
    glBindTexture(GL_TEXTURE_2D, texDefault);
    glDrawArrays(GL_TRIANGLES, 6, 6);

    // Left, Right walls with brick
    glBindTexture(GL_TEXTURE_2D, texBrickWall);
    glDrawArrays(GL_TRIANGLES, 12, 6);
    glDrawArrays(GL_TRIANGLES, 18, 6);

    // Top and Bottom with default
    glBindTexture(GL_TEXTURE_2D, texDefault);
    glDrawArrays(GL_TRIANGLES, 24, 12);

    // Roof
    glBindTexture(GL_TEXTURE_2D, texRoof);
    drawCube(shader, glm::vec3(0.0f, 8.2f, 0.0f), glm::vec3(6.5f, 0.5f, 5.5f), glm::vec3(0.9f, 0.1f, 0.1f), 32.0f);

    // Left wing
    glBindTexture(GL_TEXTURE_2D, texBrickWall);
    drawCube(shader, glm::vec3(-5.5f, 3.0f, 0.0f), glm::vec3(5.0f, 6.0f, 4.5f), glm::vec3(1.0f, 0.98f, 0.8f), 16.0f);
    glBindTexture(GL_TEXTURE_2D, texRoof);
    drawCube(shader, glm::vec3(-5.5f, 6.2f, 0.0f), glm::vec3(5.2f, 0.4f, 4.8f), glm::vec3(0.9f, 0.1f, 0.1f), 32.0f);

    // Right wing
    glBindTexture(GL_TEXTURE_2D, texBrickWall);
    drawCube(shader, glm::vec3(5.5f, 3.0f, 0.0f), glm::vec3(5.0f, 6.0f, 4.5f), glm::vec3(1.0f, 0.98f, 0.8f), 16.0f);
    glBindTexture(GL_TEXTURE_2D, texRoof);
    drawCube(shader, glm::vec3(5.5f, 6.2f, 0.0f), glm::vec3(5.2f, 0.4f, 4.8f), glm::vec3(0.9f, 0.1f, 0.1f), 32.0f);

    // Sign and door/windows with default
    glBindTexture(GL_TEXTURE_2D, texDefault);
    drawCube(shader, glm::vec3(0.0f, 7.0f, 2.6f), glm::vec3(4.0f, 1.2f, 0.1f), glm::vec3(0.9f, 0.1f, 0.1f), 32.0f);
    drawCube(shader, glm::vec3(0.0f, 1.5f, 2.6f), glm::vec3(2.0f, 3.0f, 0.2f), glm::vec3(0.4f, 0.7f, 0.9f), 64.0f);

    for (float y = 3.5f; y <= 5.5f; y += 2.0f) {
        drawCube(shader, glm::vec3(-1.5f, y, 2.6f), glm::vec3(1.0f, 1.2f, 0.1f), glm::vec3(0.4f, 0.6f, 0.9f), 128.0f);
        drawCube(shader, glm::vec3(1.5f, y, 2.6f), glm::vec3(1.0f, 1.2f, 0.1f), glm::vec3(0.4f, 0.6f, 0.9f), 128.0f);
    }
    for (float x : {-6.5f, -4.5f, 4.5f, 6.5f}) {
        for (float y : {2.5f, 4.5f}) {
            drawCube(shader, glm::vec3(x, y, 2.3f), glm::vec3(0.8f, 1.0f, 0.1f), glm::vec3(0.4f, 0.6f, 0.9f), 128.0f);
        }
    }

    drawCylinder(shader, glm::vec3(10.0f, 4.5f, 4.0f), glm::vec3(0.15f, 9.0f, 0.15f), glm::vec3(0.6f, 0.6f, 0.6f), 32.0f);
    drawCube(shader, glm::vec3(11.25f, 8.0f, 4.0f), glm::vec3(2.5f, 1.5f, 0.05f), glm::vec3(0.0f, 0.4f, 0.2f), 16.0f);
    // Red circle for Bangladesh flag (cylinder as disc, rotated to face forward)
    {
        glm::mat4 flagCircle = glm::translate(glm::mat4(1.0f), glm::vec3(11.1f, 8.0f, 4.06f));
        flagCircle = glm::rotate(flagCircle, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        flagCircle = glm::scale(flagCircle, glm::vec3(0.8f, 0.02f, 0.8f));
        drawObject(shader, cylinderVAO, cylinderVertexCount, flagCircle, glm::vec3(1.0f, 0.0f, 0.0f), 64.0f);
    }
}

void drawHouse(Shader& shader, glm::vec3 pos, glm::vec3 wallColor, glm::vec3 roofColor, float s) {
    // House body - draw individual faces with different textures
    glm::mat4 model = glm::translate(glm::mat4(1.0f), pos + glm::vec3(0.0f, 2.0f * s, 0.0f));
    model = glm::scale(model, glm::vec3(4.0f * s, 4.0f * s, 3.5f * s));
    shader.setMat4("model", model);
    shader.setVec3("objectColor", wallColor);
    shader.setFloat("shininess", 12.0f);
    shader.setBool("isEmissive", false);
    glBindVertexArray(cubeVAO);

    // Front face (+Z) with front_wall texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texFrontWall);
    glDrawArrays(GL_TRIANGLES, 6, 6);

    // Back, Left, Right walls with brick texture
    glBindTexture(GL_TEXTURE_2D, texBrickWall);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawArrays(GL_TRIANGLES, 12, 6);
    glDrawArrays(GL_TRIANGLES, 18, 6);

    // Top and Bottom with default
    glBindTexture(GL_TEXTURE_2D, texDefault);
    glDrawArrays(GL_TRIANGLES, 24, 12);

    // Roof with roof texture
    glBindTexture(GL_TEXTURE_2D, texRoof);
    drawPrism(shader, pos + glm::vec3(0.0f, 4.5f * s, 0.0f), glm::vec3(4.5f * s, 2.5f * s, 4.0f * s), roofColor, 16.0f);

    // Door and windows with default texture
    glBindTexture(GL_TEXTURE_2D, texDefault);
    drawCube(shader, pos + glm::vec3(0.0f, 1.2f * s, 1.8f * s), glm::vec3(0.9f * s, 2.2f * s, 0.15f * s), glm::vec3(0.3f, 0.18f, 0.1f), 8.0f);
    drawCube(shader, pos + glm::vec3(1.2f * s, 2.5f * s, 1.8f * s), glm::vec3(0.8f * s, 0.8f * s, 0.12f * s), glm::vec3(0.4f, 0.6f, 0.9f), 64.0f);
    drawCube(shader, pos + glm::vec3(-1.2f * s, 2.5f * s, 1.8f * s), glm::vec3(0.8f * s, 0.8f * s, 0.12f * s), glm::vec3(0.4f, 0.6f, 0.9f), 64.0f);
}

void drawWindmill(Shader& shader, glm::vec3 pos, float time) {
    drawCylinder(shader, pos + glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(1.5f, 10.0f, 1.5f), glm::vec3(0.75f, 0.7f, 0.65f), 16.0f);
    drawCone(shader, pos + glm::vec3(0.0f, 10.5f, 0.0f), glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.5f, 0.35f, 0.25f), 16.0f);

    float rotation = time * 45.0f;
    glm::mat4 bladeBase = glm::mat4(1.0f);
    bladeBase = glm::translate(bladeBase, pos + glm::vec3(0.0f, 9.0f, 0.85f));
    bladeBase = glm::rotate(bladeBase, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));

    for (int i = 0; i < 4; i++) {
        glm::mat4 model = bladeBase;
        model = glm::rotate(model, glm::radians(90.0f * i), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::translate(model, glm::vec3(0.0f, 2.5f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f, 5.0f, 0.12f));
        drawObject(shader, cubeVAO, 36, model, glm::vec3(0.85f, 0.8f, 0.7f), 16.0f);
    }

    glm::mat4 hub = glm::scale(bladeBase, glm::vec3(0.6f, 0.6f, 0.4f));
    drawObject(shader, cylinderVAO, cylinderVertexCount, hub, glm::vec3(0.4f, 0.35f, 0.3f), 32.0f);
}

void drawTree(Shader& shader, glm::vec3 pos, float s) {
    glActiveTexture(GL_TEXTURE0);

    glm::mat4 baseModel = glm::translate(glm::mat4(1.0f), pos);
    // Draw fractal tree with depth 6 to achieve the dense canopy look
    drawFractalBranch(shader, baseModel, 2.5f * s, 0.4f * s, 5);

    // Restore default texture
    glBindTexture(GL_TEXTURE_2D, texDefault);
}

void drawRiver(Shader& shader) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texRiver);
    drawCube(shader, glm::vec3(0.0f, -0.1f, -55.0f), glm::vec3(150.0f, 0.25f, 12.0f), glm::vec3(0.2f, 0.45f, 0.75f), 128.0f);
    glBindTexture(GL_TEXTURE_2D, texDefault);
    drawCube(shader, glm::vec3(0.0f, -0.05f, -48.5f), glm::vec3(152.0f, 0.12f, 2.0f), glm::vec3(0.4f, 0.3f, 0.18f), 8.0f);
    drawCube(shader, glm::vec3(0.0f, -0.05f, -61.5f), glm::vec3(152.0f, 0.12f, 2.0f), glm::vec3(0.4f, 0.3f, 0.18f), 8.0f);
}

void drawBoat(Shader& shader, float boatX) {
    // ==== OPEN WOODEN ROWBOAT ====
    glm::vec3 woodCol  = glm::vec3(0.55f, 0.40f, 0.25f);
    glm::vec3 darkWood = glm::vec3(0.40f, 0.25f, 0.15f);
    glm::vec3 poleMetal= glm::vec3(0.60f, 0.62f, 0.65f); // metal pole like image
    glm::vec3 ropeCol  = glm::vec3(0.85f, 0.75f, 0.55f); // light tan rope

    // River starts Z = -48.5 and goes to -61.5 (water). Bank is at Z > -48.5.
    float by  =  0.03f;  
    float bz  = -49.0f;  // Set origin precisely at the shoreline edge
    float tilt= 10.0f;   // Pitched up

    // We disable Face Culling specifically because the boat is an open hull mesh
    GLboolean cullingOn; glGetBooleanv(GL_CULL_FACE, &cullingOn);
    glDisable(GL_CULL_FACE);

    // Draw the continuous smooth hull custom mesh
    // We rotate 90 degrees around Y so the boat's length (X-axis) points TO the shore!
    // Positive X becomes deep water, Negative X becomes the bank if rotated -90 deg.
    // Let's rotate 90 deg so positive local X is pointing into the bank (Z positive).
    glm::mat4 bm = glm::translate(glm::mat4(1.0f), glm::vec3(boatX, by + 0.15f, bz));
    bm = glm::rotate(bm, glm::radians(90.0f), glm::vec3(0, 1, 0)); // face the shore!
    bm = glm::rotate(bm, glm::radians(tilt), glm::vec3(0, 0, -1));  // pitch up the bow
    bm = glm::scale(bm, glm::vec3(1.1f)); 
    
    drawObject(shader, rowboatVAO, rowboatVertexCount, bm, woodCol, 16.0f);
    
    if (cullingOn) glEnable(GL_CULL_FACE);

    // Two Oars resting inside roughly
    float oarL = 3.6f;
    float oarY = 0.50f; 
    
    // Oar 1 - Sticking out visibly over the edge!
    // We rotate it so the handle is inside the boat and the blade sticks out.
    glm::vec3 oar1Pos(bm * glm::vec4(0.0f, oarY + 0.1f, -1.0f, 1.0f)); 
    glm::vec3 oar1Blade(bm * glm::vec4(-oarL*0.45f * 0.707f, oarY + 0.1f + oarL*0.45f*0.2f, -1.0f - oarL*0.45f*0.707f, 1.0f));
    //drawCubeRotated(shader, woodCol, oar1Pos, glm::vec3(-20, -50 + -90, tilt+10), glm::vec3(oarL, 0.04f, 0.04f));
   // drawCubeRotated(shader, darkWood, oar1Blade, glm::vec3(-20, -50 + -90, tilt+10), glm::vec3(0.6f, 0.02f, 0.16f)); 

    // Oar 2 - Resting inside
    glm::vec3 oar2Pos(bm * glm::vec4(0.1f, oarY + 0.02f, 0.35f, 1.0f));
    glm::vec3 oar2Blade(bm * glm::vec4(0.1f - oarL*0.45f, oarY + 0.02f, 0.25f, 1.0f));
    //drawCubeRotated(shader, woodCol, oar2Pos, glm::vec3(0, -90, tilt+8), glm::vec3(oarL, 0.04f, 0.04f));
    //drawCubeRotated(shader, darkWood, oar2Blade, glm::vec3(0, -90, tilt+8), glm::vec3(0.6f, 0.02f, 0.16f)); 

    // Mooring Pole firmly on the grass bank 
    float poleX = boatX + 1.2f;   // offset slightly
    float poleZ = -46.0f; // well on land
    float poleH = 1.4f;
    drawCylinder(shader,
        glm::vec3(poleX, poleH * 0.5f, poleZ),
        glm::vec3(0.18f, poleH, 0.18f), poleMetal, 16.0f);
    drawObject(shader, sphereVAO, sphereVertexCount,
        glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(poleX, poleH, poleZ)), glm::vec3(0.20f)),
        poleMetal, 16.0f);
    drawCylinder(shader,
        glm::vec3(poleX, poleH * 0.7f, poleZ),
        glm::vec3(0.22f, 0.10f, 0.22f), ropeCol, 16.0f);

    // Rope connecting bow (shore end) to pole
    // The shore end is local X = -2.4 (since length is 4.8 and -X points to +Z shore)
    glm::vec3 bowTop = glm::vec3(bm * glm::vec4(-2.3f, 0.2f, 0.0f, 1.0f)); 
    glm::vec3 poleTie(poleX, poleH * 0.7f, poleZ);
    glm::vec3 rMid = (bowTop + poleTie) * 0.5f;
    glm::vec3 rDip = rMid + glm::vec3(0, -0.6f, 0); // Sagging completely differently
    
    float s1Len = glm::length(rDip - bowTop);
    glm::vec3 d1 = glm::normalize(rDip - bowTop);
   // drawCubeRotated(shader, ropeCol, (bowTop + rDip) * 0.5f,
       // glm::vec3(glm::degrees(asinf(-d1.y)), glm::degrees(atan2f(d1.x, d1.z)), 0),
        //glm::vec3(0.06f, 0.06f, s1Len));
    
    float s2Len = glm::length(poleTie - rDip);
    glm::vec3 d2 = glm::normalize(poleTie - rDip);
    //drawCubeRotated(shader, ropeCol, (rDip + poleTie) * 0.5f,
        //glm::vec3(glm::degrees(asinf(-d2.y)), glm::degrees(atan2f(d2.x, d2.z)), 0),
  //      glm::vec3(0.06f, 0.06f, s2Len));
}


void drawPath(Shader& shader) {
    glm::vec3 pathColor = glm::vec3(0.5f, 0.35f, 0.2f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texRoad);
    drawCube(shader, glm::vec3(0.0f, 0.02f, 10.0f), glm::vec3(100.0f, 0.08f, 4.0f), pathColor, 8.0f);
    drawCube(shader, glm::vec3(0.0f, 0.02f, 5.0f), glm::vec3(4.0f, 0.08f, 10.0f), pathColor, 8.0f);
    drawCube(shader, glm::vec3(0.0f, 0.02f, -12.5f), glm::vec3(2.5f, 0.08f, 25.0f), pathColor, 8.0f);
    drawCube(shader, glm::vec3(0.0f, 0.02f, -25.0f), glm::vec3(12.0f, 0.08f, 2.0f), pathColor, 8.0f);
    drawCube(shader, glm::vec3(-20.0f, 0.02f, 0.0f), glm::vec3(30.0f, 0.08f, 2.0f), pathColor, 8.0f);
    drawCube(shader, glm::vec3(20.0f, 0.02f, 0.0f), glm::vec3(30.0f, 0.08f, 2.0f), pathColor, 8.0f);
    drawCube(shader, glm::vec3(-23.0f, 0.02f, 15.0f), glm::vec3(12.0f, 0.08f, 10.0f), pathColor, 8.0f);
    drawCube(shader, glm::vec3(23.0f, 0.02f, 15.0f), glm::vec3(12.0f, 0.08f, 10.0f), pathColor, 8.0f);
    glBindTexture(GL_TEXTURE_2D, texDefault);
}

void drawRocks(Shader& shader, glm::vec3 pos, float scale) {
    drawCube(shader, pos + glm::vec3(0.0f, 0.4f * scale, 0.0f), glm::vec3(1.5f * scale, 0.8f * scale, 1.2f * scale), glm::vec3(0.5f, 0.5f, 0.52f), 16.0f);
    drawCube(shader, pos + glm::vec3(1.0f * scale, 0.25f * scale, 0.5f * scale), glm::vec3(0.8f * scale, 0.5f * scale, 0.7f * scale), glm::vec3(0.55f, 0.53f, 0.55f), 16.0f);
    drawCube(shader, pos + glm::vec3(-0.8f * scale, 0.2f * scale, -0.3f * scale), glm::vec3(0.6f * scale, 0.4f * scale, 0.5f * scale), glm::vec3(0.48f, 0.48f, 0.5f), 16.0f);
}

void drawGrassTuft(Shader& shader, glm::vec3 pos) {
    drawCone(shader, pos + glm::vec3(0.0f, 0.4f, 0.0f), glm::vec3(0.3f, 0.8f, 0.3f), glm::vec3(0.3f, 0.55f, 0.2f), 8.0f);
    drawCone(shader, pos + glm::vec3(0.25f, 0.35f, 0.1f), glm::vec3(0.25f, 0.7f, 0.25f), glm::vec3(0.28f, 0.52f, 0.18f), 8.0f);
    drawCone(shader, pos + glm::vec3(-0.2f, 0.38f, 0.15f), glm::vec3(0.28f, 0.75f, 0.28f), glm::vec3(0.32f, 0.58f, 0.22f), 8.0f);
    drawCone(shader, pos + glm::vec3(0.1f, 0.32f, -0.2f), glm::vec3(0.22f, 0.65f, 0.22f), glm::vec3(0.25f, 0.5f, 0.18f), 8.0f);
}

void drawStreetLamps(Shader& shader) {
    for (const auto& pos : streetLampPositions) {
        drawCylinder(shader, glm::vec3(pos.x, 2.5f, pos.z), glm::vec3(0.15f, 5.0f, 0.15f), glm::vec3(0.25f, 0.25f, 0.28f), 32.0f);
        drawCube(shader, glm::vec3(pos.x, pos.y - 0.3f, pos.z), glm::vec3(0.6f, 0.2f, 0.6f), glm::vec3(0.2f, 0.2f, 0.22f), 16.0f);
        glm::vec3 bulbColor = isNightMode ? glm::vec3(1.0f, 0.95f, 0.7f) : glm::vec3(0.8f, 0.8f, 0.75f);
        bool bulbEmissive = isNightMode && enablePointLights;
        drawCube(shader, pos, glm::vec3(0.5f, 0.5f, 0.5f), bulbColor, 256.0f, bulbEmissive);
    }
}

void drawSpotlightSource(Shader& shader) {
    glm::mat4 model = glm::translate(glm::mat4(1.0f), spotlightPos);
    model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
    drawObject(shader, sphereVAO, sphereVertexCount, model, glm::vec3(1.0f, 1.0f, 0.8f), 256.0f, true);
    drawCube(shader, spotlightPos + glm::vec3(0.0f, 0.4f, 0.0f), glm::vec3(0.8f, 0.3f, 0.8f), glm::vec3(0.2f, 0.2f, 0.25f), 32.0f);
}

void drawFootball(Shader& shader) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texFootball);
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(25.0f, 1.0f, 14.0f));
    model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
    //drawObject(shader, footballSphereVAO, footballSphereVertexCount, model, glm::vec3(1.0f, 1.0f, 1.0f), 4.0f);
    glBindTexture(GL_TEXTURE_2D, texDefault);
}

// ==================== GEOMETRY CREATION ====================

unsigned int createCube() {
    // 11 floats per vertex: pos(3) + color(3) + normal(3) + texcoord(2) = stride 44
    float vertices[] = {
        // Back face (-Z)
        -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        // Front face (+Z)
        -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
        // Left face (-X)
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        // Right face (+X)
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         // Bottom face (-Y)
         -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
          0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
          0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
          0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
         // Top face (+Y)
         -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
          0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
          0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
          0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
         -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // Position (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Color (location 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Normal (location 2)
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // TexCoord (location 3)
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glBindVertexArray(0);
    return VAO;
}

unsigned int createCone(int segments) {
    std::vector<float> vertices;
    float radius = 0.5f;
    float height = 1.0f;

    for (int i = 0; i < segments; i++) {
        float angle1 = 2.0f * PI * i / segments;
        float angle2 = 2.0f * PI * (i + 1) / segments;
        float x1 = radius * cos(angle1), z1 = radius * sin(angle1);
        float x2 = radius * cos(angle2), z2 = radius * sin(angle2);
        float nx1 = cos(angle1), nz1 = sin(angle1);
        float nx2 = cos(angle2), nz2 = sin(angle2);
        float ny = radius / height;

        float u1 = (float)i / segments;
        float u2 = (float)(i + 1) / segments;

        // Side triangle: pos(3) + color(3) + normal(3) + texcoord(2)
        vertices.insert(vertices.end(), { 0.0f, height / 2, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, (u1 + u2) * 0.5f, 1.0f });
        vertices.insert(vertices.end(), { x1, -height / 2, z1, 1.0f, 1.0f, 1.0f, nx1, ny, nz1, u1, 0.0f });
        vertices.insert(vertices.end(), { x2, -height / 2, z2, 1.0f, 1.0f, 1.0f, nx2, ny, nz2, u2, 0.0f });
        // Bottom cap
        vertices.insert(vertices.end(), { 0.0f, -height / 2, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.5f, 0.5f });
        vertices.insert(vertices.end(), { x2, -height / 2, z2, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, u2, 0.0f });
        vertices.insert(vertices.end(), { x1, -height / 2, z1, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, u1, 0.0f });
    }

    coneVertexCount = static_cast<int>(vertices.size() / 11);

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glBindVertexArray(0);
    return VAO;
}

unsigned int createCylinder(int segments) {
    std::vector<float> vertices;
    float radius = 0.5f;
    float height = 1.0f;

    for (int i = 0; i < segments; i++) {
        float angle1 = 2.0f * PI * i / segments;
        float angle2 = 2.0f * PI * (i + 1) / segments;
        float x1 = radius * cos(angle1), z1 = radius * sin(angle1);
        float x2 = radius * cos(angle2), z2 = radius * sin(angle2);
        float nx1 = cos(angle1), nz1 = sin(angle1);
        float nx2 = cos(angle2), nz2 = sin(angle2);

        float u1 = (float)i / segments;
        float u2 = (float)(i + 1) / segments;

        // Side quad (2 triangles): pos(3) + color(3) + normal(3) + texcoord(2)
        vertices.insert(vertices.end(), { x1, -height / 2, z1, 1.0f, 1.0f, 1.0f, nx1, 0.0f, nz1, u1, 0.0f });
        vertices.insert(vertices.end(), { x2, -height / 2, z2, 1.0f, 1.0f, 1.0f, nx2, 0.0f, nz2, u2, 0.0f });
        vertices.insert(vertices.end(), { x1, height / 2, z1, 1.0f, 1.0f, 1.0f, nx1, 0.0f, nz1, u1, 1.0f });
        vertices.insert(vertices.end(), { x2, -height / 2, z2, 1.0f, 1.0f, 1.0f, nx2, 0.0f, nz2, u2, 0.0f });
        vertices.insert(vertices.end(), { x2, height / 2, z2, 1.0f, 1.0f, 1.0f, nx2, 0.0f, nz2, u2, 1.0f });
        vertices.insert(vertices.end(), { x1, height / 2, z1, 1.0f, 1.0f, 1.0f, nx1, 0.0f, nz1, u1, 1.0f });
        // Top cap
        vertices.insert(vertices.end(), { 0.0f, height / 2, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f });
        vertices.insert(vertices.end(), { x1, height / 2, z1, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, u1, 0.0f });
        vertices.insert(vertices.end(), { x2, height / 2, z2, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, u2, 0.0f });
        // Bottom cap
        vertices.insert(vertices.end(), { 0.0f, -height / 2, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.5f, 0.5f });
        vertices.insert(vertices.end(), { x2, -height / 2, z2, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, u2, 0.0f });
        vertices.insert(vertices.end(), { x1, -height / 2, z1, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, u1, 0.0f });
    }

    cylinderVertexCount = static_cast<int>(vertices.size() / 11);

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glBindVertexArray(0);
    return VAO;
}

unsigned int createTrapezoid() {
    // 11 floats per vertex: pos(3) + color(3) + normal(3) + texcoord(2)
    float vertices[] = {
        -0.3f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         0.3f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
        -0.3f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
        -0.3f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.3f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.3f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, -0.8f,  0.2f,  0.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, -0.8f,  0.2f,  0.0f,  0.0f, 1.0f,
        -0.3f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, -0.8f,  0.2f,  0.0f,  0.0f, 0.0f,
        -0.3f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, -0.8f,  0.2f,  0.0f,  0.0f, 0.0f,
        -0.3f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, -0.8f,  0.2f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, -0.8f,  0.2f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.8f,  0.2f,  0.0f,  0.0f, 1.0f,
         0.3f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.8f,  0.2f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.8f,  0.2f,  0.0f,  1.0f, 1.0f,
         0.3f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.8f,  0.2f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.8f,  0.2f,  0.0f,  0.0f, 1.0f,
         0.3f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.8f,  0.2f,  0.0f,  0.0f, 0.0f,
        -0.3f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
         0.3f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.3f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.3f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        -0.3f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        -0.3f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f
    };

    trapezoidVertexCount = 30;

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glBindVertexArray(0);
    return VAO;
}

unsigned int createTriangularPrism() {
    // 11 floats per vertex: pos(3) + color(3) + normal(3) + texcoord(2)
    float vertices[] = {
        // Front triangle (+Z)
        -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         0.0f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,  0.5f, 1.0f,
         // Back triangle (-Z)
         -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
          0.0f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,  0.5f, 1.0f,
          0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
          // Left slope
          -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, -0.707f,  0.707f,  0.0f,  0.0f, 0.0f,
           0.0f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, -0.707f,  0.707f,  0.0f,  0.0f, 1.0f,
           0.0f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, -0.707f,  0.707f,  0.0f,  1.0f, 1.0f,
           0.0f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, -0.707f,  0.707f,  0.0f,  1.0f, 1.0f,
          -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, -0.707f,  0.707f,  0.0f,  1.0f, 0.0f,
          -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, -0.707f,  0.707f,  0.0f,  0.0f, 0.0f,
          // Right slope
           0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.707f,  0.707f,  0.0f,  0.0f, 0.0f,
           0.0f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.707f,  0.707f,  0.0f,  1.0f, 1.0f,
           0.0f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.707f,  0.707f,  0.0f,  0.0f, 1.0f,
           0.0f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.707f,  0.707f,  0.0f,  1.0f, 1.0f,
           0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.707f,  0.707f,  0.0f,  0.0f, 0.0f,
           0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.707f,  0.707f,  0.0f,  1.0f, 0.0f,
           // Bottom
           -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
           -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
           -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f
    };

    prismVertexCount = 24;

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glBindVertexArray(0);
    return VAO;
}

unsigned int createRowboatMesh() {
    std::vector<float> vertices;
    auto pushV = [&](glm::vec3 p, glm::vec3 n) {
        vertices.insert(vertices.end(), {p.x, p.y, p.z, 1.f, 1.f, 1.f, n.x, n.y, n.z, 0.f, 0.f});
    };
    auto addTri = [&](glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
        glm::vec3 n = glm::normalize(glm::cross(p2 - p1, p3 - p1));
        pushV(p1, n); pushV(p2, n); pushV(p3, n);
    };
    auto addQuad = [&](glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4) {
        addTri(p1, p2, p3); addTri(p1, p3, p4);
    };

    const int NS = 24; // Smooth hull
    const float BL = 4.8f;
    const float BH = 0.5f; // depth
    float p0w = 0.04f, p1w = 0.65f, p2w = 0.65f, p3w = 0.04f; // Symmetric, pointed at both ends!
    float p0h = BH*0.5f, p1h = BH*0.9f, p2h = BH*0.9f, p3h = BH*0.5f; // Symmetric height!
    
    auto bez = [](float a, float b, float c, float d, float t) {
        float u = 1.f - t;
        return u*u*u*a + 3*u*u*t*b + 3*u*t*t*c + t*t*t*d;
    };

    struct Slice { glm::vec3 keel, floorL, floorR, rimL, rimR; };
    std::vector<Slice> slices;
    for (int i=0; i<=NS; i++) {
        float t = (float)i / NS;
        float w = bez(p0w, p1w, p2w, p3w, t);
        float h = bez(p0h, p1h, p2h, p3h, t);
        float x = -BL*0.5f + t * BL;
        float flare = w * 1.3f; 
        
        slices.push_back({
            glm::vec3(x, 0, 0),                        
            glm::vec3(x, 0.08f, -w*0.8f),              
            glm::vec3(x, 0.08f, w*0.8f),               
            glm::vec3(x, h, -flare),                       
            glm::vec3(x, h, flare)                         
        });
    }

    // Skin the hull quads
    for (int i=0; i<NS; i++) {
        Slice s1 = slices[i], s2 = slices[i+1];
        // Outer Port
        addQuad(s1.keel, s2.keel, s2.rimL, s1.rimL);
        // Outer Starboard
        addQuad(s1.keel, s1.rimR, s2.rimR, s2.keel);
        // Inner Port
        addQuad(s1.floorL, s1.rimL, s2.rimL, s2.floorL);
        // Inner Starboard
        addQuad(s1.floorR, s2.floorR, s2.rimR, s1.rimR);
        // Inner Floor
        addQuad(s1.floorL, s2.floorL, s2.floorR, s1.floorR);
        // Top Rim thickness (Gunwale)
        addQuad(s1.rimL, s2.rimL, s2.rimL + glm::vec3(0,0.02f,0.02f), s1.rimL + glm::vec3(0,0.02f,0.02f));
        addQuad(s1.rimR, s1.rimR + glm::vec3(0,0.02f,-0.02f), s2.rimR + glm::vec3(0,0.02f,-0.02f), s2.rimR);
    }

    // Since it's symmetric and pointed, we just cap both ends as points
    Slice sEnd = slices[NS];
    addTri(sEnd.keel, sEnd.rimL, sEnd.rimR);
    addTri(sEnd.floorL, sEnd.floorR, sEnd.rimR);
    addTri(sEnd.floorL, sEnd.rimR, sEnd.rimL);

    Slice sStart = slices[0];
    addTri(sStart.keel, sStart.rimR, sStart.rimL);
    addTri(sStart.floorL, sStart.rimL, sStart.rimR);
    addTri(sStart.floorL, sStart.rimR, sStart.floorR);

    // Add wooden thwarts (benches) inside the mesh
    for (float tX : { -1.3f, 0.0f, 1.3f }) {
        float t = (tX + BL*0.5f)/BL;
        float tw = bez(p0w, p1w, p2w, p3w, t) * 1.15f;
        float th = 0.35f;
        // top of bench
        addQuad(glm::vec3(tX-0.12f, th, -tw), glm::vec3(tX+0.12f, th, -tw), glm::vec3(tX+0.12f, th, tw), glm::vec3(tX-0.12f, th, tw));
        // thickness sides
        addQuad(glm::vec3(tX-0.12f, th-0.03f, tw), glm::vec3(tX+0.12f, th-0.03f, tw), glm::vec3(tX+0.12f, th, tw), glm::vec3(tX-0.12f, th, tw));
        addQuad(glm::vec3(tX+0.12f, th-0.03f, -tw), glm::vec3(tX-0.12f, th-0.03f, -tw), glm::vec3(tX-0.12f, th, -tw), glm::vec3(tX+0.12f, th, -tw));
    }

    rowboatVertexCount = static_cast<int>(vertices.size() / 11);
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO); glGenBuffers(1, &VBO);
    glBindVertexArray(VAO); glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3*sizeof(float))); glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6*sizeof(float))); glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9*sizeof(float))); glEnableVertexAttribArray(3);
    glBindVertexArray(0);
    return VAO;
}

unsigned int createSphere(int stacks, int slices, float uvScale) {
    std::vector<float> vertices;
    float radius = 0.5f;

    for (int i = 0; i < stacks; i++) {
        float phi1 = PI * i / stacks;
        float phi2 = PI * (i + 1) / stacks;

        for (int j = 0; j < slices; j++) {
            float theta1 = 2.0f * PI * j / slices;
            float theta2 = 2.0f * PI * (j + 1) / slices;

            glm::vec3 p1(radius * sin(phi1) * cos(theta1), radius * cos(phi1), radius * sin(phi1) * sin(theta1));
            glm::vec3 p2(radius * sin(phi2) * cos(theta1), radius * cos(phi2), radius * sin(phi2) * sin(theta1));
            glm::vec3 p3(radius * sin(phi2) * cos(theta2), radius * cos(phi2), radius * sin(phi2) * sin(theta2));
            glm::vec3 p4(radius * sin(phi1) * cos(theta2), radius * cos(phi1), radius * sin(phi1) * sin(theta2));

            glm::vec3 n1 = glm::normalize(p1), n2 = glm::normalize(p2);
            glm::vec3 n3 = glm::normalize(p3), n4 = glm::normalize(p4);

            float u1 = uvScale * (float)j / slices, u2 = uvScale * (float)(j + 1) / slices;
            float v1 = uvScale * (float)i / stacks, v2 = uvScale * (float)(i + 1) / stacks;

            // pos(3) + color(3) + normal(3) + texcoord(2)
            vertices.insert(vertices.end(), { p1.x, p1.y, p1.z, 1.0f, 1.0f, 1.0f, n1.x, n1.y, n1.z, u1, v1 });
            vertices.insert(vertices.end(), { p2.x, p2.y, p2.z, 1.0f, 1.0f, 1.0f, n2.x, n2.y, n2.z, u1, v2 });
            vertices.insert(vertices.end(), { p3.x, p3.y, p3.z, 1.0f, 1.0f, 1.0f, n3.x, n3.y, n3.z, u2, v2 });
            vertices.insert(vertices.end(), { p1.x, p1.y, p1.z, 1.0f, 1.0f, 1.0f, n1.x, n1.y, n1.z, u1, v1 });
            vertices.insert(vertices.end(), { p3.x, p3.y, p3.z, 1.0f, 1.0f, 1.0f, n3.x, n3.y, n3.z, u2, v2 });
            vertices.insert(vertices.end(), { p4.x, p4.y, p4.z, 1.0f, 1.0f, 1.0f, n4.x, n4.y, n4.z, u2, v1 });
        }
    }

    int vertCount = static_cast<int>(vertices.size() / 11);
    // Store in the appropriate global based on uvScale
    if (uvScale <= 1.01f) {
        sphereVertexCount = vertCount;
    } else {
        footballSphereVertexCount = vertCount;
    }

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glBindVertexArray(0);
    return VAO;
}


void drawCubeRotated(Shader& lightingShader, glm::vec3 color, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale) {
    glm::mat4 m = glm::scale(glm::mat4(1.0f), glm::vec3(0.85f));
    m = glm::translate(m, pos);
    m = glm::rotate(m, glm::radians(rot[0]), glm::vec3(1,0,0));
    m = glm::rotate(m, glm::radians(rot[1]), glm::vec3(0,1,0));
    m = glm::rotate(m, glm::radians(rot[2]), glm::vec3(0,0,1));
    m = glm::scale(m, scale);
    
    glActiveTexture(GL_TEXTURE0);
    
    bool isBrick = false;
    bool isFloor = false;
    
    if (scale.y >= 0.5f) { // Do not texture flat horizontal slabs (like the roof) as bricks!
        if ((std::abs(color.x - brickCol.x) < 0.01f && std::abs(color.y - brickCol.y) < 0.01f && std::abs(color.z - brickCol.z) < 0.01f) ||
            (std::abs(color.x - extWallCol.x) < 0.01f && std::abs(color.y - extWallCol.y) < 0.01f && std::abs(color.z - extWallCol.z) < 0.01f)) {
            isBrick = true;
        }
    }
    
    if ((std::abs(color.x - classFloor.x) < 0.01f && std::abs(color.y - classFloor.y) < 0.01f && std::abs(color.z - classFloor.z) < 0.01f) ||
        (std::abs(color.x - hallFloor.x) < 0.01f && std::abs(color.y - hallFloor.y) < 0.01f && std::abs(color.z - hallFloor.z) < 0.01f) ||
        (std::abs(color.x - corrFloor.x) < 0.01f && std::abs(color.y - corrFloor.y) < 0.01f && std::abs(color.z - corrFloor.z) < 0.01f)) {
        isFloor = true;
    }
    
    static int locUseTriplanar = -1;
    static int locTriplanarScale = -1;
    if (locUseTriplanar == -1) {
        locUseTriplanar = glGetUniformLocation(lightingShader.ID, "useTriplanar");
        locTriplanarScale = glGetUniformLocation(lightingShader.ID, "triplanarScale");
    }

    if (isBrick || isFloor) {
        glBindTexture(GL_TEXTURE_2D, isBrick ? texBrickWall : texFloorTiles);
        glUniform1i(locUseTriplanar, 1);
        glUniform1f(locTriplanarScale, isBrick ? 0.35f : 0.6f);
    } else {
        glBindTexture(GL_TEXTURE_2D, texDefault);
        glUniform1i(locUseTriplanar, 0);
    }
    
    drawObject(lightingShader, cubeVAO, 36, m, color, 16.0f, false);

    if (isBrick || isFloor) {
        glUniform1i(locUseTriplanar, 0);
        glBindTexture(GL_TEXTURE_2D, texDefault);
    }
}

// ==================== PLAYGROUND SLIDE ====================
// Curve types:
//   1. CLAMPED B-spline   - S-shaped slide surface (11 ctrl pts, clamped endpoints)
//   2. Ruled surface      - lateral side walls along B-spline
//   3. Cubic Bezier       - arching handrails at entry
void drawSlide(Shader& shader, glm::vec3 base) {
    glm::vec3 colOrange = glm::vec3(1.0f,  0.52f, 0.05f);
    glm::vec3 colBlue   = glm::vec3(0.18f, 0.38f, 0.75f);
    glm::vec3 colMetal  = glm::vec3(0.30f, 0.30f, 0.35f);
    glm::vec3 colPlat   = glm::vec3(0.42f, 0.42f, 0.48f);

    const float SH = 2.8f;
    const float SL = 7.2f;
    const float SW = 1.4f;
    const float WH = 0.28f;
    const float HW = SW * 0.5f;

    // --- SUPPORT LEGS (back + mid only, NO front isolated posts) ---
    for (float lx : {-HW + 0.05f, HW - 0.05f}) {
        drawCubeRotated(shader, colMetal, base + glm::vec3(lx, SH*0.5f, 0.4f),
            glm::vec3(0), glm::vec3(0.09f, SH, 0.09f));
        drawCubeRotated(shader, colMetal, base + glm::vec3(lx, SH*0.15f, SL*0.52f),
            glm::vec3(0), glm::vec3(0.09f, SH*0.30f, 0.09f));
    }
    // Back top cross-brace removed to avoid clipping the orange chute

    // --- PLATFORM (spans local z = -1.1 to 0.0, aligning with the chute start at z=0) ---
    drawCubeRotated(shader, colPlat, base + glm::vec3(0, SH + 0.04f, -0.55f),
        glm::vec3(0), glm::vec3(SW + 0.3f, 0.07f, 1.1f));
    for (float gx : {-(HW + 0.1f), HW + 0.1f}) {
        drawCubeRotated(shader, colBlue, base + glm::vec3(gx, SH + 0.55f, -0.2f),
            glm::vec3(0), glm::vec3(0.06f, 1.1f, 0.06f));
        drawCubeRotated(shader, colBlue, base + glm::vec3(gx, SH + 0.55f, 0.5f),
            glm::vec3(0), glm::vec3(0.06f, 1.1f, 0.06f));
        drawCubeRotated(shader, colBlue, base + glm::vec3(gx, SH + 1.1f, 0.15f),
            glm::vec3(0), glm::vec3(0.06f, 0.06f, 0.78f));
    }

    // --- LADDER ---
    for (float lx : {-(HW-0.05f), HW-0.05f})
        drawCubeRotated(shader, colMetal, base + glm::vec3(lx, SH*0.5f, -1.13f), // Repositioned externally
            glm::vec3(0), glm::vec3(0.06f, SH, 0.06f));
    for (int r = 0; r < 6; r++) {
        float ry = (SH / 6.0f) * (r + 0.5f);
        drawCubeRotated(shader, colMetal, base + glm::vec3(0, ry, -1.13f),       // Repositioned externally
            glm::vec3(0), glm::vec3(SW - 0.1f, 0.055f, 0.055f));
    }

    // =========================================================
    // 1. CLAMPED CUBIC B-SPLINE — S-SHAPED SLIDE SURFACE
    //    11 control points = [P0,P0,P0, P1..P4, P5,P5,P5]
    //    Clamping guarantees curve starts exactly at P0=(0,SH)
    //    and ends at P5=(SL,0), flush with platform edge.
    // =========================================================
    const int NC = 11;
    float cpZ[NC] = { 0.0f, 0.0f, 0.0f,  1.0f, 2.5f, 3.8f, 5.2f, 6.4f,  SL, SL, SL };
    float cpY[NC] = { SH, SH, SH,  SH*0.80f, SH*0.52f, SH*0.30f, SH*0.13f, 0.05f,  0.0f, 0.0f, 0.0f };

    const int NS = 42;
    float spZ[NS+1], spY[NS+1];
    int numSegs = NC - 3;

    for (int si = 0; si <= NS; si++) {
        float u  = (float)si / NS;
        float fs = u * numSegs;
        int  seg = (si == NS) ? numSegs - 1 : (int)fs;
        float t  = fs - seg;
        float t2 = t*t, t3 = t2*t;
        float b0 = (1.f/6.f)*(-t3 + 3*t2 - 3*t + 1);
        float b1 = (1.f/6.f)*( 3*t3 - 6*t2 + 4);
        float b2 = (1.f/6.f)*(-3*t3 + 3*t2 + 3*t + 1);
        float b3 = (1.f/6.f)*(t3);
        spZ[si] = b0*cpZ[seg]+b1*cpZ[seg+1]+b2*cpZ[seg+2]+b3*cpZ[seg+3];
        spY[si] = b0*cpY[seg]+b1*cpY[seg+1]+b2*cpY[seg+2]+b3*cpY[seg+3];
    }

    // Slide surface panels
    for (int i = 0; i < NS; i++) {
        float dz  = spZ[i+1] - spZ[i];
        float dy  = spY[i+1] - spY[i];
        float len = sqrtf(dz*dz + dy*dy);
        if (len < 0.0001f) continue;
        float pitchDeg = glm::degrees(atan2f(-dy, dz));
        float mz = (spZ[i] + spZ[i+1]) * 0.5f;
        float my = (spY[i] + spY[i+1]) * 0.5f;
        drawCubeRotated(shader, colOrange,
            base + glm::vec3(0.f, my, mz),
            glm::vec3(pitchDeg, 0, 0),
            glm::vec3(SW, 0.10f, len * 1.05f));
    }

    // =========================================================
    // 2. RULED SURFACE — LATERAL SIDE WALLS
    // =========================================================
    for (int i = 0; i < NS; i++) {
        float dz  = spZ[i+1] - spZ[i];
        float dy  = spY[i+1] - spY[i];
        float len = sqrtf(dz*dz + dy*dy);
        if (len < 0.0001f) continue;
        float pitchDeg = glm::degrees(atan2f(-dy, dz));
        float mz = (spZ[i] + spZ[i+1]) * 0.5f;
        float my = (spY[i] + spY[i+1]) * 0.5f;
        for (float wx : {-HW, HW}) {
            drawCubeRotated(shader, colOrange,
                base + glm::vec3(wx, my + WH*0.5f, mz),
                glm::vec3(pitchDeg, 0, 0),
                glm::vec3(0.07f, WH, len * 1.05f));
        }
    }

    // =========================================================
    // 3. CUBIC BEZIER — ARCHING HANDRAILS
    // =========================================================
    glm::vec3 bzPts[2][4] = {
        { base+glm::vec3(-HW, SH, -1.1f),
          base+glm::vec3(-HW, SH+1.05f, -0.5f),
          base+glm::vec3(-HW, SH+1.05f,  0.3f),
          base+glm::vec3(-HW, SH+0.4f,   0.6f) },
        { base+glm::vec3( HW, SH, -1.1f),
          base+glm::vec3( HW, SH+1.05f, -0.5f),
          base+glm::vec3( HW, SH+1.05f,  0.3f),
          base+glm::vec3( HW, SH+0.4f,   0.6f) }
    };


    const int BN = 14;
    for (int side = 0; side < 2; side++) {
        glm::vec3 p0=bzPts[side][0], p1=bzPts[side][1],
                  p2=bzPts[side][2], p3=bzPts[side][3];
        glm::vec3 prev = p0;
        for (int k = 1; k <= BN; k++) {
            float t = (float)k / BN, u = 1.f - t;
            glm::vec3 cur = u*u*u*p0 + 3.f*u*u*t*p1 + 3.f*u*t*t*p2 + t*t*t*p3;
            glm::vec3 seg = cur - prev;
            float sLen = glm::length(seg);
            if (sLen < 0.001f) { prev = cur; continue; }
            glm::vec3 dn = seg / sLen;
            drawCubeRotated(shader, colBlue,
                (prev + cur) * 0.5f,
                glm::vec3(glm::degrees(atan2f(-dn.y, dn.z)),
                          glm::degrees(atan2f( dn.x, dn.z)), 0.f),
                glm::vec3(0.07f, 0.07f, sLen));
            prev = cur;
        }
        drawCubeRotated(shader, colBlue,
            base + glm::vec3(side==0 ? -HW : HW, SH*0.5f, -1.1f),
            glm::vec3(0), glm::vec3(0.07f, SH, 0.07f));
    }
}

void drawPicnicSpot(Shader& shader, glm::vec3 base) {
    glm::vec3 colWood = glm::vec3(0.65f, 0.40f, 0.20f);
    glm::vec3 colUmb = glm::vec3(0.98f, 0.98f, 0.96f); // More whitish color
    glm::vec3 colPole = glm::vec3(0.35f, 0.35f, 0.35f);
    
    auto evalBezierCubic = [](glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t) -> glm::vec3 {
        float u = 1.0f - t;
        return u*u*u*p0 + 3.0f*u*u*t*p1 + 3.0f*u*t*t*p2 + t*t*t*p3;
    };
    
    auto evalBSplineCubic = [](glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t) -> glm::vec3 {
        float t2 = t*t, t3 = t2*t;
        float b0 = (1.f/6.f)*(-t3 + 3*t2 - 3*t + 1);
        float b1 = (1.f/6.f)*( 3*t3 - 6*t2 + 4);
        float b2 = (1.f/6.f)*(-3*t3 + 3*t2 + 3*t + 1);
        float b3 = (1.f/6.f)*(t3);
        return b0*p0 + b1*p1 + b2*p2 + b3*p3;
    };

    // 1. TABLE (Ruled Surfaces between Linear Bezier Curves) — scaled 1.5x
    float TW = 1.5f, TL = 3.0f, TH = 1.125f;
    glm::vec3 tL0(-TW/2, TH, -TL/2), tR0(TW/2, TH, -TL/2);
    glm::vec3 tL1(-TW/2, TH,  TL/2), tR1(TW/2, TH,  TL/2);
    int tSteps = 6;
    for(int i=0; i<tSteps; i++) {
        float t = (i + 0.5f) / tSteps;
        glm::vec3 pL = tL0 * (1-t) + tL1 * t;
        glm::vec3 pR = tR0 * (1-t) + tR1 * t;
        drawCubeRotated(shader, colWood, base + (pL+pR)*0.5f, glm::vec3(0), glm::vec3(TW, 0.07f, TL/tSteps - 0.02f));
    }
    
    // A-frame Legs (scale with TW/TH automatically)
    for (float z : {-TL*0.35f, TL*0.35f}) {
        glm::vec3 top1(-TW*0.25f, TH - 0.05f, z), bot1(-TW*0.4f, 0.0f, z);
        float roll1 = glm::degrees(atan2(top1.x - bot1.x, top1.y - bot1.y));
        drawCubeRotated(shader, colWood, base + (top1+bot1)*0.5f, glm::vec3(0,0,-roll1), glm::vec3(0.12f, glm::length(top1-bot1), 0.12f));
        
        glm::vec3 top2(TW*0.25f, TH - 0.05f, z), bot2(TW*0.4f, 0.0f, z);
        float roll2 = glm::degrees(atan2(top2.x - bot2.x, top2.y - bot2.y));
        drawCubeRotated(shader, colWood, base + (top2+bot2)*0.5f, glm::vec3(0,0,-roll2), glm::vec3(0.12f, glm::length(top2-bot2), 0.12f));
        
        drawCubeRotated(shader, colWood, base + glm::vec3(0, TH*0.4f, z), glm::vec3(0), glm::vec3(TW*0.75f, 0.09f, 0.09f));
    }

    // 2. VERY SIMPLE UMBRELLA (Faceted Pyramid)
    float uhR = 1.4f;  // umbrella radius
    float poleHeight = 2.f; // pole height above table (raised so canopy clears the table top)
    float poleTop = TH + poleHeight; // top of the pole = 0.75 + 2.5 = 3.25
    float canopyH = 0.5f; // height of the canopy cone
    // Canopy: peak at poleTop, rim fans outward-downward

    static unsigned int umbrellaVAO = 0;
    static int uVertCount = 0;
    if (umbrellaVAO == 0) {
        std::vector<float> uVerts;
        int segments = 8; // Octagonal segmented look like the reference parasol
        // Peak at local y=0 (will be placed at poleTop), rim fans outward and DOWN
        for (int i = 0; i < segments; i++) {
            float a1 = 2.0f * PI * i / segments;
            float a2 = 2.0f * PI * (i + 1) / segments;
            glm::vec3 p0(0.0f, 0.0f, 0.0f);                           // peak at top
            glm::vec3 p1(uhR * cos(a1), -canopyH, uhR * sin(a1));     // rim downward
            glm::vec3 p2(uhR * cos(a2), -canopyH, uhR * sin(a2));
            glm::vec3 n = glm::normalize(glm::cross(p2 - p0, p1 - p0));
            // Upper surface
            uVerts.insert(uVerts.end(), { p0.x, p0.y, p0.z, 1,1,1, n.x,n.y,n.z, 0.5f, 1.0f });
            uVerts.insert(uVerts.end(), { p1.x, p1.y, p1.z, 1,1,1, n.x,n.y,n.z, 0.0f, 0.0f });
            uVerts.insert(uVerts.end(), { p2.x, p2.y, p2.z, 1,1,1, n.x,n.y,n.z, 1.0f, 0.0f });
            // Under surface
            glm::vec3 nDown = -n;
            uVerts.insert(uVerts.end(), { p0.x, p0.y, p0.z, 1,1,1, nDown.x,nDown.y,nDown.z, 0.5f, 1.0f });
            uVerts.insert(uVerts.end(), { p2.x, p2.y, p2.z, 1,1,1, nDown.x,nDown.y,nDown.z, 1.0f, 0.0f });
            uVerts.insert(uVerts.end(), { p1.x, p1.y, p1.z, 1,1,1, nDown.x,nDown.y,nDown.z, 0.0f, 0.0f });
        }
        uVertCount = uVerts.size() / 11;
        unsigned int VBO;
        glGenVertexArrays(1, &umbrellaVAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(umbrellaVAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, uVerts.size() * sizeof(float), uVerts.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float))); glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float))); glEnableVertexAttribArray(2);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float))); glEnableVertexAttribArray(3);
        glBindVertexArray(0);
    }
    
    // Place canopy with its PEAK at poleTop — rim fans downward from there
    glm::mat4 uModel = glm::translate(glm::mat4(1.0f), base + glm::vec3(0, poleTop, 0));
   // drawObject(shader, umbrellaVAO, uVertCount, uModel, colUmb, 16.0f);
    
    // Draw Pole from table surface up to canopy base
    float poleCenterY = TH + (poleHeight * 0.5f);
    //drawCubeRotated(shader, colPole, base + glm::vec3(0, poleCenterY, 0), glm::vec3(0), glm::vec3(0.06f, poleHeight, 0.06f));

    // 3. CURVED BENCHES (B-Splines & Ruled Surfaces)
    float sh = 0.45f; // seat height
    float bw = 0.45f; // seat width
    float bh = 0.9f;  // backrest height
    for (int side = 0; side < 2; side++) {
        float fX = (side == 0) ? -1.0f : 1.0f; // flip X for symmetric bench
        
        // Control points forming a semi-circle around the table — closer to table
        glm::vec3 cp[5] = {
            glm::vec3(1.0f, sh, -1.8f) * glm::vec3(fX, 1, 1),
            glm::vec3(2.2f, sh, -1.2f) * glm::vec3(fX, 1, 1),
            glm::vec3(2.8f, sh,  0.0f) * glm::vec3(fX, 1, 1),
            glm::vec3(2.2f, sh,  1.2f) * glm::vec3(fX, 1, 1),
            glm::vec3(1.0f, sh,  1.8f) * glm::vec3(fX, 1, 1)
        };
        
        int bSegs = 2; 
        int bSteps = 12;
        
        // Draw the transverse ruled slats & legs
        glm::vec3 prevInner(0), prevOuter(0);
        for (int seg = 0; seg < bSegs; seg++) {
            for (int i = 0; i <= bSteps; i++) {
                if (seg > 0 && i == 0) continue; 
                float t = (float)i / bSteps;
                glm::vec3 inner = evalBSplineCubic(cp[seg], cp[seg+1], cp[seg+2], cp[seg+3], t);
                
                glm::vec3 dir = glm::normalize(glm::vec3(inner.x, 0, inner.z)); 
                glm::vec3 outer = inner + dir * bw;
                glm::vec3 top = outer + glm::vec3(0, bh - sh, 0) + dir * 0.15f;
                
                // Seat Transverse Rulers
                glm::vec3 ruleDir = glm::normalize(outer - inner);
                float rYaw = glm::degrees(atan2(ruleDir.x, ruleDir.z));
                drawCubeRotated(shader, colWood, base + (inner+outer)*0.5f, glm::vec3(0, rYaw, 0), glm::vec3(0.05f, 0.04f, glm::length(outer-inner) * 1.1f));
                
                // Backrest Transverse Rulers (REMOVED for minimalist flat-top plank surface bench)
                
                // Legs at intervals
                if (i % (bSteps/2) == 0) {
                    glm::vec3 legMid = inner + dir * (bw/2) - glm::vec3(0, sh/2, 0);
                    drawCubeRotated(shader, colWood, base + legMid, glm::vec3(0, rYaw, 0), glm::vec3(bw*0.8f, sh, 0.06f));
                }
                
                // Longitudinal seat braces
                if (i > 0 || seg > 0) {
                    glm::vec3 innerLower = inner - glm::vec3(0, 0.04f, 0);
                    glm::vec3 outerLower = outer - glm::vec3(0, 0.04f, 0);
                    
                    glm::vec3 midIn = (innerLower + prevInner) * 0.5f;
                    float yawIn = glm::degrees(atan2((innerLower-prevInner).x, (innerLower-prevInner).z));
                    drawCubeRotated(shader, colWood, base + midIn + dir*0.05f, glm::vec3(0, yawIn, 0), glm::vec3(0.05f, 0.05f, glm::length(innerLower-prevInner)*1.05f));
                    
                    glm::vec3 midOut = (outerLower + prevOuter) * 0.5f;
                    float yawOut = glm::degrees(atan2((outerLower-prevOuter).x, (outerLower-prevOuter).z));
                    drawCubeRotated(shader, colWood, base + midOut - dir*0.05f, glm::vec3(0, yawOut, 0), glm::vec3(0.05f, 0.05f, glm::length(outerLower-prevOuter)*1.05f));
                }
                prevInner = inner - glm::vec3(0, 0.04f, 0); 
                prevOuter = outer - glm::vec3(0, 0.04f, 0);
            }
        }
    }
}

void drawRealisticSchool(Shader& lightingShader) {
    drawLeftWing(lightingShader);
    drawCenterEntrance(lightingShader);
    drawRightWing(lightingShader);
    drawCorridor(lightingShader);
    drawFlagpole(lightingShader);
}

void drawWindow(Shader& lightingShader, glm::vec3 pos, glm::vec3 scale, bool isBackWindow) {
    // Top frame
    drawCubeRotated(lightingShader, deskMetal, pos + glm::vec3(0, scale.y/2.0f - 0.05f, 0), glm::vec3(0), glm::vec3(scale.x, 0.1f, scale.z));
    // Bottom frame
    drawCubeRotated(lightingShader, deskMetal, pos + glm::vec3(0, -scale.y/2.0f + 0.05f, 0), glm::vec3(0), glm::vec3(scale.x, 0.1f, scale.z));
    // Left frame
    drawCubeRotated(lightingShader, deskMetal, pos + glm::vec3(-scale.x/2.0f + 0.05f, 0, 0), glm::vec3(0), glm::vec3(0.1f, scale.y, scale.z));
    // Right frame
    drawCubeRotated(lightingShader, deskMetal, pos + glm::vec3(scale.x/2.0f - 0.05f, 0, 0), glm::vec3(0), glm::vec3(0.1f, scale.y, scale.z));
    
    // Vertical mullion
    drawCubeRotated(lightingShader, deskMetal, pos, glm::vec3(0), glm::vec3(0.1f, scale.y, scale.z));
    // Horizontal mullion
    drawCubeRotated(lightingShader, deskMetal, pos, glm::vec3(0), glm::vec3(scale.x, 0.1f, scale.z));

    // Glass panes - only draw when windows are CLOSED
    // When open, the window is a clear opening so you can see inside classrooms
    if (!windowsOpen) {
        float paneW = scale.x / 2.0f - 0.05f;
        float paneH = scale.y - 0.1f;
        
        // Left pane (fixed)
        drawCubeRotated(lightingShader, glassCol, pos + glm::vec3(-scale.x/4.0f, 0, 0.01f), glm::vec3(0), glm::vec3(paneW, paneH, 0.02f));
        // Right pane
        drawCubeRotated(lightingShader, glassCol, pos + glm::vec3(scale.x/4.0f, 0, -0.01f), glm::vec3(0), glm::vec3(paneW, paneH, 0.02f));
    }
}

void drawLeftWing(Shader& lightingShader) {
    float cx = -10.0f;
    float w = 14.0f, d = 8.0f;  // Classroom depth (Z=0 to Z=-8)

    for (int floor = 0; floor <= 1; floor++) {
        float fY = floor * FL_H;
        
        // Floor and ceiling slabs
        drawCubeRotated(lightingShader, classFloor, glm::vec3(cx, fY + 0.1f, -4.0f), glm::vec3(0), glm::vec3(w, 0.2f, d));
        if (floor == 0) drawCubeRotated(lightingShader, ceilCol, glm::vec3(cx, fY + FL_H - 0.1f, -4.0f), glm::vec3(0), glm::vec3(w, 0.2f, d));

        // Left Side Solid Wall
        drawCubeRotated(lightingShader, brickCol, glm::vec3(cx - w/2.0f + 0.1f, fY + FL_H/2.0f, -4.0f), glm::vec3(0), glm::vec3(0.2f, FL_H, d));
        if (floor==1) drawCubeRotated(lightingShader, extWallCol, glm::vec3(cx, fY + FL_H - 0.1f, -4.0f), glm::vec3(0), glm::vec3(w, 0.2f, d)); // Roof

        // Internal dividers between the 3 rooms
        drawCubeRotated(lightingShader, intWallCol, glm::vec3(-12.25f, fY + FL_H/2.0f, -4.0f), glm::vec3(0), glm::vec3(0.2f, FL_H, d));
        drawCubeRotated(lightingShader, intWallCol, glm::vec3(-7.75f, fY + FL_H/2.0f, -4.0f), glm::vec3(0), glm::vec3(0.2f, FL_H, d));

        // Front Interior Partition (Z = -0.1f) - Clean continuous wall with door holes
        float pZ = -0.1f;
        drawCubeRotated(lightingShader, intWallCol, glm::vec3(-16.0f, fY + FL_H/2.0f, pZ), glm::vec3(0), glm::vec3(2.0f, FL_H, 0.2f)); // Left edge
        drawCubeRotated(lightingShader, intWallCol, glm::vec3(-12.25f, fY + FL_H/2.0f, pZ), glm::vec3(0), glm::vec3(3.5f, FL_H, 0.2f)); // Between door 1 and 2
        drawCubeRotated(lightingShader, intWallCol, glm::vec3(-7.75f, fY + FL_H/2.0f, pZ), glm::vec3(0), glm::vec3(3.5f, FL_H, 0.2f)); // Between door 2 and 3
        drawCubeRotated(lightingShader, intWallCol, glm::vec3(-4.0f, fY + FL_H/2.0f, pZ), glm::vec3(0), glm::vec3(2.0f, FL_H, 0.2f)); // Right edge
        // Top continuous beam over doors
        drawCubeRotated(lightingShader, intWallCol, glm::vec3(cx, fY + DOOR_H + (FL_H - DOOR_H)/2.0f, pZ), glm::vec3(0), glm::vec3(w, FL_H - DOOR_H, 0.2f));

        // Back Face Grid Structure (Z = -7.9f) - Now EXTERIOR wall with windows
        float bGridZ = -7.9f;
        drawCubeRotated(lightingShader, brickCol, glm::vec3(cx, fY + 0.5f, bGridZ), glm::vec3(0), glm::vec3(w, 1.0f, 0.2f));
        drawCubeRotated(lightingShader, brickCol, glm::vec3(cx, fY + FL_H - 0.5f, bGridZ), glm::vec3(0), glm::vec3(w, 1.0f, 0.2f));
        drawCubeRotated(lightingShader, brickCol, glm::vec3(-16.375f, fY + FL_H/2.0f, bGridZ), glm::vec3(0), glm::vec3(1.25f, FL_H, 0.2f));
        drawCubeRotated(lightingShader, brickCol, glm::vec3(-12.25f, fY + FL_H/2.0f, bGridZ), glm::vec3(0), glm::vec3(2.0f, FL_H, 0.2f));
        drawCubeRotated(lightingShader, brickCol, glm::vec3(-7.75f, fY + FL_H/2.0f, bGridZ), glm::vec3(0), glm::vec3(2.0f, FL_H, 0.2f));
        drawCubeRotated(lightingShader, brickCol, glm::vec3(-3.625f, fY + FL_H/2.0f, bGridZ), glm::vec3(0), glm::vec3(1.25f, FL_H, 0.2f));

        // Back windows and classroom interiors
        float bWinZ = -7.85f;
        float winH = 1.6f, winD = 0.05f;
        for (float wx : {-14.5f, -10.0f, -5.5f}) {
            drawWindow(lightingShader, glm::vec3(wx, fY + FL_H/2.0f, bWinZ), glm::vec3(2.5f, winH, winD), true);
            
            // Thick exterior window frames - BACK (protruding concrete)
            drawCubeRotated(lightingShader, extWallCol, glm::vec3(wx, fY + FL_H/2.0f + winH/2.0f + 0.15f, bWinZ - 0.1f), glm::vec3(0), glm::vec3(2.9f, 0.3f, 0.3f));
            drawCubeRotated(lightingShader, extWallCol, glm::vec3(wx, fY + FL_H/2.0f - winH/2.0f - 0.15f, bWinZ - 0.1f), glm::vec3(0), glm::vec3(2.9f, 0.3f, 0.3f));
            drawCubeRotated(lightingShader, extWallCol, glm::vec3(wx - 1.3f, fY + FL_H/2.0f, bWinZ - 0.1f), glm::vec3(0), glm::vec3(0.3f, winH + 0.6f, 0.3f));
            drawCubeRotated(lightingShader, extWallCol, glm::vec3(wx + 1.3f, fY + FL_H/2.0f, bWinZ - 0.1f), glm::vec3(0), glm::vec3(0.3f, winH + 0.6f, 0.3f));

            drawClassroomInterior(lightingShader, wx, -4.0f, fY + 0.2f);
        }
    }
}

void drawCenterEntrance(Shader& lightingShader) {
    // --- MAIN BUILDING BEHIND PORCH ---
    // Floor Slabs (Z=0 to Z=-8, depth 8, center Z=-4, length Z=1.6 center Z=-1??)
    // The front part Z=-1 is fine, just let corridor do Z=0 to Z=3.
    // Ground floor
    drawCubeRotated(lightingShader, classFloor, glm::vec3(0.0f, 0.1f, -4.0f), glm::vec3(0), glm::vec3(5.6f, 0.2f, 8.0f));
    // First floor slab with hole for stairs
    drawCubeRotated(lightingShader, classFloor, glm::vec3(-1.9f, FL_H + 0.1f, -4.0f), glm::vec3(0), glm::vec3(1.8f, 0.2f, 8.0f)); // Left part
    drawCubeRotated(lightingShader, ceilCol, glm::vec3(-1.9f, FL_H - 0.1f, -4.0f), glm::vec3(0), glm::vec3(1.8f, 0.2f, 8.0f));
    drawCubeRotated(lightingShader, classFloor, glm::vec3( 1.9f, FL_H + 0.1f, -4.0f), glm::vec3(0), glm::vec3(1.8f, 0.2f, 8.0f)); // Right part
    drawCubeRotated(lightingShader, ceilCol, glm::vec3( 1.9f, FL_H - 0.1f, -4.0f), glm::vec3(0), glm::vec3(1.8f, 0.2f, 8.0f));

    // Solid Side Walls (Z=0 to Z=-8)
    drawCubeRotated(lightingShader, extWallCol, glm::vec3(-2.9f, FL_H, -4.0f), glm::vec3(0), glm::vec3(0.2f, FL_H*2.0f, 8.0f));
    drawCubeRotated(lightingShader, extWallCol, glm::vec3( 2.9f, FL_H, -4.0f), glm::vec3(0), glm::vec3(0.2f, FL_H*2.0f, 8.0f));
    drawCubeRotated(lightingShader, extWallCol, glm::vec3(0.0f, FL_H*2.0f - 0.1f, -4.0f), glm::vec3(0), glm::vec3(6.0f, 0.2f, 8.0f)); // Roof

    // Back wall grid structure (Z = -7.9f) - EXTERIOR wall (staircase back wall)
    float bGridZ = -7.9f;
    for (int floor = 0; floor <= 1; floor++) {
        float fY = floor * FL_H;
        // Solid wall for center section (staircase area)
        drawCubeRotated(lightingShader, extWallCol, glm::vec3(0.0f, fY + FL_H/2.0f, bGridZ), glm::vec3(0), glm::vec3(6.0f, FL_H, 0.2f));
    }

    // Ground Floor Front Wall (Shifted to Z = 2.9f)
    drawCubeRotated(lightingShader, brickCol, glm::vec3(0.0f, 3.1f, 2.9f), glm::vec3(0), glm::vec3(6.0f, 1.0f, 0.2f)); // Top strip
    drawCubeRotated(lightingShader, brickCol, glm::vec3(-2.2f, FL_H/2.0f, 2.9f), glm::vec3(0), glm::vec3(1.6f, FL_H, 0.2f)); // Thick Left pillar sealing to door
    drawCubeRotated(lightingShader, brickCol, glm::vec3( 2.2f, FL_H/2.0f, 2.9f), glm::vec3(0), glm::vec3(1.6f, FL_H, 0.2f)); // Thick Right pillar sealing to door

    // Grand Entrance Doors (Shifted to Z = 2.85f)
    drawCubeRotated(lightingShader, doorCol, glm::vec3(-0.95f, 1.3f, 2.85f), glm::vec3(0, 80, 0), glm::vec3(0.9f, 2.6f, 0.05f));
    drawCubeRotated(lightingShader, doorCol, glm::vec3( 0.95f, 1.3f, 2.85f), glm::vec3(0, -80, 0), glm::vec3(0.9f, 2.6f, 0.05f));

    // Internal Grand Staircase (Starts from Z=-1.8, going down to ground floor inside the Z<0 space)
    int inSteps = 16;
    float inRun = 4.4f / inSteps;           // 0.275 per step
    float inRise = FL_H / inSteps;          // 0.225 per step
    glm::vec3 stairWhite = glm::vec3(0.95f, 0.95f, 0.95f);
    for(int i=0; i<inSteps; i++) {
        drawCubeRotated(lightingShader, stairWhite, glm::vec3(0.0f, inRise*i + inRise/2.0f, -1.8f - inRun*i - inRun/2.0f), glm::vec3(0), glm::vec3(2.0f, inRise, inRun));
    }

    // --- 2nd Floor Stairwell Landing ---
    // The 1st-floor slab has a gap (x: -1.0 to +1.0, full depth) for the stairwell opening.
    // The staircase tops out at y = FL_H, z ≈ -6.2 (last step rear edge = -1.8 - 4.4).
    // Without a floor here, there is a void at the top of the stairs. Fix: add a
    // continuous landing slab at FL_H that matches the left/right slabs and fills
    // the entire 2.0-wide gap from the front entry (z=-1.8) to the back wall (z=-7.8).
    // This is the flat surface a person steps onto when they reach the 2nd floor.
    float landingGapW  = 2.0f;    // full width of the stairwell hole in the slab
    float landingZFront = -1.8f;  // front edge (door side, same as stair start)
    float landingZBack  = -7.8f;  // back edge, just before the back wall at -7.9
    float landingZCenter = (landingZFront + landingZBack) / 2.0f; // -4.8
    float landingZDepth  = landingZFront - landingZBack;          // 6.0

    // Floor slab of the landing (top surface at FL_H + 0.2, matches the left/right slabs)
    drawCubeRotated(lightingShader, classFloor,
        glm::vec3(0.0f, FL_H + 0.1f, landingZCenter),
        glm::vec3(0), glm::vec3(landingGapW, 0.2f, landingZDepth));

    // Ceiling of the space below the landing (ground-floor ceiling at FL_H - 0.1, matches the rest)
    drawCubeRotated(lightingShader, ceilCol,
        glm::vec3(0.0f, FL_H - 0.1f, landingZCenter),
        glm::vec3(0), glm::vec3(landingGapW, 0.2f, landingZDepth));

    // First Floor Front Wall (Shifted to Z = 2.9f)
    drawCubeRotated(lightingShader, brickCol, glm::vec3(0.0f, FL_H + 0.5f, 2.9f), glm::vec3(0), glm::vec3(6.0f, 1.0f, 0.2f)); // Balcony rim below split windows
    drawCubeRotated(lightingShader, brickCol, glm::vec3(0.0f, FL_H*2.0f - 0.4f, 2.9f), glm::vec3(0), glm::vec3(6.0f, 0.8f, 0.2f)); // Roof rim
    drawCubeRotated(lightingShader, brickCol, glm::vec3(-2.55f, FL_H + FL_H/2.0f, 2.9f), glm::vec3(0), glm::vec3(0.9f, FL_H, 0.2f)); // Left side
    drawCubeRotated(lightingShader, brickCol, glm::vec3( 2.55f, FL_H + FL_H/2.0f, 2.9f), glm::vec3(0), glm::vec3(0.9f, FL_H, 0.2f)); // Right side
    drawCubeRotated(lightingShader, brickCol, glm::vec3(0.0f, FL_H + FL_H/2.0f, 2.9f), glm::vec3(0), glm::vec3(1.1f, FL_H, 0.2f)); // Center pillar

    // Now draw two windows instead of one large one
    float cwX[2] = {-1.3f, 1.3f};
    for(int i=0; i<2; i++) {
        drawWindow(lightingShader, glm::vec3(cwX[i], FL_H + 1.8f, 2.85f), glm::vec3(1.6f, 1.6f, 0.05f), false);
        // frames (Shifted visually +3.0)
        drawCubeRotated(lightingShader, extWallCol, glm::vec3(cwX[i], FL_H + 1.8f + 0.8f + 0.15f, 2.95f), glm::vec3(0), glm::vec3(2.0f, 0.3f, 0.3f)); // Top
        drawCubeRotated(lightingShader, extWallCol, glm::vec3(cwX[i], FL_H + 1.8f - 0.8f - 0.15f, 2.95f), glm::vec3(0), glm::vec3(2.0f, 0.3f, 0.3f)); // Bottom
        drawCubeRotated(lightingShader, extWallCol, glm::vec3(cwX[i] - 0.85f, FL_H + 1.8f, 2.95f), glm::vec3(0), glm::vec3(0.3f, 2.2f, 0.3f)); // Left
        drawCubeRotated(lightingShader, extWallCol, glm::vec3(cwX[i] + 0.85f, FL_H + 1.8f, 2.95f), glm::vec3(0), glm::vec3(0.3f, 2.2f, 0.3f)); // Right
    }

    // --- PORCH --- (Shifted +3.0)
    // Porch Roof Floor Slab
    drawCubeRotated(lightingShader, extWallCol, glm::vec3(0.0f, FL_H - 0.2f, 4.5f), glm::vec3(0), glm::vec3(6.0f, 0.4f, 3.2f));
    // 4 Porch Pillars
    for(float px : {-2.6f, -0.9f, 0.9f, 2.6f}) {
        drawCubeRotated(lightingShader, pillarCol, glm::vec3(px, FL_H/2.0f - 0.2f, 5.5f), glm::vec3(0), glm::vec3(0.4f, FL_H, 0.4f));
    }

    // Entrance stairs (Shifted +3.0 -> starts around 7.0)
    int steps = 6;
    float stepRun = 2.5f / steps;
    float stepRise = 0.6f / steps; 
    for (int i=0; i<steps; i++) {
        drawCubeRotated(lightingShader, deskMetal, glm::vec3(0.0f, stepRise*i + stepRise/2.0f, 7.0f - stepRun*i), glm::vec3(0), glm::vec3(6.0f, stepRise, stepRun));
    }

    // Signboard blocks
    glm::vec3 signCol = glm::vec3(0.95f, 0.95f, 0.95f); // White/light grey like the image
    // Bengali text signboard (Shifted +3.0)
    drawCubeRotated(lightingShader, signCol, glm::vec3(0.0f, FL_H + 0.4f, 6.0f), glm::vec3(0), glm::vec3(5.0f, 0.8f, 0.1f));
    // English text signboard (Shifted +3.0)
    drawCubeRotated(lightingShader, signCol, glm::vec3(0.0f, 2.0f*FL_H + 0.6f, 2.9f), glm::vec3(0), glm::vec3(5.6f, 1.0f, 0.1f));
    drawCubeRotated(lightingShader, signCol, glm::vec3(0.0f, 2.0f*FL_H + 0.6f, 2.9f), glm::vec3(0), glm::vec3(5.6f, 1.0f, 0.1f));
}


void drawRightWing(Shader& lightingShader) {
    float cx = 11.0f, w = 16.0f;
    float d = 8.0f;  // Classroom depth (Z=0 to Z=-8)
    
    for (int floor = 0; floor <= 1; floor++) {
        float fY = floor * FL_H;
        
        // Classroom floor and ceiling
        drawCubeRotated(lightingShader, classFloor, glm::vec3(cx, fY + 0.1f, -4.0f), glm::vec3(0), glm::vec3(w, 0.2f, d));
        if (floor==0) drawCubeRotated(lightingShader, ceilCol, glm::vec3(cx, fY + FL_H - 0.1f, -4.0f), glm::vec3(0), glm::vec3(w, 0.2f, d));

        // Right Side Solid Wall 
        drawCubeRotated(lightingShader, extWallCol, glm::vec3(cx + w/2.0f - 0.1f, fY + FL_H/2.0f, -4.0f), glm::vec3(0), glm::vec3(0.2f, FL_H, d));
        // Roof
        if (floor==1) drawCubeRotated(lightingShader, extWallCol, glm::vec3(cx, fY + FL_H - 0.1f, -4.0f), glm::vec3(0), glm::vec3(w, 0.2f, d));
        
        // Inner room dividers
        drawCubeRotated(lightingShader, intWallCol, glm::vec3(7.75f, fY + FL_H/2.0f, -4.0f), glm::vec3(0), glm::vec3(0.2f, FL_H, d));
        drawCubeRotated(lightingShader, intWallCol, glm::vec3(12.25f, fY + FL_H/2.0f, -4.0f), glm::vec3(0), glm::vec3(0.2f, FL_H, d));

        // Front Interior Partition (Z = -0.1f) - Clean continuous wall with door holes
        float pZ = -0.1f;
        drawCubeRotated(lightingShader, intWallCol, glm::vec3(4.0f, fY + FL_H/2.0f, pZ), glm::vec3(0), glm::vec3(2.0f, FL_H, 0.2f)); // Left edge
        drawCubeRotated(lightingShader, intWallCol, glm::vec3(7.75f, fY + FL_H/2.0f, pZ), glm::vec3(0), glm::vec3(3.5f, FL_H, 0.2f)); // Between door 1 and 2
        drawCubeRotated(lightingShader, intWallCol, glm::vec3(12.25f, fY + FL_H/2.0f, pZ), glm::vec3(0), glm::vec3(3.5f, FL_H, 0.2f)); // Between door 2 and 3
        drawCubeRotated(lightingShader, intWallCol, glm::vec3(17.0f, fY + FL_H/2.0f, pZ), glm::vec3(0), glm::vec3(4.0f, FL_H, 0.2f)); // Right edge spans 15.0 to 19.0
        // Top continuous beam over doors
        drawCubeRotated(lightingShader, intWallCol, glm::vec3(cx, fY + DOOR_H + (FL_H - DOOR_H)/2.0f, pZ), glm::vec3(0), glm::vec3(w, FL_H - DOOR_H, 0.2f));

        // Back wall grid (Z = -7.9f) - Now EXTERIOR wall with windows
        float bGridZ = -7.9f;
        drawCubeRotated(lightingShader, brickCol, glm::vec3(cx, fY + 0.5f, bGridZ), glm::vec3(0), glm::vec3(w, 1.0f, 0.2f));
        drawCubeRotated(lightingShader, brickCol, glm::vec3(cx, fY + FL_H - 0.5f, bGridZ), glm::vec3(0), glm::vec3(w, 1.0f, 0.2f));
        drawCubeRotated(lightingShader, brickCol, glm::vec3(3.625f, fY + FL_H/2.0f, bGridZ), glm::vec3(0), glm::vec3(1.25f, FL_H, 0.2f));
        drawCubeRotated(lightingShader, brickCol, glm::vec3(7.75f, fY + FL_H/2.0f, bGridZ), glm::vec3(0), glm::vec3(2.0f, FL_H, 0.2f));
        drawCubeRotated(lightingShader, brickCol, glm::vec3(12.25f, fY + FL_H/2.0f, bGridZ), glm::vec3(0), glm::vec3(2.0f, FL_H, 0.2f));
        drawCubeRotated(lightingShader, brickCol, glm::vec3(17.375f, fY + FL_H/2.0f, bGridZ), glm::vec3(0), glm::vec3(3.25f, FL_H, 0.2f));

        // Back windows and classroom interiors
        float bWinZ = -7.85f;
        float winH = 1.6f, winD = 0.05f;
        for (float wx : {5.5f, 10.0f, 14.5f}) {
            drawWindow(lightingShader, glm::vec3(wx, fY + FL_H/2.0f, bWinZ), glm::vec3(2.5f, winH, winD), true);
            
            // Thick exterior window frames - BACK (protruding concrete, visible from backside)
            drawCubeRotated(lightingShader, extWallCol, glm::vec3(wx, fY + FL_H/2.0f + winH/2.0f + 0.15f, bWinZ - 0.1f), glm::vec3(0), glm::vec3(2.9f, 0.3f, 0.3f)); // Top
            drawCubeRotated(lightingShader, extWallCol, glm::vec3(wx, fY + FL_H/2.0f - winH/2.0f - 0.15f, bWinZ - 0.1f), glm::vec3(0), glm::vec3(2.9f, 0.3f, 0.3f)); // Bottom
            drawCubeRotated(lightingShader, extWallCol, glm::vec3(wx - 1.3f, fY + FL_H/2.0f, bWinZ - 0.1f), glm::vec3(0), glm::vec3(0.3f, winH + 0.6f, 0.3f)); // Left
            drawCubeRotated(lightingShader, extWallCol, glm::vec3(wx + 1.3f, fY + FL_H/2.0f, bWinZ - 0.1f), glm::vec3(0), glm::vec3(0.3f, winH + 0.6f, 0.3f)); // Right

            drawClassroomInterior(lightingShader, wx, -4.0f, fY + 0.2f);
        }
    }

    // Right Wing Entrance Steps (shifted +3.0 in Z for new corridor front)
    int rSteps = 4;
    float rStepRise = 0.5f / rSteps;
    float rStepRun = 1.5f / rSteps;
    for (int i = 0; i < rSteps; i++) {
        drawCubeRotated(lightingShader, extWallCol,
            glm::vec3(7.5f, rStepRise * i + rStepRise / 2.0f, 3.0f + rStepRun * i + rStepRun / 2.0f),
            glm::vec3(0), glm::vec3(3.0f, rStepRise, rStepRun));
    }
}

void drawFlagpole(Shader& lightingShader) {
    float baseX = -6.0f;
    float baseZ = 7.5f;
    float y = 0.1f;
    
    // Tiered base
    glm::mat4 bm = glm::scale(glm::mat4(1.0f), glm::vec3(0.85f));
    bm = glm::translate(bm, glm::vec3(baseX, y, baseZ));
    bm = glm::scale(bm, glm::vec3(1.5f, 0.2f, 1.5f));
    drawCylinderModel(lightingShader, bm, extWallCol, 16.0f, false);
    
    y += 0.2f;
    bm = glm::scale(glm::mat4(1.0f), glm::vec3(0.85f));
    bm = glm::translate(bm, glm::vec3(baseX, y, baseZ));
    bm = glm::scale(bm, glm::vec3(1.0f, 0.2f, 1.0f));
    drawCylinderModel(lightingShader, bm, extWallCol, 16.0f, false);
    
    // Pole
    y += 0.1f;
    float poleH = 10.0f;
    bm = glm::scale(glm::mat4(1.0f), glm::vec3(0.85f));
    bm = glm::translate(bm, glm::vec3(baseX, y + poleH/2.0f, baseZ));
    bm = glm::scale(bm, glm::vec3(0.05f, poleH, 0.05f));
    drawCylinderModel(lightingShader, bm, glm::vec3(0.8f), 16.0f, false); 
    
    // Bangladesh Flag
    glm::vec3 flagGreen = glm::vec3(0.0f, 0.4f, 0.2f);
    glm::vec3 flagRed = glm::vec3(0.8f, 0.1f, 0.1f);
    
    unsigned int dummyVAO = 0; // Not used
    drawCubeRotated(lightingShader, flagGreen, glm::vec3(baseX + 1.25f, y + poleH - 1.0f, baseZ), glm::vec3(0, 5, 0), glm::vec3(2.5f, 1.5f, 0.02f));
    
    glm::mat4 fm = glm::scale(glm::mat4(1.0f), glm::vec3(0.85f));
    fm = glm::translate(fm, glm::vec3(baseX + 1.0f, y + poleH - 1.0f, baseZ + 0.02f));
        fm = glm::rotate(fm, glm::radians(90.0f), glm::vec3(1,0,0));
    fm = glm::scale(fm, glm::vec3(0.5f, 0.03f, 0.5f)); 
    drawCylinderModel(lightingShader, fm, flagRed, 16.0f, false);
}

// ==== INTERIOR FURNITURE ====
void drawClassroomInterior(Shader& lightingShader, float cX, float zCenter, float yBase) {
    // Whiteboard at the BACK wall (-Z direction from room center)
    float bZ = zCenter - 3.8f; 
    drawCubeRotated(lightingShader, boardCol, glm::vec3(cX, yBase + 1.6f, bZ), glm::vec3(0), glm::vec3(3.0f, 1.4f, 0.05f));
    
    // Teacher Desk
    drawCubeRotated(lightingShader, deskWood, glm::vec3(cX - 1.0f, yBase + 0.6f, bZ + 1.5f), glm::vec3(0), glm::vec3(2.0f, 0.05f, 1.0f));
    drawCubeRotated(lightingShader, deskMetal, glm::vec3(cX - 1.0f - 0.9f, yBase + 0.3f, bZ + 1.0f), glm::vec3(0), glm::vec3(0.1f, 0.6f, 0.1f));
    drawCubeRotated(lightingShader, deskMetal, glm::vec3(cX - 1.0f + 0.9f, yBase + 0.3f, bZ + 1.0f), glm::vec3(0), glm::vec3(0.1f, 0.6f, 0.1f));

    // Student Desks Grid (Facing the back wall)
    float startZ = zCenter - 1.0f; 
    for (int row=0; row<3; row++) {
        float dZ = startZ + row*1.8f; // Desks get closer to the front windows (+Z)
        for (int col=-1; col<=1; col++) {
            float dX = cX + (col * 1.2f);
            drawDesk(lightingShader, glm::vec3(dX, yBase, dZ), 180.0f); // Rotate 180 degrees to face back
        }
    }
}

void drawDesk(Shader& lightingShader, glm::vec3 pos, float rotY) {
    glm::vec3 rotVec = glm::vec3(0, rotY, 0);
    // Desk surface
    drawCubeRotated(lightingShader, deskWood, pos + glm::vec3(0, 0.5f, 0), rotVec, glm::vec3(1.0f, 0.05f, 0.6f));
    
    // To handle rotation for legs and benches properly while using drawCube, it's easier to just assume symmetrical or approximate visually
    // since my simple drawCube applies rotation relative to the cube's local center.
    // Instead of doing complex math, I'll pass the rotation and just offset symmetrically!
    
    // desk legs (approximate for any rotation since it's a small square)
    drawCubeRotated(lightingShader, deskMetal, pos + glm::vec3(-0.4f, 0.25f, -0.2f), rotVec, glm::vec3(0.05f, 0.5f, 0.05f));
    drawCubeRotated(lightingShader, deskMetal, pos + glm::vec3( 0.4f, 0.25f, -0.2f), rotVec, glm::vec3(0.05f, 0.5f, 0.05f));
    drawCubeRotated(lightingShader, deskMetal, pos + glm::vec3(-0.4f, 0.25f,  0.2f), rotVec, glm::vec3(0.05f, 0.5f, 0.05f));
    drawCubeRotated(lightingShader, deskMetal, pos + glm::vec3( 0.4f, 0.25f,  0.2f), rotVec, glm::vec3(0.05f, 0.5f, 0.05f));
    
    // Student bench/chair (offset based on rotation!)
    float benchDir = (rotY > 90.0f) ? 1.0f : -1.0f;
    // Chair seat
    drawCubeRotated(lightingShader, lockerCol, pos + glm::vec3(0, 0.3f, benchDir * 0.6f), rotVec, glm::vec3(0.6f, 0.05f, 0.5f));
    // Chair backrest
    drawCubeRotated(lightingShader, lockerCol, pos + glm::vec3(0, 0.6f, benchDir * 0.8f), rotVec, glm::vec3(0.6f, 0.5f, 0.05f));

    // 4 Chair legs — placed at the four corners of the seat, running from floor to seat bottom
    // Seat bottom is at y = 0.3 - 0.025 = 0.275, so leg height = 0.275, center at y = 0.1375
    float legH   = 0.275f;
    float legY   = legH / 2.0f;          // 0.1375 — vertical center of leg
    float legW   = 0.05f;                 // leg cross-section
    float seatHW = 0.25f;                 // half width  of seat (0.6/2 - small inset)
    float seatHD = 0.20f;                 // half depth  of seat (0.5/2 - small inset)
    // Front-left leg
    drawCubeRotated(lightingShader, deskMetal,
        pos + glm::vec3(-seatHW, legY, benchDir * (0.6f - seatHD)), rotVec, glm::vec3(legW, legH, legW));
    // Front-right leg
    drawCubeRotated(lightingShader, deskMetal,
        pos + glm::vec3( seatHW, legY, benchDir * (0.6f - seatHD)), rotVec, glm::vec3(legW, legH, legW));
    // Back-left leg
    drawCubeRotated(lightingShader, deskMetal,
        pos + glm::vec3(-seatHW, legY, benchDir * (0.6f + seatHD)), rotVec, glm::vec3(legW, legH, legW));
    // Back-right leg
    drawCubeRotated(lightingShader, deskMetal,
        pos + glm::vec3( seatHW, legY, benchDir * (0.6f + seatHD)), rotVec, glm::vec3(legW, legH, legW));
}


// ==== DOOR HELPER ====
void drawDoor(Shader& lightingShader, glm::vec3 pos, float doorWidth, float doorHeight, bool opens) {
    // Door frame (dark wood surround)
    float frameT = 0.08f;
    // Top frame
    drawCubeRotated(lightingShader, doorFrame, pos + glm::vec3(0, doorHeight/2.0f + frameT/2.0f, 0), glm::vec3(0), glm::vec3(doorWidth + frameT*2, frameT, 0.22f));
    // Left frame
    drawCubeRotated(lightingShader, doorFrame, pos + glm::vec3(-doorWidth/2.0f - frameT/2.0f, 0, 0), glm::vec3(0), glm::vec3(frameT, doorHeight, 0.22f));
    // Right frame
    drawCubeRotated(lightingShader, doorFrame, pos + glm::vec3(doorWidth/2.0f + frameT/2.0f, 0, 0), glm::vec3(0), glm::vec3(frameT, doorHeight, 0.22f));

    // Door panel (hinged on left side)
    if (opens && doorOpen) {
        // Open door - rotated 80 degrees around left hinge
        drawCubeRotated(lightingShader, doorCol, pos + glm::vec3(-doorWidth/2.0f + 0.02f, 0, -0.4f), glm::vec3(0, 80, 0), glm::vec3(doorWidth, doorHeight, 0.06f));
    } else {
        // Closed door
        drawCubeRotated(lightingShader, doorCol, pos, glm::vec3(0), glm::vec3(doorWidth, doorHeight, 0.06f));
    }

    // Door handle (small knob)
    drawCubeRotated(lightingShader, deskMetal, pos + glm::vec3(doorWidth/2.0f - 0.15f, 0, 0.04f), glm::vec3(0), glm::vec3(0.06f, 0.12f, 0.06f));
}

// ==== CORRIDOR ====
void drawCorridor(Shader& lightingShader) {
    // Corridor runs IN FRONT of the building from Z = 0.0 to Z = 3.0
    // The old front wall at Z = -0.1 now has doors into each classroom
    // A new exterior front wall is at Z = 3.0

    float fullLeft  = -17.0f;
    float fullRight =  19.0f;
    float corrW     = fullRight - fullLeft;   // 36 units
    float corrX     = (fullLeft + fullRight) / 2.0f; // 1.0

    for (int floor = 0; floor <= 1; floor++) {
        float fY = floor * FL_H;

        // --- Corridor Floor ---
        drawCubeRotated(lightingShader, corrFloor,
            glm::vec3(corrX, fY + 0.12f, CORR_Z_CENTER),
            glm::vec3(0), glm::vec3(corrW, 0.05f, CORR_W));

        // --- Corridor Ceiling ---
        drawCubeRotated(lightingShader, ceilCol,
            glm::vec3(corrX, fY + FL_H - 0.12f, CORR_Z_CENTER),
            glm::vec3(0), glm::vec3(corrW, 0.05f, CORR_W));

        // --- New Exterior Front Wall at Z = 3.0 ---
        // Solid wall with windows
        float frontZ = CORR_Z_FRONT; // 3.0

        // --- Open Pillared Corridor Facade ---
        
        // Top continuous beam to support the roof
        float beamH = 0.8f;
        drawCubeRotated(lightingShader, extWallCol, glm::vec3(-10.0f, fY + FL_H - beamH/2.0f, frontZ), glm::vec3(0), glm::vec3(14.0f, beamH, 0.4f)); // Left Wing Rim
        drawCubeRotated(lightingShader, extWallCol, glm::vec3(11.0f, fY + FL_H - beamH/2.0f, frontZ), glm::vec3(0), glm::vec3(16.0f, beamH, 0.4f));  // Right Wing Rim

        // Elegant Standalone Pillars
        float pW = 0.6f, pD = 0.6f;
        // Left Wing Pillars (Span: -17 to -3)
        for (float px : {-16.7f, -13.3f, -10.0f, -6.6f, -3.3f}) {
            drawCubeRotated(lightingShader, pillarCol, glm::vec3(px, fY + FL_H/2.0f, frontZ), glm::vec3(0), glm::vec3(pW, FL_H, pD));
        }
        // Right Wing Pillars (Span: 3 to 19)
        for (float px : {3.3f, 7.15f, 11.0f, 14.85f, 18.7f}) {
            drawCubeRotated(lightingShader, pillarCol, glm::vec3(px, fY + FL_H/2.0f, frontZ), glm::vec3(0), glm::vec3(pW, FL_H, pD));
        }

        // Waist-high safety railing for the upper floor corridor
        if (floor == 1) {
            drawCubeRotated(lightingShader, extWallCol, glm::vec3(-10.0f, fY + 0.6f, frontZ), glm::vec3(0), glm::vec3(14.0f, 1.2f, 0.2f)); // Left Wing
            drawCubeRotated(lightingShader, extWallCol, glm::vec3(11.0f, fY + 0.6f, frontZ), glm::vec3(0), glm::vec3(16.0f, 1.2f, 0.2f));  // Right Wing
        }

        // --- Doors in old front wall (Z = -0.1) for each classroom ---
        float allDoors[] = {-14.5f, -10.0f, -5.5f, 5.5f, 10.0f, 14.5f};
        for (float dx : allDoors) {
            float aboveDoorH = FL_H - DOOR_H - 0.08f;
            float pZ = -0.1f;

            // Draw the door
            drawDoor(lightingShader, glm::vec3(dx, fY + DOOR_H/2.0f, pZ), DOOR_W, DOOR_H, true);

        }
    }
}
