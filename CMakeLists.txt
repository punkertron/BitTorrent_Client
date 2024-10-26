cmake_minimum_required(VERSION 3.25.1)
project(TorrentProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2 -march=native")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g3")

include_directories(incs)
include_directories(spdlog)
include_directories(bencode)
include_directories(cxxopts)

# libtorrent_core.a
set(LIB_SOURCES_DIR srcs)
set(LIB_SOURCES
    ${LIB_SOURCES_DIR}/connection.cpp
    ${LIB_SOURCES_DIR}/Message.cpp
    ${LIB_SOURCES_DIR}/PeerConnection.cpp
    ${LIB_SOURCES_DIR}/PeerRetriever.cpp
    ${LIB_SOURCES_DIR}/PeersQueue.cpp
    ${LIB_SOURCES_DIR}/Piece.cpp
    ${LIB_SOURCES_DIR}/PieceManager.cpp
    ${LIB_SOURCES_DIR}/TorrentClient.cpp
    ${LIB_SOURCES_DIR}/TorrentFileParser.cpp
    ${LIB_SOURCES_DIR}/utils.cpp
)
add_library(torrent_core STATIC ${LIB_SOURCES})
target_link_libraries(torrent_core PUBLIC cryptopp curl)

##################################################################
# torrent_client - Console version of the program
set(CONSOLE_SOURCES_DIR terminal_main)
add_executable(torrent_client
    ${CONSOLE_SOURCES_DIR}/main.cpp
)
target_link_libraries(torrent_client PRIVATE torrent_core)

##################################################################
# Torrent Client Qt - GUI version of the program
find_package(Qt6 COMPONENTS Widgets QUIET)
if (Qt6_FOUND)
    message(STATUS "Using Qt6")
    set(QT_VERSION 6)
    set(QT_LIBRARIES Qt6::Widgets)
else()
    find_package(Qt5 COMPONENTS Widgets REQUIRED)
    message(STATUS "Using Qt5")
    set(QT_VERSION 5)
    set(QT_LIBRARIES Qt5::Widgets)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(QT_SOURCES_DIR Qt)
add_executable(TorrentClientQt
    ${QT_SOURCES_DIR}/main_Qt.cpp
    ${QT_SOURCES_DIR}/Window.cpp
    ${QT_SOURCES_DIR}/Window.hpp
    ${QT_SOURCES_DIR}/resources.qrc
)

set_target_properties(TorrentClientQt PROPERTIES OUTPUT_NAME "Torrent Client Qt")

target_link_libraries(TorrentClientQt ${QT_LIBRARIES} torrent_core)

##################################################################
# format code. runs first
find_program(CLANG_FORMAT "clang-format")
if(CLANG_FORMAT)
    message(STATUS "clang-format found")

    file(GLOB_RECURSE ALL_SOURCE_FILES
        incs/*.hpp
        ${LIB_SOURCES_DIR}/*.cpp
        ${CONSOLE_SOURCES_DIR}/*.hpp
        ${QT_SOURCES_DIR}/*.cpp
        ${QT_SOURCES_DIR}/*.hpp
    )

    add_custom_target(
        clang-format
        ALL
        COMMAND clang-format -i ${ALL_SOURCE_FILES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        VERBATIM
    )

    add_dependencies(torrent_core clang-format)
    add_dependencies(torrent_client clang-format)
    add_dependencies(TorrentClientQt clang-format)

else()
    message(STATUS "clang-format not found, skipping formatting step")
endif()