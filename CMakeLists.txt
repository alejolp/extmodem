cmake_minimum_required (VERSION 2.6)
project (extmodem)

# Alejandro Santos LU4EXT/HB3YNQ alejolp@gmail.com

## set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_SOURCE_DIR}/cmake/Modules")

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=c++11 -ggdb -march=native -fstack-protector ")

    if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic -Wno-unused-local-typedefs")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -ggdb -rdynamic -fstack-protector -lpthread -Wl,-z,relro ")
    endif()

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -g")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} -O2")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()
endif()

### BOOST ###

#set(Boost_USE_STATIC_LIBS       OFF)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

# boost_locale requires 1.48.0+
find_package( Boost 1.49.0 REQUIRED COMPONENTS system program_options thread )
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
list(APPEND EXTMODEM_LIBS ${Boost_LIBRARIES})

### ALSA ###

find_package(ALSA)

if(ALSA_FOUND)
    add_definitions(-DALSA_FOUND)
    include_directories(${ALSA_INCLUDE_DIRS})
    list(APPEND EXTMODEM_LIBS ${ALSA_LIBRARIES})
endif(ALSA_FOUND)

### PORTAUDIO ###

find_package(Portaudio)

if(PORTAUDIO2_FOUND)
  add_definitions(-DPORTAUDIO_FOUND)
  include_directories(${PORTAUDIO_INCLUDE_DIRS})
  list(APPEND EXTMODEM_LIBS ${PORTAUDIO_LIBRARIES})
endif(PORTAUDIO2_FOUND)

### HAMLIB ###

find_package(hamlib)

if(hamlib_FOUND)
  add_definitions(-DHAMLIB_FOUND)
  include_directories(${hamlib_INCLUDE_DIRS})
  list(APPEND EXTMODEM_LIBS ${hamlib_LIBRARIES})
endif(hamlib_FOUND)

###

file(GLOB_RECURSE EXTMODEM_SOURCES "src/*.cpp")
file(GLOB_RECURSE EXTMODEM_HEADERS "src/*.h")

add_executable(${PROJECT_NAME} ${EXTMODEM_SOURCES})

target_link_libraries(${PROJECT_NAME} ${EXTMODEM_LIBS} )

