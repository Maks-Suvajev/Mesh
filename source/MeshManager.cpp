#include "Meshmanager.h"

namespace gfx
{


    MeshManager::MeshManager(std::vector<MeshEntry>&& meshList, QOpenGLExtraFunctions* openGLFunctions)
        : m_openGLFunctions(openGLFunctions)
    {
        for (auto& meshEntry : meshList)
        {
            addMesh(std::move(meshEntry.name), std::move(meshEntry.meshData));
        }

        printAllMeshNames();
    }

        MeshManager::MeshManager( QOpenGLExtraFunctions* openGLFunctions)
        : m_openGLFunctions(openGLFunctions)
    {
    }

    void MeshManager::addMesh(std::string meshName, MeshData&& meshData)
    {
        m_meshes[meshName] = std::make_unique<Mesh>(std::move(meshData), m_openGLFunctions);
    }
    
    Mesh* MeshManager::getMesh(std::string meshName)
    {
        if (m_meshes.contains(meshName))
        {
            return m_meshes[meshName].get();
        }

        #ifdef ENABLE_DEBUG_MESSAGES
            std::cout << "ERROR::Requested mesh name does not exist: " << meshName << std::endl;
        #endif

        return nullptr;
        
    }
    
    void MeshManager::printAllMeshNames()
    {
        uint32_t total_meshes = 0;

        std::cout << "-----------------------------------" << std::endl;
        std::cout << "Meshes currently loaded:           " << std::endl;
        std::cout << "-----------------------------------" << std::endl;

        for (auto& [key, mesh] : m_meshes)
        {
            std::cout << "Mesh name: " << key << std::endl;

            ++total_meshes;
        }

        std::cout << "-----------------------------------" << std::endl;

        std::cout << "There are a total of " << total_meshes << " meshes." << std::endl;
    }


}