#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imfilebrowser.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include <shader.h>
#include <camera.hpp>
#include <mesh.hpp>
#include <object_reader.hpp>
#include <renderer.hpp>
#include <resource_manager.h>
#include <texture.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void mouseCursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void markMesh(GLFWwindow* window, int meshIndex);

// Settings
const unsigned int SCR_WIDTH = 1366;
const unsigned int SCR_HEIGHT = 768;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

std::vector<Object3D*> objects;

std::set<int> selectedIndexes;
Object3D* selectedObject = nullptr;

int main() {
    // GLFW: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // GLFW window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Model Viewer", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCursorPosCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetScrollCallback(window, mouseScrollCallback);
    // Tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    // GLAD: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // Configure global opengl state
    glEnable(GL_DEPTH_TEST);

    // -------------------------------------------------------------------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsClassic();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGui::FileBrowser fileDialog;
    fileDialog.SetTypeFilters({ ".obj" });

    // -------------------------------------------------------------------
    Renderer renderer(glm::vec2(SCR_WIDTH, SCR_HEIGHT), camera);
    ObjectReader objReader;

    // -------------------------------------------------------------------
    // Render loop
    // -------------------------------------------------------------------
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        // --------------------------------------------------------------
        glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Mesh rendering
        for (int x = 0; x < objects.size(); x++) {
            int renderModes = RenderModes_Normal;
            if (selectedIndexes.find(x) != selectedIndexes.end()) { // mesh selected
                renderModes |= RenderModes_Wireframe;
            }
            renderer.render(*objects[x], renderModes);
        }

        // --------------------------------------------------------------
        // Mesh selection window
        ImGui::Begin("Meshes", (bool*)0, ImGuiWindowFlags_AlwaysAutoResize);
            // Mesh loader button
            if (ImGui::Button("Import object"))
                fileDialog.Open();
            fileDialog.Display();
            if (fileDialog.HasSelected()) {
                for (Object3D* obj : objReader.readModel(fileDialog.GetSelected().string().c_str()))
                    objects.push_back(obj);
                fileDialog.ClearSelected();
            }
            // List of meshes in scene
            if (ImGui::BeginListBox("##meshes-list")) {
                for (int i = 0; i < objects.size(); i++) {
                    std::string originalMeshName = objects[i]->mesh.getName();
                    std::string meshName = originalMeshName.empty() ? "mesh_" + std::to_string(i) : originalMeshName;
                    bool meshSelected = selectedIndexes.find(i) != selectedIndexes.end();
                    if (ImGui::Selectable(meshName.c_str(), meshSelected)) {
                        markMesh(window, i);
                    }
                }
                ImGui::EndListBox();
            }
        ImGui::End();

        // --------------------------------------------------------------
        // Transformations window
        if (selectedObject) {
            ImGui::Begin("Transform", (bool*)0, ImGuiWindowFlags_AlwaysAutoResize);
            // Position
            ImGui::Text("Position");
            ImGui::DragScalar("X##position_x", ImGuiDataType_Float, &selectedObject->position.x, 0.05f);
            ImGui::DragScalar("Y##position_y", ImGuiDataType_Float, &selectedObject->position.y, 0.05f);
            ImGui::DragScalar("Z##position_z", ImGuiDataType_Float, &selectedObject->position.z, 0.05f);
            ImGui::Separator();
            // Rotation
            ImGui::Text("Rotation");
            ImGui::SliderAngle("X##rotation_x", &selectedObject->rotation.x, 0.0f, 360.0f);
            ImGui::SliderAngle("Y##rotation_y", &selectedObject->rotation.y, 0.0f, 360.0f);
            ImGui::SliderAngle("Z##rotation_z", &selectedObject->rotation.z, 0.0f, 360.0f);
            ImGui::Separator();
            // Scale
            ImGui::Text("Scale");
            ImGui::DragScalar("X##scale_x", ImGuiDataType_Float, &selectedObject->scale.x, 0.05f);
            ImGui::DragScalar("Y##scale_y", ImGuiDataType_Float, &selectedObject->scale.y, 0.05f);
            ImGui::DragScalar("Z##scale_z", ImGuiDataType_Float, &selectedObject->scale.z, 0.05f);
            ImGui::End();
        }

        // --------------------------------------------------------------
        // Render windows
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // --------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // ------------------------------------------------------------------
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // GLFW: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window) {
    float speed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? 5.0f : 1.0f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.moveForward(deltaTime * speed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.moveBackward(deltaTime * speed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.moveLeft(deltaTime * speed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.moveRight(deltaTime * speed);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.moveUp(deltaTime * speed);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.moveDown(deltaTime * speed);
}

// Möller–Trumbore intersection algorithm
static bool rayIntersectsTriangle(const glm::vec3& origin, const glm::vec3& dir, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float* intersection) {
    const float epsilon = 0.000001f;

    glm::vec3 e1 = v1 - v0;
    glm::vec3 e2 = v2 - v0;

    glm::vec3 h = glm::cross(dir, e2);
    float a = glm::dot(e1, h);

    if (a > -epsilon && a < epsilon) {
        return false; // Ray is parallel to the triangle
    }

    float f = 1.0f / a;
    glm::vec3 s = origin - v0;
    float u = f * glm::dot(s, h);

    if (u < 0.0f || u > 1.0f) {
        return false; // Intersection point is outside the triangle
    }

    glm::vec3 q = glm::cross(s, e1);
    float v = f * glm::dot(dir, q);

    if (v < 0.0f || u + v > 1.0f) {
        return false; // Intersection point is outside the triangle
    }

    float t = f * glm::dot(e2, q);

    if (t > epsilon) {
        if (intersection) {
            *intersection = t;
        }
        return true; // Intersection point is valid
    }

    return false; // No intersection
}

/*
* Processes the selection of a mesh
*
* If the multiple selection key is pressed it will select the mesh if it is not selected
* or deselect it if it is already selected.
* Otherwise, it will only mark the selected mesh.
*/
void markMesh(GLFWwindow* window, int meshIndex) {
    bool multipleSelection = glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS;
    if (multipleSelection) {
        auto it = selectedIndexes.find(meshIndex);
        if (it != selectedIndexes.end()) {
            selectedIndexes.erase(it);
        } else {
            selectedIndexes.insert(meshIndex);
        }
    }
    else {
        selectedIndexes.clear();
        selectedIndexes.insert(meshIndex);
    }
    selectedObject = (selectedIndexes.size() == 1) ? objects[*selectedIndexes.begin()] : nullptr;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouseCursorPosCallback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT))
        camera.processMouseMovement(xoffset, yoffset);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (ImGui::GetIO().WantCaptureMouse || button != GLFW_MOUSE_BUTTON_LEFT || action != GLFW_PRESS) {
        return;
    }

    double cursorX, cursorY;
    glfwGetCursorPos(window, &cursorX, &cursorY);
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    glm::vec4 viewport = glm::vec4(0.0f, 0.0f, windowWidth, windowHeight);
    glm::mat4 projection = glm::perspective(glm::radians(camera.cameraZoom), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
    glm::mat4 view = camera.getViewMatrix();

    float closestIntersection = std::numeric_limits<float>::max();
    int closestIntersectionIndex = -1;

    for (int x = 0; x < objects.size(); ++x) {
        glm::mat4 model = objects[x]->getModelMatrix();

        glm::vec3 worldNear = glm::unProject(glm::vec3(float(cursorX), float(windowHeight - cursorY), 0.0f), view * model, projection, viewport);
        glm::vec3 worldFar = glm::unProject(glm::vec3(float(cursorX), float(windowHeight - cursorY), 1.0f), view * model, projection, viewport);
        glm::vec3 rayDir = glm::normalize(worldFar - worldNear);

        Mesh mesh = objects[x]->mesh;
        std::vector<glm::vec3> verticesData = mesh.getVertices();
        std::vector<GLuint> indices = mesh.getIndices();

        for (uint32_t i = 0; i < indices.size(); i += 3) {
            float intersectionPos;
            if (rayIntersectsTriangle(worldNear, rayDir, verticesData[indices[i]], verticesData[indices[i + 1]], verticesData[indices[i + 2]], &intersectionPos)) {
                if (intersectionPos < closestIntersection) {
                    closestIntersection = intersectionPos;
                    closestIntersectionIndex = x;
                }
            }
        }
    }

    if (closestIntersectionIndex != -1) {
        markMesh(window, closestIntersectionIndex);
    } else {
        selectedIndexes.clear();
        selectedObject = nullptr;
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.zoom(static_cast<float>(yoffset));
}