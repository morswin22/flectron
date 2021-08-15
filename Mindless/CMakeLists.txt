project(Mindless)

configure_file(assets/platform.png assets/platform.png COPYONLY)
configure_file(shaders/batch.vert shaders/batch.vert COPYONLY)
configure_file(shaders/batch.frag shaders/batch.frag COPYONLY)

add_executable(Mindless src/mindless.cpp)

target_link_libraries(Mindless MindlessEngine)