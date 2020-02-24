#include <string>
#include <glm/glm.hpp>
#include <vector>
#include <cstring>

#include "MeshLoader.h"
#include "../Utils/Logger.h"

Mesh* LoadMesh(const char* path) {
    FILE *file = std::fopen(path, "r");
    if (file == NULL) {
        Logger::Error("LoadMesh - Could not read file path: " + std::string(path));
        return NULL;
    }

    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> verticesIndices;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> normalsIndices;
    std::vector<glm::vec2> uvs;
    std::vector<unsigned int> uvsIndices;

    unsigned int indicesCount = 0;
    char lineHeader[128];
    int res = fscanf(file, "%s", lineHeader);
    while (res != EOF) {
        if (strcmp(lineHeader, "v") == 0) { // POSITION
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            vertices.push_back(vertex);

        } else if (strcmp(lineHeader, "vt") == 0) { // UVs
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            uvs.push_back(uv);

        } else if (strcmp(lineHeader, "vn") == 0) { // NORMALS
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            normals.push_back(normal);
        }  else if (strcmp(lineHeader, "f") == 0) { // INDICES
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
            
            verticesIndices.push_back(vertexIndex[0]);
            verticesIndices.push_back(vertexIndex[1]);
            verticesIndices.push_back(vertexIndex[2]);
            normalsIndices.push_back(normalIndex[0]);
            normalsIndices.push_back(normalIndex[1]);
            normalsIndices.push_back(normalIndex[2]);
            uvsIndices.push_back(uvIndex[0]);
            uvsIndices.push_back(uvIndex[1]);
            uvsIndices.push_back(uvIndex[2]);
            indicesCount += 3;
        }
        res = fscanf(file, "%s", lineHeader);
    }
    std::fclose(file);

    // TODO: This method makes unnecessary copies of vertices even though they're being shared among multiple triangles.
    Vertex* finalMesh = new Vertex[verticesIndices.size()];
    for (unsigned int i = 0; i < verticesIndices.size(); i++) {
        unsigned int vertexIndex = verticesIndices[i] - 1;
        finalMesh[i].position = vertices[verticesIndices[i] - 1];
        finalMesh[i].normal = normals[normalsIndices[i] - 1];
        finalMesh[i].uv = uvs[uvsIndices[i] - 1];
    }
    for (unsigned int i = 0; i < verticesIndices.size(); i++) verticesIndices[i] = i;
    
    Mesh *mesh = new Mesh();
    mesh->Create((GLfloat*)finalMesh, verticesIndices.size(), &verticesIndices[0], verticesIndices.size());
    delete[] finalMesh;
    return mesh;
}