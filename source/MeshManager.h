#ifndef MESH_MANAGER_H
#define MESH_MANAGER_H

#include <unordered_map>
#include <string>
#include <optional>
#include <iostream>

#include "Mesh.h"

#include <QOpenGLExtraFunctions>


namespace gfx
{

class MeshManager
{
    public:
        MeshManager() = default;
        MeshManager(QOpenGLExtraFunctions* openGLFunctions);
        MeshManager(std::vector<MeshEntry>&& meshList, QOpenGLExtraFunctions* openGLFunctions);
        void addMesh(std::string meshName, MeshData&& meshData);

        Mesh* getMesh(std::string meshName);

        void printAllMeshNames();

    private:
        std::unordered_map<std::string, std::unique_ptr<Mesh>> m_meshes;
        QOpenGLExtraFunctions* m_openGLFunctions;
};

}

#endif