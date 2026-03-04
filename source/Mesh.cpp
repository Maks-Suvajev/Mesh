#include "Mesh.h"


namespace gfx
{

Mesh::Mesh(MeshData&& initData, QOpenGLExtraFunctions* openGLFunctions)
    : m_openGLFunctions(openGLFunctions)
{

    #ifdef ENABLE_DEBUG_MESSAGES
        std::cout << "Vertices size = " << initData.vertices.size() << std::endl;
    #endif
    
    // VAO init and bind
    m_openGLFunctions->glGenVertexArrays(1, &m_gpuHandles.VAO);
    m_openGLFunctions->glBindVertexArray(m_gpuHandles.VAO);            

    // VBO init and bind
    m_openGLFunctions->glGenBuffers(1, &m_gpuHandles.VBO);
    m_openGLFunctions->glBindBuffer(GL_ARRAY_BUFFER, m_gpuHandles.VBO);
    m_openGLFunctions->glBufferData(GL_ARRAY_BUFFER, initData.vertices.size() * sizeof(float), initData.vertices.data(), GL_STATIC_DRAW);

    m_gpuHandles.useEBO = false;

    if (!initData.indices.empty())
    { 
        // EBO init bind
        m_openGLFunctions->glGenBuffers(1, &m_gpuHandles.EBO);
        m_openGLFunctions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gpuHandles.EBO);
        m_openGLFunctions->glBufferData(GL_ELEMENT_ARRAY_BUFFER, initData.indices.size() * sizeof(unsigned int), initData.indices.data(), GL_STATIC_DRAW);
        
        m_gpuHandles.useEBO = true;

        #ifdef ENABLE_DEBUG_MESSAGES
            std::cout << "DEBUG::No indices found - not populating EBO" << std::endl;
        #endif

    }
    // Define VAO attributes

    //TODO: This is compatible currently only with my predefined cube mesh
    // Define position coords
    m_openGLFunctions->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    m_openGLFunctions->glEnableVertexAttribArray(0);

    // Normals
    m_openGLFunctions->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    m_openGLFunctions->glEnableVertexAttribArray(1);

    // Texture coords
    m_openGLFunctions->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    m_openGLFunctions->glEnableVertexAttribArray(2);

    #ifdef ENABLE_DEBUG_MESSAGES
        std::cout << "DEBUG::Finished mesh init" << std::endl;
    #endif

    m_gpuHandles.numVertices = initData.numVertices;
}

}