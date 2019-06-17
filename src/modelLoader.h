#pragma once
#include <vector>

struct Mesh;

namespace modelLoader
{
    void loadModel(std::string modelPath, std::vector<Mesh>& elements);
}
