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
#include <fstream>

#include <camera.hpp>
#include <font.h>
#include <mesh.hpp>
#include <renderer.hpp>
#include <resource_manager.h>
#include <scene.hpp>
#include <shader.h>
#include <texture.h>
#include <text_renderer.h>
#include <transformable_group.hpp>

using namespace std;

void processInput(GLFWwindow* window);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void mouseCursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
static bool rayIntersectsTriangle(const glm::vec3& origin, const glm::vec3& dir, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float* intersection);
void markMesh(GLFWwindow* window, int meshIndex);
void deleteSelectedObjects();
Object3D* getSelectedObject();

// Settings
const unsigned int SCR_WIDTH = 1366;
const unsigned int SCR_HEIGHT = 768;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Camera
Camera camera(glm::vec3(0.0f, 1.0f, 4.0f));

// Scene
Scene scene;

// Selected objects
TransformableGroup selectedObjects;

// Timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
float animationLastFrame = 0.0f;

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
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCursorPosCallback);
    glfwSetScrollCallback(window, mouseScrollCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    // GLAD: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // Configure global opengl state
    glEnable(GL_DEPTH_TEST);
    // ImGUI: initialize and configure
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsClassic();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // -------------------------------------------------------------------
    // File browser
    ImGui::FileBrowser fileDialog;
    fileDialog.SetTypeFilters({ ".obj", ".json"});
    // Object renderer
    Renderer renderer(glm::vec2(SCR_WIDTH, SCR_HEIGHT), camera, scene.light);
    // Object reader
    ObjectReader objReader;
    // Text renderer
    TextRenderer textRenderer(SCR_WIDTH, SCR_HEIGHT, Font("assets/fonts/Gobold Regular.otf", 11));
    textRenderer.setHorizontalAlignment(TextLeft);
    textRenderer.setColor(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

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

        // Info
        textRenderer.renderText("Controls:", 10.0f, 10.0f);
        textRenderer.renderText("[AWSD] Movement", 10.0f, 25.0f);
        textRenderer.renderText("[Space] Up", 10.0f, 40.0f);
        textRenderer.renderText("[Ctrl] Down", 10.0f, 55.0f);
        textRenderer.renderText("[Alt] Multiple selection", 10.0f, 70.0f);
        textRenderer.renderText("[Right click] Camera", 10.0f, 85.0f);

        // Object rendering
        for (int x = 0; x < scene.objects.size(); x++) {
            int renderModes = RenderModes_Normal;
            if (selectedObjects.contains(x)) {
                renderModes |= RenderModes_Wireframe;
            }
            renderer.render(*scene.objects[x], renderModes);
        }

        // Animation
        if (scene.animations.size() > 0) {
            for (int x = 0; x < scene.animations.size(); x++) {
                scene.animations[x].animate(currentFrame);
			}
		}

        // --------------------------------------------------------------
        // Object selection window
        ImGui::Begin("Objects", (bool*)0, ImGuiWindowFlags_AlwaysAutoResize);

            // Import object/scene button
            if (ImGui::Button("Import")) {
                fileDialog.Open();
            }
            fileDialog.Display();
            if (fileDialog.HasSelected()) {
                // Object import
                if (fileDialog.GetSelected().extension().string() == ".obj") {
                    for (Object3D* obj : objReader.readModel(fileDialog.GetSelected().string().c_str()))
                        scene.objects.push_back(obj);
                // JSON scene import
                } else {
                    scene.parse(fileDialog.GetSelected().string().c_str());
                }
                fileDialog.ClearSelected();
            }

            // Object remove button
            ImGui::SameLine();
            if (ImGui::Button("Delete selected")) {
                deleteSelectedObjects();
            }

            // List of meshes in scene
            if (ImGui::BeginListBox("##meshes-list", ImVec2(300.0f, 200.0f))) {
                for (int i = 0; i < scene.objects.size(); i++) {
                    std::string originalMeshName = scene.objects[i]->mesh.getName();
                    std::string meshName = originalMeshName.empty() ? "mesh_" + std::to_string(i) : originalMeshName;
                    if (ImGui::Selectable(meshName.c_str(), selectedObjects.contains(i))) {
                        markMesh(window, i);
                    }
                }
                ImGui::EndListBox();
            }
        ImGui::End();

        // --------------------------------------------------------------
        // Transformations window
        if (!selectedObjects.empty()) {
            ImGui::Begin("Transform", (bool*)0, ImGuiWindowFlags_AlwaysAutoResize);
            // Position
            ImGui::Text("Position");
            ImGui::DragScalar("X##position_x", ImGuiDataType_Float, &selectedObjects.position.x, 0.01f);
            ImGui::DragScalar("Y##position_y", ImGuiDataType_Float, &selectedObjects.position.y, 0.01f);
            ImGui::DragScalar("Z##position_z", ImGuiDataType_Float, &selectedObjects.position.z, 0.01f);
            ImGui::Separator();
            // Rotation
            ImGui::Text("Rotation");
            ImGui::SliderAngle("X##rotation_x", &selectedObjects.rotation.x, -360.0f, 360.0f);
            ImGui::SliderAngle("Y##rotation_y", &selectedObjects.rotation.y, -360.0f, 360.0f);
            ImGui::SliderAngle("Z##rotation_z", &selectedObjects.rotation.z, -360.0f, 360.0f);
            ImGui::Separator();
            // Scale
            ImGui::Text("Scale");
            ImGui::DragScalar("X##scale_x", ImGuiDataType_Float, &selectedObjects.scale.x, 0.01f);
            ImGui::DragScalar("Y##scale_y", ImGuiDataType_Float, &selectedObjects.scale.y, 0.01f);
            ImGui::DragScalar("Z##scale_z", ImGuiDataType_Float, &selectedObjects.scale.z, 0.01f);
            ImGui::End();
            // Update selected objects attributes
            selectedObjects.update();
        }

        // --------------------------------------------------------------
        // Lightning window
        ImGui::Begin("Lightning", (bool*)0, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Position");
        ImGui::DragScalar("X##light_position_x", ImGuiDataType_Float, &scene.light.position.x, 0.01f);
        ImGui::DragScalar("Y##light_position_y", ImGuiDataType_Float, &scene.light.position.y, 0.01f);
        ImGui::DragScalar("Z##light_position_z", ImGuiDataType_Float, &scene.light.position.z, 0.01f);
        ImGui::Text("Color");
        ImGui::ColorEdit3("##light_color", (float*)&scene.light.color);
        ImGui::Text("Phong");
        ImGui::DragScalar("Ambient##ambient_strength", ImGuiDataType_Float, &scene.light.ambientStrength, 0.01f);
        ImGui::DragScalar("Diffuse##diffuse_strength", ImGuiDataType_Float, &scene.light.diffuseStrength, 0.01f);
        ImGui::DragScalar("Specular##specular_strength", ImGuiDataType_Float, &scene.light.specularStrength, 0.01f);
        ImGui::End();

        // --------------------------------------------------------------
        // Material window
        if (selectedObjects.size() == 1) {
            Material* material = &getSelectedObject()->mesh.getMaterial();
            ImGui::Begin("Material", (bool*)0, ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::ColorEdit3("Ambient##material_ambient", (float*)&material->ambientColor);
            ImGui::ColorEdit3("Diffuse##material_diffuse", (float*)&material->diffuseColor);
            ImGui::ColorEdit3("Specular##material_specular", (float*)&material->specularColor);
            ImGui::DragScalar("Shininess##material_shininess", ImGuiDataType_Float, &material->shininess, 0.01f);
            ImGui::DragScalar("Opacity##material_opacity", ImGuiDataType_Float, &material->opacity, 0.01f);
            ImGui::End();
        }

        // --------------------------------------------------------------
        // Render windows
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        // OpenGL stuff
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // ------------------------------------------------------------------
    // Clear allocated resources.
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}

// ================================================================================================
// Callbacks
// ================================================================================================

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

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

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

    for (int x = 0; x < scene.objects.size(); ++x) {
        glm::mat4 model = scene.objects[x]->getModelMatrix();

        glm::vec3 worldNear = glm::unProject(glm::vec3(float(cursorX), float(windowHeight - cursorY), 0.0f), view * model, projection, viewport);
        glm::vec3 worldFar = glm::unProject(glm::vec3(float(cursorX), float(windowHeight - cursorY), 1.0f), view * model, projection, viewport);
        glm::vec3 rayDir = glm::normalize(worldFar - worldNear);

        Mesh mesh = scene.objects[x]->mesh;
        std::vector<glm::vec3> verticesData = mesh.getVertices();
        std::vector<GLuint> indices = mesh.getIndices();

        for (uint32_t i = 0; i < indices.size(); i += 3) {
            if (i + 3 >= indices.size())
                break;
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
        selectedObjects.clear();
    }
}

void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.zoom(static_cast<float>(yoffset));
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Exit application
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    // Remove selected meshes
    if (key == GLFW_KEY_DELETE && action == GLFW_PRESS) {
        deleteSelectedObjects();
    }
    // Camera positions
    float distance = 15.0f;
    if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
        camera.position = glm::vec3(0.0f, 0.0f, distance);
        camera.yaw = -90.0f;
        camera.pitch = 0.0f;
        camera.cameraZoom = 45.0f;
        camera.updateCameraVectors();
    }
    if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
        camera.position = glm::vec3(distance, 0.0f, 0.0f);
        camera.yaw = 180.0f;
        camera.pitch = 0.0f;
        camera.cameraZoom = 45.0f;
        camera.updateCameraVectors();
    }
    if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
        camera.position = glm::vec3(0.0f, 0.0f, -distance);
        camera.yaw = 90.0f;
        camera.pitch = 0.0f;
        camera.cameraZoom = 45.0f;
        camera.updateCameraVectors();
    }
    if (key == GLFW_KEY_4 && action == GLFW_PRESS) {
        camera.position = glm::vec3(-distance, 0.0f, 0.0f);
        camera.yaw = 0.0f;
        camera.pitch = 0.0f;
        camera.cameraZoom = 45.0f;
        camera.updateCameraVectors();
    }
    if (key == GLFW_KEY_5 && action == GLFW_PRESS) {
        camera.position = glm::vec3(0.0f, distance, 0.0f);
        camera.yaw = -90.0f;
        camera.pitch = -90.0f;
        camera.cameraZoom = 45.0f;
        camera.updateCameraVectors();
    }
}

// ================================================================================================
// Internal functions
// ================================================================================================

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
        if (selectedObjects.contains(meshIndex)) {
            selectedObjects.remove(meshIndex);
        }
        else {
            selectedObjects.add(meshIndex, scene.objects[meshIndex]);
        }
    }
    else {
        selectedObjects.clear();
        selectedObjects.add(meshIndex, scene.objects[meshIndex]);
    }
}

// Delete all selected meshes
void deleteSelectedObjects() {
    scene.objects.erase(std::remove_if(scene.objects.begin(), scene.objects.end(), [&](Object3D* objPtr) {
        int index = std::distance(scene.objects.begin(), std::find(scene.objects.begin(), scene.objects.end(), objPtr));
        return selectedObjects.contains(index);
        }), scene.objects.end());
    selectedObjects.clear();
}

// Get the first selected object
Object3D* getSelectedObject() {
    for (int x = 0; x < scene.objects.size(); x++) {
		if (selectedObjects.contains(x)) {
			return scene.objects[x];
		}
	}
}