#include "Mesh.h"


namespace gfx
{

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures,  QOpenGLExtraFunctions* openGLFunctions)
    : m_openGLFunctions(openGLFunctions),
      m_textures(textures)
{
    m_gpuHandles.numIndices = indices.size();

    m_openGLFunctions->glGenVertexArrays(1, &m_gpuHandles.VAO);
    m_openGLFunctions->glGenBuffers(1, &m_gpuHandles.VBO);
    m_openGLFunctions->glGenBuffers(1, &m_gpuHandles.EBO);

    m_openGLFunctions->glBindVertexArray(m_gpuHandles.VAO);
    m_openGLFunctions->glBindBuffer(GL_ARRAY_BUFFER, m_gpuHandles.VBO);

    m_openGLFunctions->glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

    m_openGLFunctions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gpuHandles.EBO);
    m_openGLFunctions->glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_gpuHandles.numIndices * sizeof(unsigned int), 
                 &indices[0], GL_STATIC_DRAW);

    // vertex positions
    m_openGLFunctions->glEnableVertexAttribArray(0);	
    m_openGLFunctions->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    m_openGLFunctions->glEnableVertexAttribArray(1);	
    m_openGLFunctions->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    m_openGLFunctions->glEnableVertexAttribArray(2);	
    m_openGLFunctions->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    m_openGLFunctions->glBindVertexArray(0);
}

void Mesh::Draw(Shader* shader) 
{
    assert(shader != nullptr);
    assert(m_openGLFunctions != nullptr);
    assert(m_gpuHandles.VAO != 0);
    assert(m_gpuHandles.numIndices > 0);
    assert(shader->getShaderID() != 0); 

    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for (unsigned int i = 0; i < m_textures.size(); i++)
    {
        assert(m_textures[i].textureID != 0); 
        m_openGLFunctions->glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name = m_textures[i].shaderType;
        if(name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if(name == "texture_specular")
            number = std::to_string(specularNr++);

        shader->updateUniformValue((name + number).c_str(), i);
        m_openGLFunctions->glBindTexture(GL_TEXTURE_2D, m_textures[i].textureID);
    }
    m_openGLFunctions->glActiveTexture(GL_TEXTURE0);

    while (m_openGLFunctions->glGetError() != GL_NO_ERROR);
    // draw mesh
    m_openGLFunctions->glBindVertexArray(m_gpuHandles.VAO);

    GLenum err = m_openGLFunctions->glGetError();
    if (err != GL_NO_ERROR) {
        qDebug() << "Error after glBindVertexArray:" << err;
    }

    m_openGLFunctions->glDrawElements(GL_TRIANGLES, m_gpuHandles.numIndices, GL_UNSIGNED_INT, nullptr);

    err = m_openGLFunctions->glGetError();
    if (err != GL_NO_ERROR) {
        qDebug() << "Error after glDrawElements:" << err;
    }
    m_openGLFunctions->glBindVertexArray(0);
} 

}