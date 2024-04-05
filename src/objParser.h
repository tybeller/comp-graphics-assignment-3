#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

void parseFile(std::string name, std::vector<float>& outVect, std::vector<unsigned int>& outIndices, std::vector<float>& outNormalsVect, std::vector<unsigned int>& outNormalsIndices);
