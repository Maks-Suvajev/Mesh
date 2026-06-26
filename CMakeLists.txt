add_library(SceneModelLib 
    source/SceneModelManager.cpp
)

# ASSIMP
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL_PDB OFF CACHE BOOL "" FORCE)

target_link_libraries(SceneModelLib
    PUBLIC
        assimp
    
    PRIVATE 
        TypeLib
        ShaderLib # Need temporarily while the draw function is in the mesh
        EntityLib
)

target_include_directories(SceneModelLib 
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/source
    PRIVATE ${CMAKE_SOURCE_DIR}/Dependencies/includes/stb
)