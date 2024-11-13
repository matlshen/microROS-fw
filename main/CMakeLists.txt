file(GLOB_RECURSE BSP_SOURCES bsp/*.c bsp/*.cpp bsp/*.h bsp/*.hpp)
file(GLOB_RECURSE SRC_SOURCES src/*.c src/*.cpp src/*.h src/*.hpp)
idf_component_register(SRCS ${SRC_SOURCES} ${BSP_SOURCES} INCLUDE_DIRS "bsp" "src")