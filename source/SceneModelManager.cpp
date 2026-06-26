#include "SceneModelManager.h"

namespace gfx
{
    SceneModelManager::SceneModelManager(AssetRegistry* assetRegistry, gfx::TextureManager* textureManager, gfx::MaterialManager* materialManager, QOpenGLExtraFunctions* openGLFunctions)
        :  ResourceManager<SceneModel>(assetRegistry, supportedModelFileTypes),
           m_openGLFunctions(openGLFunctions),
           m_textureManager(textureManager),
           m_materialManager(materialManager)
    {
        refreshElements();
    }

    void SceneModelManager::spawnModel(const std::string& key)
    {
         m_entityFactory->createRenderableEntity(getElementRef(key).sceneModelRoot);
    }

    void SceneModelManager::registerElement(const std::filesystem::path& sourcePath)
    {
        std::string key;

        try
        {
            key = std::filesystem::canonical(sourcePath).generic_string();
        }
        catch(const std::filesystem::filesystem_error& e)
        {
            std::cerr << "SceneModelManager::registerElement::Requested file does not exist: " << e.what() << std::endl;
        }


        if (m_elements.contains(key))
        {
            #ifdef ENABLE_DEBUG_MESSAGES
                std::cout << "ERROR::Element already loaded with the key: " << sourcePath.string() << std::endl;
            #endif

            return;
        }

        m_elements[key] = std::make_unique<gfx::SceneModel>(gfx::SceneModel{.sourceFileName = sourcePath.filename().string(), .sourceFilePath = sourcePath});

    }

    void SceneModelManager::drawScene(Shader* shader)
    {
         for (const auto& [key, element] : m_elements)
         {
            //element.get()->Draw(shader);
         }
    }

    void SceneModelManager::loadModel(const std::string& key)
    {
        m_materialCounter = 0; 
        SceneModel* model = m_elements.at(key).get();

        Assimp::Importer import;

        std::cout << "Entered loadModel()" << std::endl;

        const aiScene* scene = import.ReadFile(model->sourceFilePath.string(), aiProcess_Triangulate | aiProcess_FlipUVs);


        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            #ifdef ENABLE_DEBUG_MESSAGES
                std::cout << "ERROR::Model::loadModel::" << import.GetErrorString() << std::endl;
            #endif

            return;
        }

        processNode(scene->mRootNode, scene, &model->sceneModelRoot, model->sourceFilePath);

        model->isLoaded = true;

    }

    void SceneModelManager::processNode(aiNode* node, const aiScene* scene, SceneNode* sceneNode, const std::filesystem::path& modelSourcePath)
    {
        //copy transform over
        sceneNode->relativeTransform = glm::transpose(glm::make_mat4(&node->mTransformation.a1));

        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            processMesh(mesh, scene, sceneNode, modelSourcePath.generic_string(), m_openGLFunctions);
        }

        sceneNode->children.reserve(node->mNumChildren); // reserve to prevent reallocation

        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            sceneNode->children.emplace_back();
            processNode(node->mChildren[i], scene,  &sceneNode->children[i], modelSourcePath);
        }

    }

    std::vector<Vertex> SceneModelManager::processVertices(const aiMesh* mesh)
    {
        std::vector<Vertex> vertices;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;

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

        return vertices;
    }

    std::vector<uint32_t> SceneModelManager::processIndices(const aiMesh* mesh)
    {
        std::vector<uint32_t> indices;
    
        for (uint32_t i = 0; i < mesh->mNumFaces; ++i)
        {
            aiFace face = mesh->mFaces[i];
            for (uint32_t j = 0; j < face.mNumIndices; ++j)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        return indices;
    }

    void SceneModelManager::processMesh(aiMesh* mesh, const aiScene* scene, SceneNode* model, std::string_view modelSourcePath, QOpenGLExtraFunctions* openGLFunctions)
    {
        gfx::RenderAsset newRenderAsset{};

        std::vector<Vertex> vertices = processVertices(mesh);
        std::vector<unsigned int> indices = processIndices(mesh);

        model->renderAssets.mesh = initialiseGpuHandles(std::move(vertices), std::move(indices), openGLFunctions);

        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            model->renderAssets.materialKey = loadMaterial(material, modelSourcePath);
        }   
    }

    GpuHandles SceneModelManager::initialiseGpuHandles(std::vector<gfx::Vertex>&& vertices, std::vector<unsigned int>&& indices, QOpenGLExtraFunctions* openGLFunctions)
    {

        assert(!indices.empty()); // Make sure indices aren't empty

        uint32_t maxIndex = *std::max_element(indices.begin(), indices.end()); 
        assert(maxIndex < vertices.size());

        GpuHandles gpuHandles;

        gpuHandles.numIndices = indices.size();

        openGLFunctions->glGenVertexArrays(1, &gpuHandles.VAO);
        openGLFunctions->glGenBuffers(1, &gpuHandles.VBO);
        openGLFunctions->glGenBuffers(1, &gpuHandles.EBO);

        openGLFunctions->glBindVertexArray(gpuHandles.VAO);
        openGLFunctions->glBindBuffer(GL_ARRAY_BUFFER, gpuHandles.VBO);

        openGLFunctions->glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

        openGLFunctions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuHandles.EBO);
        openGLFunctions->glBufferData(GL_ELEMENT_ARRAY_BUFFER, gpuHandles.numIndices * sizeof(unsigned int), 
                    &indices[0], GL_STATIC_DRAW);

        // vertex positions
        openGLFunctions->glEnableVertexAttribArray(0);	
        openGLFunctions->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        openGLFunctions->glEnableVertexAttribArray(1);	
        openGLFunctions->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        openGLFunctions->glEnableVertexAttribArray(2);	
        openGLFunctions->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

        openGLFunctions->glBindVertexArray(0);

        return gpuHandles;
    }

    std::optional<std::filesystem::path> SceneModelManager::findValidTexturePath(std::string_view modelSourcePath, std::string_view textureStringPath)
    {
        std::filesystem::path basePath = std::filesystem::path(modelSourcePath).parent_path();
        std::filesystem::path texturePath = std::filesystem::path(textureStringPath);

        std::filesystem::path combinedPath = basePath / texturePath;

        if (std::filesystem::exists(texturePath))
        {
            return std::filesystem::canonical(texturePath);
        }
        else if (std::filesystem::exists(combinedPath))
        {
            return std::filesystem::canonical(combinedPath);
        }

        return std::nullopt;

    }

    GLuint SceneModelManager::getMaterialTexture(const aiMaterial* material, std::string_view modelSourcePath, aiTextureType type)
    {
        aiReturn retVal;
        aiString texturePath;

        if (material->GetTexture(type, 0, &texturePath) == AI_SUCCESS)
        {
            std::optional<std::filesystem::path> path = findValidTexturePath(modelSourcePath, std::string(texturePath.C_Str()));

            if (path.has_value())
            {
                const std::string key = std::filesystem::canonical(path.value()).generic_string();
                m_textureManager->registerElement(path.value());
                m_textureManager->loadTexture(key);

                return m_textureManager->getTextureID(key);
            }
        }

        return m_textureManager->getDefaultTexture(type);
    }


    // return the key to the new material that was created.
    std::string SceneModelManager::loadMaterial(const aiMaterial* material, std::string_view modelSourcePath)
    {
        gfx::Material newMaterial{};
        std::string newKey;

        // Create name
        aiString materialName;
        if (material->Get(AI_MATKEY_NAME, materialName) == AI_SUCCESS)
        {
            // Construct name from path and material name
            newMaterial.name = modelSourcePath + "_" + std::string(materialName.C_Str());
        }
        else
        {
            // Without material name, construct default name
            newMaterial.name = modelSourcePath + "_material_" + std::to_string(m_materialCounter);
            ++m_materialCounter;
        }

        newKey = newMaterial.name;

        newMaterial.shininess = gfx::defaultShininess; //Assuming no shininess attached

        newMaterial.lightingTextures.diffuse    = getMaterialTexture(material, modelSourcePath, aiTextureType_DIFFUSE);
        newMaterial.lightingTextures.specular   = getMaterialTexture(material, modelSourcePath, aiTextureType_SPECULAR);

        m_materialManager->registerElement(newKey, std::move(newMaterial));

        return newKey;
    }
}