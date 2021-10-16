project(Murmuration)

file(GLOB SRC "ParticleSystemApp.cpp" "ParticleSystemApp.hpp" "${CONAN_SRC_DIRS_IMGUI}/imgui_impl_opengl3.cpp" "${CONAN_SRC_DIRS_IMGUI}/imgui_impl_sdl.cpp")

add_executable(Murmuration ${SRC})
set_target_properties(Murmuration PROPERTIES FOLDER app)
set_target_properties(Murmuration PROPERTIES LINK_FLAGS "/ignore:4099")

add_compile_definitions(IMGUI_IMPL_OPENGL_LOADER_GLAD)

target_include_directories(Murmuration PRIVATE ${OPENGL_INCLUDE_DIRS}
                                            "$ENV{TP_DIR}/diligentGraphics" ${CONAN_SRC_DIRS_IMGUI})

conan_target_link_libraries(Murmuration)
target_link_libraries(Murmuration physics render ui imgui utils midireader oscreader ${OPENGL_LIBRARIES})

install(TARGETS Murmuration RUNTIME DESTINATION bin)