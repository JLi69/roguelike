cmake_minimum_required(VERSION 3.11)

set(CMAKE_C_FLAGS "-O2 -static-libgcc")

project(roguelike)

add_custom_target(
	run
	COMMAND ./${PROJECT_NAME}	
	)

find_package(OpenGL REQUIRED)

include_directories(glad/include)
aux_source_directory(src source)
aux_source_directory(lib deps)

add_executable(
	${PROJECT_NAME}

	${source}
	${deps}
	glad/src/glad.c
)

#Link with libraries
target_link_libraries(${PROJECT_NAME} glfw3 m)
target_link_libraries(${PROJECT_NAME} SOIL2 m OpenGL::GL)
