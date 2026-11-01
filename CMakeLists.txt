cmake_minimum_required(VERSION 3.12...3.27.4)
project(DDNet CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "In-source builds are not supported.")
endif()

# Автоматический поиск всех файлов
file(GLOB_RECURSE GAME_CLIENT_SRC "src/game/client/*.cpp" "src/game/client/*.h")
file(GLOB_RECURSE GAME_SERVER_SRC "src/game/server/*.cpp" "src/game/server/*.h")
file(GLOB_RECURSE ENGINE_SHARED_SRC "src/engine/shared/*.cpp" "src/engine/shared/*.h")

# Добавляем твой файл
list(APPEND GAME_CLIENT_SRC "${CMAKE_SOURCE_DIR}/src/game/client/components/nocheat_detector.cpp")

add_library(engine-shared OBJECT ${ENGINE_SHARED_SRC})
add_library(game-shared OBJECT ${GAME_SERVER_SRC})
add_library(game-client OBJECT ${GAME_CLIENT_SRC})

# Пути для компилятора
include_directories(src)
include_directories(src/game/client)
include_directories(src/game/client/components)
include_directories(src/engine)

add_executable(DDNet src/engine/client/main.cpp)
target_link_libraries(DDNet PRIVATE game-client game-shared engine-shared)

add_executable(DDNet-Server src/engine/server/main.cpp)
target_link_libraries(DDNet-Server PRIVATE game-shared engine-shared)
