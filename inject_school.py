import os
import re

main_cpp_path = r"c:\Users\USER\OneDrive\Desktop\4-2\LAB\Graphics\smart_village_shrawosy\main.cpp"
school_code_path = r"c:\Users\USER\OneDrive\Desktop\4-2\LAB\Graphics\smart_village_shrawosy\school_code_snippet.txt"

with open(main_cpp_path, "r", encoding="utf-8") as f:
    main_code = f.read()

with open(school_code_path, "r", encoding="utf-8") as f:
    school_code = f.read()

# 1. Constants
constants = """
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

"""
main_code = main_code.replace("bool pKeyPressed = false;", "bool pKeyPressed = false;\n" + constants)

# 2. Prototypes
prototypes = """
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
"""
main_code = main_code.replace("void drawSchool(Shader& shader);", "// void drawSchool(Shader& shader);\n" + prototypes)

# 3. Call the school in main loop
main_code = main_code.replace("// drawSchool(shader);", "drawRealisticSchool(shader);")

# 4. Inputs
inputs = """
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && !pKeyPressed) {
        doorOpen = !doorOpen;
        pKeyPressed = true;
        std::cout << ">> Doors: " << (doorOpen ? "OPEN" : "CLOSED") << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE) pKeyPressed = false;

    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && !oKeyPressed) {
        windowsOpen = !windowsOpen;
        oKeyPressed = true;
        std::cout << ">> Windows: " << (windowsOpen ? "OPEN" : "CLOSED") << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_RELEASE) oKeyPressed = false;
"""
main_code = main_code.replace("if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !bKeyPressed)", inputs + "\n    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !bKeyPressed)")

# 5. Adapt the school code snippet
school_code = re.sub(r'void drawRealisticSchool\(unsigned int& cubeVAO, Shader& lightingShader, Cylinder& basicCylinder\)', 'void drawRealisticSchool(Shader& lightingShader)', school_code)
school_code = re.sub(r'drawLeftWing\(cubeVAO, lightingShader\);', 'drawLeftWing(lightingShader);', school_code)
school_code = re.sub(r'drawCenterEntrance\(cubeVAO, lightingShader, basicCylinder\);', 'drawCenterEntrance(lightingShader);', school_code)
school_code = re.sub(r'drawRightWing\(cubeVAO, lightingShader\);', 'drawRightWing(lightingShader);', school_code)
school_code = re.sub(r'drawCorridor\(cubeVAO, lightingShader\);', 'drawCorridor(lightingShader);', school_code)
school_code = re.sub(r'drawFlagpole\(cubeVAO, lightingShader, basicCylinder\);', 'drawFlagpole(lightingShader);', school_code)

school_code = re.sub(r'void drawWindow\(unsigned int& cubeVAO, Shader& lightingShader', 'void drawWindow(Shader& lightingShader', school_code)
school_code = re.sub(r'drawWindow\(cubeVAO, ', 'drawWindow(', school_code)

school_code = re.sub(r'void drawLeftWing\(unsigned int& cubeVAO, Shader& lightingShader', 'void drawLeftWing(Shader& lightingShader', school_code)
school_code = re.sub(r'void drawCenterEntrance\(unsigned int& cubeVAO, Shader& lightingShader, Cylinder& basicCylinder\)', 'void drawCenterEntrance(Shader& lightingShader)', school_code)
school_code = re.sub(r'void drawRightWing\(unsigned int& cubeVAO, Shader& lightingShader\)', 'void drawRightWing(Shader& lightingShader)', school_code)
school_code = re.sub(r'void drawFlagpole\(unsigned int& cubeVAO, Shader& lightingShader, Cylinder& basicCylinder\)', 'void drawFlagpole(Shader& lightingShader)', school_code)
school_code = re.sub(r'void drawClassroomInterior\(unsigned int& cubeVAO, Shader& lightingShader', 'void drawClassroomInterior(Shader& lightingShader', school_code)
school_code = re.sub(r'drawClassroomInterior\(cubeVAO, ', 'drawClassroomInterior(', school_code)
school_code = re.sub(r'void drawDesk\(unsigned int& cubeVAO, Shader& lightingShader', 'void drawDesk(Shader& lightingShader', school_code)
school_code = re.sub(r'drawDesk\(cubeVAO, ', 'drawDesk(', school_code)

school_code = re.sub(r'void drawDoor\(unsigned int& cubeVAO, Shader& lightingShader', 'void drawDoor(Shader& lightingShader', school_code)
school_code = re.sub(r'drawDoor\(cubeVAO, ', 'drawDoor(', school_code)
school_code = re.sub(r'void drawCorridor\(unsigned int& cubeVAO, Shader& lightingShader\)', 'void drawCorridor(Shader& lightingShader)', school_code)

school_code = re.sub(r'drawCube\(cubeVAO, ', 'drawCubeRotated(', school_code)

school_code = re.sub(r'basicCylinder\.drawCylinder\(lightingShader, ([^,]+), ([^)]+)\);', r'drawCylinderModel(lightingShader, \1, \2, 16.0f, false);', school_code)
# In drawFlagpole there's a typo from the regex: nm
school_code = re.sub(r'drawCubeRotated\(lightingShader, flagGreen,', 'unsigned int dummyVAO = 0; // Not used\n    drawCubeRotated(lightingShader, flagGreen,', school_code)

# 6. Append helper function and the adapted school code to main_code
helper = """
void drawCubeRotated(Shader& lightingShader, glm::vec3 color, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale) {
    glm::mat4 m = glm::mat4(1.0f);
    m = glm::translate(m, pos);
    m = glm::rotate(m, glm::radians(rot[0]), glm::vec3(1,0,0));
    m = glm::rotate(m, glm::radians(rot[1]), glm::vec3(0,1,0));
    m = glm::rotate(m, glm::radians(rot[2]), glm::vec3(0,0,1));
    m = glm::scale(m, scale);
    
    // Bind default texture so color matters over any existing texture bound
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texDefault);
    
    drawObject(lightingShader, cubeVAO, 36, m, color, 16.0f, false);
}

"""

main_code = main_code + "\n\n" + helper + school_code

with open(main_cpp_path, "w", encoding="utf-8") as f:
    f.write(main_code)

print("SUCCESS: Code replaced successfully in main.cpp")
