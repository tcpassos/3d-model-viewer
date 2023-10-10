#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// Settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

std::vector<Object3D*> objects;

Object3D* selectedObject = nullptr;
int selectedObjectIndex = -1;

int main() {
    // GLFW: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // GLFW window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    // Tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    // GLAD: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // Configure global opengl state
    glEnable(GL_DEPTH_TEST);

    // --------------------------------------------------------------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsClassic();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // --------------------------------------------------------------
    Renderer renderer(glm::vec2(SCR_WIDTH, SCR_HEIGHT), camera);
    ObjectReader objReader;
    objects = objReader.readModel("assets/obj/madara/madara.obj");

    // --------------------------------------------------------------
    // Render loop
    // --------------------------------------------------------------
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

        for (int x = 0; x < objects.size(); x++) {
            renderer.render(*objects[x], x == selectedObjectIndex);
        }

        // Mesh selection window
        ImGui::Begin("Meshes", (bool*)0, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::BeginListBox("##meshes-list");
            for (int i = 0; i < objects.size(); i++) {
                std::string meshName = "mesh_" + std::to_string(i);
                if (ImGui::Selectable(meshName.c_str(), i == selectedObjectIndex)) {
                    selectedObject = objects[i];
                    selectedObjectIndex = i;
                }
            }
        ImGui::EndListBox();
        ImGui::End();

        if (selectedObject) {
            // Transformations window
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

// Möller–Trumbore intersection algorithm
static int rayIntersectsTriangle(glm::vec3 origin, glm::vec3 dir, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, float* intersection)
{
    // Triangle edges
    glm::vec3 e1(v1 - v0);
    glm::vec3 e2(v2 - v0);

    const float epsilon = 0.000001f;

    glm::vec3 P, Q;
    float i;
    float t;

    // Calculate determinant
    P = glm::cross(dir, e2);
    float det = glm::dot(e1, P);
    // If determinant is (close to) zero, the ray lies in the plane of the triangle or parallel it's plane
    if ((det > -epsilon) && (det < epsilon))
    {
        return 0;
    }
    float invDet = 1.0f / det;

    // Distance from first vertex to ray origin
    glm::vec3 T = origin - v0;

    // Calculate u parameter
    float u = glm::dot(T, P) * invDet;
    // Intersection point lies outside of the triangle
    if ((u < 0.0f) || (u > 1.0f))
    {
        return 0;
    }

    //Prepare to test v parameter
    Q = glm::cross(T, e1);

    // Calculate v parameter
    float v = glm::dot(dir, Q) * invDet;
    // Intersection point lies outside of the triangle
    if (v < 0.f || u + v  > 1.f) return 0;

    // Calculate t
    t = glm::dot(e2, Q) * invDet;

    if (t > epsilon)
    {
        // Triangle interesected
        if (intersection)
        {
            *intersection = t;
        }
        return true;
    }

    // No intersection
    return false;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        selectedObject = nullptr;
        selectedObjectIndex = -1;
    }

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

    int w, h;
    double mx, my;
    uint32_t numIntersections = 0;

    glfwGetCursorPos(window, &mx, &my);
    glfwGetWindowSize(window, &w, &h);

    struct
    {
        int32_t index = -1;
        float lastPos = std::numeric_limits<float>::max();
    } intersection;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {

        glm::vec4 viewport = glm::vec4(0.0f, 0.0f, w, h);
        glm::mat4 projection = glm::perspective(glm::radians(camera.cameraZoom), (float)w / (float)h, 0.1f, 100.0f);
        glm::mat4 view = camera.getViewMatrix();

        for (int x = 0; x < objects.size(); x++) {

            glm::mat4 model = objects[x]->getModelMatrix();

            // Mouse world pos on near plane
            glm::vec3 worldNear = glm::unProject(glm::vec3(float(mx), float(h - my), 0.0f), view * model, projection, viewport);
            // Mouse world pos on far plane
            glm::vec3 worldFar = glm::unProject(glm::vec3(float(mx), float(h - my), 1.0f), view * model, projection, viewport);

            // Get ray between pos on near and far plane
            glm::vec3 rayDir = glm::normalize(worldFar - worldNear);

            Mesh mesh = objects[x]->getMesh();

            std::vector<glm::vec3> verticesData = mesh.getVertices();
            std::vector<GLuint> indices = mesh.getIndices();

            for (uint32_t i = 0; i < indices.size(); i+=3)
            {   
                float currIntersectionPos;
                if (rayIntersectsTriangle(worldNear, rayDir, verticesData[indices[i]], verticesData[indices[i + 1]], verticesData[indices[i + 2]], &currIntersectionPos))
                {
                    if (currIntersectionPos < intersection.lastPos)
                    {
                        intersection.lastPos = currIntersectionPos;
                        intersection.index = i;
                    }
                    numIntersections++;
                    selectedObject = objects[x];
                    selectedObjectIndex = x;
                }
            }

        }

    }

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
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

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.zoom(static_cast<float>(yoffset));
}