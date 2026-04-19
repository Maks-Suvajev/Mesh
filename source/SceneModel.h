
#ifndef SCENE_MODEL_H
#define SCENE_MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"
#include "Shader.h"

#include "stb_image.h"

namespace gfx
{

class SceneModel
{
    public:
        SceneModel(const std::string& path, QOpenGLExtraFunctions* openGLFunctions)
            : m_openGLFunctions(openGLFunctions),
              m_isLoaded(false),
              m_systemFilePath(std::filesystem::canonical(path)),
              m_filename(m_systemFilePath.filename().string())
        {
        }

        void Draw(Shader* shader);

        const bool isLoaded()
        {
            return m_isLoaded;
        }

        const std::string& getName()
        {
            return m_filename;
        } 

        
        const std::filesystem::path& getFilePath()
        {
            return m_systemFilePath;
        } 


        void loadModel();

    private:
        bool m_isLoaded;
        std::filesystem::path m_systemFilePath;
        std::string m_filename;
        std::vector<Mesh> meshes;
        std::vector<Texture> textures_loaded;
        QOpenGLExtraFunctions* m_openGLFunctions;

        void processNode(aiNode* node, const aiScene* scene);
        Mesh processMesh(aiMesh* mesh, const aiScene* scene, QOpenGLExtraFunctions* openGLFunctions);
        unsigned int TextureFromFile(const char *path, const std::string &directory);

        std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);


};

}

#endif