#ifndef MESH_H
#define MESH_H

#include "SceneModelTypes.h"
#include "TextureTypes.h"
#include "Shader.h"
#include <iostream>

#include <QOpenGLExtraFunctions>

namespace gfx
{


class Mesh
{
    public:
        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures,  QOpenGLExtraFunctions* openGLFunctions);
        void Draw(Shader* shader);

    private:

        std::vector<Texture>     m_textures; 
        GpuHandles               m_gpuHandles;
        QOpenGLExtraFunctions*   m_openGLFunctions;
};

}

#endif