cmake_minimum_required(VERSION 3.0)
project(semestral-project C CXX)

set(SOURCES main.cpp object.cpp shader.cpp stb_image.c buffer.cpp camera.cpp generalObject.cpp baseline.cpp terrain.cpp)
set(EXE pgr-sem)
set(SHARE /home/vanari/Documents/pgr/share)
set(GLAD ${SHARE}/glad)
set(GLAD_SRC ${GLAD}/src/glad.c)

include_directories(${GLAD}/include)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(${EXE} ${SOURCES} ${GLAD_SRC})

target_link_libraries(${EXE}
	glfw
	assimp
	noise
	)
