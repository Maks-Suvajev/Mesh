#ifndef MESH_H
#define MESH_H

#include "MeshTypes.h"
#include <iostream>

#include <QOpenGLExtraFunctions>

namespace gfx
{

class Mesh
{
    public:
        Mesh(MeshData&& initData, QOpenGLExtraFunctions* openGLFunctions);

        GpuHandles getGPUHandles()
        {
            return m_gpuHandles;
        }

    private:
        GpuHandles               m_gpuHandles;
        QOpenGLExtraFunctions*   m_openGLFunctions;
};

}

#endif