#ifndef SCENE_MODEL_MANAGER_H
#define SCENE_MODEL_MANAGER_H

// STL
#include <unordered_map>
#include <string>
#include <optional>
#include <iostream>

// Custom Libs
#include "Shader.h"
#include "ResourceManager.h"
#include "TextureManager.h"
#include "MaterialManager.h"

#include "TextureTypes.h"
#include "MaterialTypes.h"

#include "EntityFactory.h"

// External Lib
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <QOpenGLExtraFunctions>


namespace gfx
{

class SceneModelManager : public ResourceManager<SceneModel>
{
    public:
        SceneModelManager() = default;
        SceneModelManager(AssetRegistry* assetRegistry, gfx::TextureManager* textureManager, gfx::MaterialManager* materialManager, QOpenGLExtraFunctions* openGLFunctions);

        void registerElement(const std::filesystem::path& sourcePath) override;
        void drawScene(Shader* shader);

        void loadModel(const std::string& key);
        void processNode(aiNode* node, const aiScene* scene, SceneNode* sceneNode, const std::filesystem::path& modelSourcePath);
        void processMesh(aiMesh* mesh, const aiScene* scene, SceneNode* model, std::string_view modelSourcePath, QOpenGLExtraFunctions* openGLFunctions);
        std::vector<uint32_t> processIndices(const aiMesh* mesh);
        std::vector<Vertex> processVertices(const aiMesh* mesh);
        GpuHandles initialiseGpuHandles(std::vector<gfx::Vertex>&& vertices, std::vector<unsigned int>&& indices, QOpenGLExtraFunctions* openGLFunctions);
        GLuint getMaterialTexture(const aiMaterial* material, std::string_view modelSourcePath, aiTextureType type);
        std::optional<std::filesystem::path> findValidTexturePath(std::string_view modelSourcePath, std::string_view textureStringPath);
        std::string loadMaterial(const aiMaterial* material, std::string_view modelSourcePath);

        // Spawner
        void spawnModel(const std::string& key);



    private:
        QOpenGLExtraFunctions*  m_openGLFunctions;
        gfx::TextureManager*    m_textureManager;
        gfx::MaterialManager*   m_materialManager;
        EntityFactory*          m_entityFactory;
        
        uint32_t m_materialCounter; // Use for default material naming if material name is not found
};

}

#endif 