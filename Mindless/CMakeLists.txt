project(Mindless)

configure_file(assets/atlas.png assets/atlas.png COPYONLY)
configure_file(assets/font.png assets/font.png COPYONLY)
configure_file(shaders/batch.vert shaders/batch.vert COPYONLY)
configure_file(shaders/batch.frag shaders/batch.frag COPYONLY)
configure_file(shaders/light.comp shaders/light.comp COPYONLY)

add_executable(Mindless src/mindless.cpp)

target_link_libraries(Mindless MindlessEngine)