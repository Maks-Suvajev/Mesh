#include "SceneModel.h"

namespace gfx
{

void SceneModel::Draw(Shader* shader)
{
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes[i].Draw(shader);
    }
}

void SceneModel::loadModel()
{
    Assimp::Importer import;

    std::cout << "Entered loadModel()" << std::endl;

    const aiScene* scene = import.ReadFile(m_systemFilePath.string(), aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        #ifdef ENABLE_DEBUG_MESSAGES
            std::cout << "ERROR::Model::loadModel::" << import.GetErrorString() << std::endl;
        #endif

        return;
    }

    processNode(scene->mRootNode, scene);

    m_isLoaded = true;
}

void SceneModel::processNode(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene, m_openGLFunctions));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh SceneModel::processMesh(aiMesh* mesh, const aiScene* scene, QOpenGLExtraFunctions* openGLFunctions)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        // Process vertex
        glm::vec3  vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.Normal = vector;

        if (mesh->mTextureCoords[0])
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;

        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }

    // Process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Process material
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }   

    unsigned int maxIndex = *std::max_element(indices.begin(), indices.end());
    assert(maxIndex < vertices.size());

    return Mesh(vertices, indices, textures, m_openGLFunctions);
}

std::vector<Texture> SceneModel::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        std::cout << "Assimp texture path for type " << typeName << ": " << str.C_Str() << std::endl;

        bool skip = false;

        for (unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if (std::filesystem::weakly_canonical(textures_loaded[j].systemSourcePath) == std::filesystem::weakly_canonical(str.C_Str()))
            {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }

        if (!skip)
        {
            Texture texture;

            std::string directory = std::filesystem::path(m_systemFilePath).parent_path().string();

            texture.textureID = TextureFromFile(str.C_Str(), directory);
            texture.shaderType = typeName;
            texture.systemSourcePath = std::filesystem::path(str.C_Str());

            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }

    return textures;
}

unsigned int SceneModel::TextureFromFile(const char *path, const std::string &directory)
{
    std::string filename = directory + '/' + std::string(path);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    
    if (!data) {
        std::cout << "Texture failed to load at path: " << filename << std::endl;
        return 0; // return 0, let the caller handle it
    }

    GLenum format = GL_RGB; // safe default
    if (nrComponents == 1)      format = GL_RED;
    else if (nrComponents == 3) format = GL_RGB;
    else if (nrComponents == 4) format = GL_RGBA;
    else {
        std::cout << "Unexpected nrComponents: " << nrComponents << " at " << filename << std::endl;
        stbi_image_free(data);
        return 0;
    }

    unsigned int textureID;
    m_openGLFunctions->glGenTextures(1, &textureID);
    m_openGLFunctions->glBindTexture(GL_TEXTURE_2D, textureID);
    m_openGLFunctions->glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    m_openGLFunctions->glGenerateMipmap(GL_TEXTURE_2D);
    m_openGLFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    m_openGLFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    m_openGLFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    m_openGLFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);

    return textureID;
}

}