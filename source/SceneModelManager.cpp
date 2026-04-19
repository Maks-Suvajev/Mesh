#include "SceneModelManager.h"

namespace gfx
{
    SceneModelManager::SceneModelManager(AssetRegistry* assetRegistry, QOpenGLExtraFunctions* openGLFunctions)
        :  ResourceManager<SceneModel>(assetRegistry, supportedModelFileTypes),
           m_openGLFunctions(openGLFunctions)
    {
        refreshElements();
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

        m_elements[key] = std::make_unique<SceneModel>(sourcePath.string(), m_openGLFunctions);

    }

    void SceneModelManager::drawScene(Shader* shader)
    {
         for (const auto& [key, element] : m_elements)
         {
            element.get()->Draw(shader);
         }
    }

    void SceneModelManager::loadModel(const std::string& key)
    {
        std::cout << "Entered SceneModelManager::loadModel" << std::endl;
        getMap().at(key)->loadModel();
    }

}