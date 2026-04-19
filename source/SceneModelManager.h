#ifndef SCENE_MODEL_MANAGER_H
#define SCENE_MODEL_MANAGER_H

#include <unordered_map>
#include <string>
#include <optional>
#include <iostream>

#include "SceneModel.h"
#include "ResourceManager.h"

#include <QOpenGLExtraFunctions>


namespace gfx
{

class SceneModelManager : public ResourceManager<SceneModel>
{
    public:
        SceneModelManager() = default;
        SceneModelManager(AssetRegistry* assetRegistry, QOpenGLExtraFunctions* openGLFunctions);

        void registerElement(const std::filesystem::path& sourcePath) override;
        void drawScene(Shader* shader);

        void loadModel(const std::string& key);

    private:
        QOpenGLExtraFunctions* m_openGLFunctions;
};

}

#endif 