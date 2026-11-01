cmake_minimum_required(VERSION 3.12...3.27.4)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "In-source builds are not supported.")
endif()

set(IS_IOS FALSE)
if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
  set(IS_IOS TRUE)
elseif(DEFINED CMAKE_OSX_SYSROOT AND CMAKE_OSX_SYSROOT MATCHES "iphoneos|iphonesimulator")
  set(IS_IOS TRUE)
endif()

if(NOT IS_IOS)
  set(CMAKE_OSX_DEPLOYMENT_TARGET 10.15 CACHE STRING "Minimum macOS deployment version")
endif()

file(STRINGS src/game/version.h VERSION_LINE LIMIT_COUNT 1 REGEX "^#define GAME_RELEASE_VERSION_INTERNAL ")
if(VERSION_LINE MATCHES "([0-9]+)\\.([0-9]+)\\.([0-9]+)")
  set(VERSION_MAJOR ${CMAKE_MATCH_1})
  set(VERSION_MINOR ${CMAKE_MATCH_2})
  set(VERSION_PATCH ${CMAKE_MATCH_3})
else()
  set(VERSION_MAJOR 0)
  set(VERSION_MINOR 0)
  set(VERSION_PATCH 0)
endif()

set(DATA_VERSION_FILE "${CMAKE_BINARY_DIR}/data/data_version.txt")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/data")
file(WRITE "${DATA_VERSION_FILE}" "DDNet ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}\n")

file(STRINGS src/game/version.h TCLIENT_VERSION_LINE LIMIT_COUNT 1 REGEX "^#define TCLIENT_VERSION ")
if(TCLIENT_VERSION_LINE MATCHES "([0-9]+)\\.([0-9]+)\\.([0-9]+)")
  set(TCLIENT_VERSION_MAJOR ${CMAKE_MATCH_1})
  set(TCLIENT_VERSION_MINOR ${CMAKE_MATCH_2})
  set(TCLIENT_VERSION_PATCH ${CMAKE_MATCH_3})
else()
  set(TCLIENT_VERSION_MAJOR 0)
  set(TCLIENT_VERSION_MINOR 0)
  set(TCLIENT_VERSION_PATCH 0)
endif()

project(TClient VERSION ${TCLIENT_VERSION_MAJOR}.${TCLIENT_VERSION_MINOR}.${TCLIENT_VERSION_PATCH})
file(APPEND "${DATA_VERSION_FILE}" "TClient ${TCLIENT_VERSION_MAJOR}.${TCLIENT_VERSION_MINOR}.${TCLIENT_VERSION_PATCH}\n")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(TARGET_BITS "32")
else()
  set(TARGET_BITS "64")
endif()

if(IS_IOS)
  set(TARGET_OS "ios")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(TARGET_OS "windows")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(TARGET_OS "linux")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  set(TARGET_OS "mac")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Android")
  set(TARGET_OS "android")
endif()

include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)
include(CheckLinkerFlag)
include(CheckSymbolExists)
include(CheckAtomic)

option(WEBSOCKETS "Enable websockets support" OFF)
option(MYSQL "Enable mysql support" OFF)
option(AUTOUPDATE "Enable the autoupdater" ON)
option(INFORM_UPDATE "Inform about available updates" ON)
option(VIDEORECORDER "Enable video recording support via FFmpeg" ON)
option(UPNP "Enable UPnP support" OFF)
option(ANTIBOT "Enable support for a dynamic anticheat library" OFF)
option(HEADLESS_CLIENT "Build the client without graphics" OFF)
option(CLIENT "Compile client" ON)
option(SERVER "Compile server" ON)
option(TOOLS "Compile tools" ON)
option(DOWNLOAD_GTEST "Download and compile GTest" ON)
option(STEAM "Build the Steam release version" ON)
option(DISCORD "Enable Discord rich presence support" ON)
option(DISCORD_DYNAMIC "Enable discovering Discord rich presence libraries at runtime" OFF)
option(PREFER_BUNDLED_LIBS "Prefer bundled libraries over system libraries" ON)
option(DEV "Don't generate stuff necessary for packaging" OFF)
option(VULKAN "Enable the vulkan backend" ON)
option(EXCEPTION_HANDLING "Enable crashdumps" OFF)
option(IPO "Enable interprocedural optimizations" OFF)
option(PRECOMPILE_HEADERS "Precompile commonly used headers" ON)
option(SECURITY_COMPILER_FLAGS "Whether to set security-relevant compiler flags" ON)

if(TARGET_OS STREQUAL "android")
  set(DISCORD OFF)
  set(STEAM OFF)
endif()

if(NOT VERSION)
  set(VERSION ${PROJECT_VERSION})
endif()

set(OpenGL_GL_PREFERENCE LEGACY)

if(NOT(CMAKE_BUILD_TYPE))
  if(NOT(DEV))
    set(CMAKE_BUILD_TYPE Release)
  else()
    set(CMAKE_BUILD_TYPE Debug)
  endif()
endif()

set(SERVER_EXECUTABLE DDNet-Server CACHE STRING "Name of the built server executable")
set(CLIENT_EXECUTABLE DDNet CACHE STRING "Name of the build client executable")

function(set_src VAR GLOBBING DIRECTORY)
  set(FILES)
  foreach(file ${ARGN})
    list(APPEND FILES "${PROJECT_SOURCE_DIR}/${DIRECTORY}/${file}")
  endforeach()
  set(${VAR} ${FILES} PARENT_SCOPE)
endfunction()

set_src(GAME_SERVER GLOB src/game/server
  entities/character.cpp entities/character.h entities/flag.cpp entities/flag.h entities/laser.cpp entities/laser.h entities/projectile.cpp entities/projectile.h
  entity.cpp entity.h gamecontext.cpp gamecontext.h gamecontroller.cpp gamecontroller.h gameworld.cpp gameworld.h
  player.cpp player.h score.cpp score.h teehistorian.cpp teehistorian.h teams.cpp teams.h
)

set_src(ENGINE_SHARED GLOB src/engine/shared
  compression.cpp compression.h config.cpp config.h console.cpp console.h demo.cpp demo.h econ.cpp econ.h
  engine.cpp engine.h fifo.cpp fifo.h filecollection.cpp filecollection.h global_uuid_manager.cpp global_uuid_manager.h
  host_lookup.cpp host_lookup.h huffman.cpp huffman.h jobs.cpp jobs.h json.cpp json.h kernel.cpp kernel.h
  linereader.cpp linereader.h map.cpp map.h masterserver.cpp masterserver.h memheap.cpp memheap.h
  netban.cpp netban.h network.cpp network.h network_client.cpp network_client.h network_console.cpp network_console.h
  network_console_conn.cpp network_console_conn.h network_conn.cpp network_conn.h network_server.cpp network_server.h network_stun.cpp network_stun.h
  packer.cpp packer.h protocol_ex.cpp protocol_ex.h ringbuffer.cpp ringbuffer.h serverinfo.cpp serverinfo.h
  sixup_translate_snapshot.cpp sixup_translate_snapshot.h snapshot.cpp snapshot.h storage.cpp storage.h stun.cpp stun.h
  teehistorian_ex.cpp teehistorian_ex.h translation_context.cpp translation_context.h uuid_manager.cpp uuid_manager.h
  video.cpp video.h websockets.cpp websockets.h bestclient_indicator_protocol.cpp bestclient_indicator_protocol.h
)

set_src(GAME_CLIENT GLOB_RECURSE src/game/client
  animstate.cpp animstate.h component.cpp component.h
  components/bestclient/automargin.cpp components/bestclient/automargin.h components/bestclient/better_preview.cpp components/bestclient/better_preview.h
  components/bestclient/chat_scroll.cpp components/bestclient/chat_scroll.h components/bestclient/clientindicator/browser_cache.cpp
  components/bestclient/clientindicator/version_label.h components/bestclient/scoreboard.cpp components/bestclient/scoreboard.h components/bestclient/version.h
  components/console.cpp components/console.h components/controls.cpp components/controls.h components/countryflags.cpp components/countryflags.h
  components/demo.cpp components/demo.h components/editor.cpp components/editor.h components/emoticon.cpp components/emoticon.h
  components/engine.cpp components/engine.h components/ghost.cpp components/ghost.h components/hud.cpp components/hud.h
  components/items.cpp components/items.h components/killmessages.cpp components/killmessages.h components/mapimages.cpp components/mapimages.h
  components/menus.cpp components/menus.h components/motd.cpp components/motd.h components/particles.cpp components/particles.h
  components/players.cpp components/players.h components/race_demo.cpp components/race_demo.h components/skins.cpp components/skins.h
  components/sounds.cpp components/sounds.h components/statboard.cpp components/statboard.h components/tooltips.cpp components/tooltips.h
  components/voting.cpp components/voting.h gameclient.cpp gameclient.h
  
  # ТВОЙ ФАЙЛ
  components/nocheat_detector.cpp
)

add_library(engine-shared EXCLUDE_FROM_ALL OBJECT ${ENGINE_SHARED})
add_library(game-shared EXCLUDE_FROM_ALL OBJECT ${GAME_SERVER})
add_library(game-client EXCLUDE_FROM_ALL OBJECT ${GAME_CLIENT})

if(CLIENT)
  add_executable(${CLIENT_EXECUTABLE} ${PLATFORM_CLIENT})
  target_link_libraries(${CLIENT_EXECUTABLE} PRIVATE game-client game-shared engine-shared ${PLATFORM_CLIENT_LIBS} ${PLATFORM_LIBS})
  install(TARGETS ${CLIENT_EXECUTABLE} DESTINATION .)
endif()

if(SERVER)
  add_executable(${SERVER_EXECUTABLE} src/engine/server/main.cpp)
  target_link_libraries(${SERVER_EXECUTABLE} PRIVATE game-shared engine-shared ${PLATFORM_LIBS})
  install(TARGETS ${SERVER_EXECUTABLE} DESTINATION .)
endif()
