add_library(MeshLib 
    source/Mesh.cpp
    source/MeshManager.cpp
)

target_link_libraries(MeshLib
    PRIVATE TypeLib
)

target_include_directories(MeshLib 
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/source
)