#pragma once
#include <bgfx/bgfx.h>
#include <vector>

struct GLFWwindow;

struct Mesh
{
    bgfx::ProgramHandle program;
    bgfx::VertexBufferHandle vertexBuffer;
    bgfx::IndexBufferHandle indexBuffer;
};

struct Color
{
    float r;
    float g;
    float b;
    float a;
};


struct Element
{
    float transform[16];
    Color color;
    Mesh mesh;
};

struct PosNormalVertex
{
    float x;
    float y;
    float z;
    uint32_t normal;
};

namespace graphics
{
    static const bgfx::ViewId kClearView = 0;
    static int windowWidth;
    static int windowHeight;
    static bgfx::UniformHandle kColorUniform;
    static bgfx::UniformHandle kInvertedModelUniform;
    static bgfx::UniformHandle kViewPosUniform;

    bool init(GLFWwindow* window);
    void renderFrame();
    void shutdown();

    bgfx::ShaderHandle loadShader(const char *FILENAME);

    void renderElements(std::vector<Element>& elements);

    Mesh createCubeMesh();
}