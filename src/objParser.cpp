#include "objParser.h"

#include <fstream>
#include <stdexcept>
#include <sstream>
#include <iostream>
//#include <unordered_map>

/*struct colorLib {
    std::unordered_map<std::string, glm::vec3> diffColors;
};

int parseMaterials(std::string name, colorLib& cl) {
    std::ifstream mtlFile;
    mtlFile.open("../data/" + name + ".mtl");
    if (!mtlFile.is_open()) {
        return -1;
    }

    std::string line;

    std::string currMaterial = "";

    while (getline(mtlFile, line)){
        std::istringstream lineStream(line);

        std::string lineHeader;
        lineStream >> lineHeader;

        if (lineHeader == "newmtl"){
            lineStream >> currMaterial;
        }
        if (lineHeader == "Kd"){
            glm::vec3 diffuseColor;
            lineStream >> diffuseColor.x >> diffuseColor.y >> diffuseColor.z;

            cl.diffColors[currMaterial] = diffuseColor;
        }
    }
}*/

void parseFile(std::string name, std::vector<float>& outVect, std::vector<unsigned int>& outIndices, std::vector<float>& outNormalsVect, std::vector<unsigned int>& outNormalsIndices) {
    //colorLib cl;
    std::ifstream objFile;

    //parseMaterials(name, cl);
    objFile.open("../data/" + name + ".obj");
    if (!objFile.is_open()) {
        //throw an error
        throw std::invalid_argument("File \"" + name + "\" cannot be opened");
    }

    std::string line;

    std::vector<glm::vec3> verticies;
    //std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    std::vector<std::vector<int>> faces;
    std::vector<std::vector<int>> facesNormals;
    //glm::vec3 currColor = vec3(1.0f, 0.0f, 0.0f);
    
    while (getline(objFile, line)) {
        std::istringstream lineStream(line);

        //each line SHOULD start with a string describing whats on the line
        std::string lineHeader;
        lineStream >> lineHeader;
        
        //verticies eg "v -1.000000 -1.000000 1.000000"
        if (lineHeader == "v") {
            glm::vec3 v;
            lineStream >> v.x >> v.y >> v.z;
            verticies.push_back(v * glm::vec3(0.03f, 0.03f, 0.03f));
            float a = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            float c = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            
            glm::vec3 color = v;
            v = v * glm::vec3(0.02f, 0.02f, 0.02f);

            outVect.insert(outVect.end(), {v.x, v.y, v.z, a, b, c});
        }
        /*
        //vertex textures eg "vt -1.000000 -1.000000 1.000000"
        else if (lineHeader == "vt") {
            glm::vec2 uv;
            lineStream >> uv.x >> uv.y;
            uvs.push_back(uv);
        }*/
        //vertex normals eg "vn -1.000000 -1.000000 1.000000"
        else if (lineHeader == "vn") {
            glm::vec3 normal;
            lineStream >> normal.x >> normal.y >> normal.z;
            outNormalsVect.insert(outNormalsVect.end(), {normal.x, normal.y, normal.z});
        }
        
        //faces eg "f 4//4 3//3 2//2 1//1 ..." or "f 4/4/4 3/3/3 2/2/2 1/1/1 ..."
        else if (lineHeader == "f") {
            std::vector<int> face;
            std::vector<int> normal;
            std::string faceIndex;
            while (lineStream >> faceIndex) {
                //push back the first int in this string before the /
                face.push_back(std::stoi(faceIndex.substr(0, faceIndex.find('/'))) - 1);
                
                //find the last int 
                std::string afterFirstSlash = faceIndex.substr(faceIndex.find('/') + 1, faceIndex.size());
                if(afterFirstSlash.size() > 1) {
                    normal.push_back(std::stoi(afterFirstSlash.substr(afterFirstSlash.find('/') + 1, faceIndex.size())));
                }

            }
            faces.push_back(face);
            facesNormals.push_back(normal);
        }
    }
    

    //now process the verticies by faces 
    for (unsigned int x = 0; x < faces.size(); x++) {
        std::vector<int> face = faces[x];
        std::vector<int> faceNormal;

        if (facesNormals.size() > 0){
            faceNormal = facesNormals[x];
        }

        for (unsigned int i = 1; i < face.size()-1; i++) {
            outIndices.push_back(face[0]);
            if (faceNormal.size() > 0){
                outNormalsIndices.push_back(faceNormal[0]);
            }

            for (unsigned int j = 0; j < 2; j++){
                outIndices.push_back(face[i+j]);
                if (faceNormal.size() > i+j) {
                    outNormalsIndices.push_back(faceNormal[i+j]);
                }
            }
        }
    }
    return;
};
