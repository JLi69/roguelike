cmake_minimum_required(VERSION 3.11)

set(CMAKE_C_FLAGS "-O2 -static-libgcc")

project(roguelike)

add_custom_target(
	run
	COMMAND ./${PROJECT_NAME}	
	)

add_custom_target(
	lib_glad
	COMMAND cd lib/glad && make	
	)

find_package(OpenGL REQUIRED)

add_subdirectory(lib/glad)

aux_source_directory(src source)
aux_source_directory(lib deps)

add_executable(
	${PROJECT_NAME}

	${source}
	${deps}
)

#Link with libraries
add_dependencies(${PROJECT_NAME} lib_glad)
target_link_libraries(${PROJECT_NAME} glfw3 m)
target_link_libraries(${PROJECT_NAME} SOIL2 m OpenGL::GL)
target_link_libraries(${PROJECT_NAME} ${CMAKE_BINARY_DIR}/lib/glad/libglad.a)
