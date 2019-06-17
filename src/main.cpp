#include <stdio.h>
#include <chrono>

#include <GLFW/glfw3.h>
#include <bgfx/bgfx.h>
#include <bx/math.h>

#include <portable-file-dialogs.h>
#include "graphics.h"
#include "modelLoader.h"


static bool s_showStats = false;

static void glfw_errorCallback(int error, const char *description)
{
    fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

static void glfw_keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_F1 && action == GLFW_RELEASE)
        s_showStats = !s_showStats;
}

int main(int argc, char **argv)
{
    auto fileRequest = pfd::open_file("Choose files to read", "/tmp/",
        { "Obj Files (.obj)", "*.obj",
          "All Files", "*" },
        false);

    auto fileName = fileRequest.result()[0];
    printf(fileName.c_str());

    // Create a GLFW window without an OpenGL context.
    glfwSetErrorCallback(glfw_errorCallback);
    if (!glfwInit())
    {
        return 1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow *window = glfwCreateWindow(1024, 768, "helloworld", nullptr, nullptr);
    if (!window)
    {
        return 1;
    }

    glfwSetKeyCallback(window, glfw_keyCallback);

    if (!graphics::init(window))
    {
        return 1;
    }

    std::vector<Mesh> meshes;
    modelLoader::loadModel(fileName, meshes);

    std::vector<Element> elements;
    for (auto& mesh : meshes)
    {
        Element element;
        element.mesh = mesh;
        bx::mtxIdentity(element.transform);
        bx::mtxScale(element.transform, 1.0);

        elements.push_back(element);
    }

    unsigned int counter = 0;

    auto lastFrame = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(window)) 
    {
        glfwPollEvents();

        // Enable stats or debug text.
        bgfx::setDebug(s_showStats ? BGFX_DEBUG_STATS : BGFX_DEBUG_TEXT);

        auto now = std::chrono::high_resolution_clock::now();
        auto delta = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(now - lastFrame).count() /
            1000.0f;
        lastFrame = now;

        graphics::renderElements(elements);
        graphics::renderFrame();

        counter++;
    }

    graphics::shutdown();
    glfwTerminate();
}