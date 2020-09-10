# CMake entry point
cmake_minimum_required (VERSION 2.6)
project (Dangerous)

find_package(OpenGL REQUIRED)

add_definitions(-DDEBUG_RENDER)


if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
    message( FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like bin_Visual2012_64bits/)" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
    message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
    message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()



# Compile external dependencies 
add_subdirectory (external)

# On Visual 2005 and above, this module can set the debug working directory
cmake_policy(SET CMP0026 OLD)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/rpavlik-cmake-modules-fe2273")
include(CreateLaunchers)
include(MSVCMultipleProcessCompile) # /MP

if(INCLUDE_DISTRIB)
    add_subdirectory(distrib)
endif(INCLUDE_DISTRIB)



include_directories(
    external/AntTweakBar-1.16/include/
    external/glfw-3.1.2/include/GLFW/
    external/glm-0.9.7.1/
    external/glew-1.13.0/include/
    external/assimp-3.0.1270/include/
    external/bullet-2.81-rev2613/src/
    external/stb_image/
    .
)

set(ALL_LIBS
    ${OPENGL_LIBRARY}
    glfw
    GLEW_1130
)

add_definitions(
    -DTW_STATIC
    -DTW_NO_LIB_PRAGMA
    -DTW_NO_DIRECT3D
    -DGLEW_STATIC
    -D_CRT_SECURE_NO_WARNINGS
)

set(VD_SRC "${CMAKE_CURRENT_SOURCE_DIR}/engine/src")
set(engine_SOURCES 
    engine/src/game.cpp
    engine/src/main.cpp
    engine/src/scene/actor.h
    engine/src/scene/actor.cpp
    engine/src/scene/sprite.h
    engine/src/scene/sprite.cpp
    engine/src/scene/material.h
    engine/src/scene/material.cpp
    engine/src/scene/texture.h
    engine/src/scene/texture.cpp
    engine/src/scene/tweens.h
    engine/src/scene/tweens.cpp
    engine/src/scene/button.h
    engine/src/scene/button.cpp
    engine/src/core/game_var.h
    engine/src/core/actor_state.h
    engine/src/core/quad.h
    engine/src/core/batch_collector.h
    engine/src/core/batch_collector.cpp
    engine/src/core/vertex.h
    engine/src/core/vertex.cpp
    engine/src/core/signal.h
    engine/src/system/input_system.h
    engine/src/system/input_system.cpp
    engine/src/system/debug_render.h
    engine/src/system/debug_render.cpp
    engine/src/system/actors/debug_render_actor.h
    engine/src/system/actors/debug_render_actor.cpp
    engine/src/system/actors/mouse_area.h
    engine/src/system/actors/mouse_area.cpp
#    engine/src/system/texture_remover.h
#    engine/src/system/texture_remover.cpp
)

# engine
add_executable(engine 
    ${engine_SOURCES}

#    common/shader.cpp
#    common/shader.hpp
#    common/controls.cpp
#    common/controls.hpp
#    common/texture.cpp
#    common/texture.hpp
    
    engine/TransformVertexShader.vertexshader
    engine/TextureFragmentShader.fragmentshader
    )
include_directories(engine/src)
target_link_libraries(engine
    ${ALL_LIBS}
)
# Xcode and Visual working directories
set_target_properties(engine PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/engine/")
create_target_launcher(engine WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/engine/")

source_group(TREE ${VD_SRC} FILES ${engine_SOURCES})


SOURCE_GROUP(common REGULAR_EXPRESSION ".*/common/.*" )
SOURCE_GROUP(shaders REGULAR_EXPRESSION ".*/.*shader$" )


if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
add_custom_command(
   TARGET engine POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/engine${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/engine/"
)
elseif (${CMAKE_GENERATOR} MATCHES "Xcode" )

endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )

