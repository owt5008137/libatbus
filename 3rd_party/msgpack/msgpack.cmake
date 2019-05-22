if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.10")
    include_guard(GLOBAL)
endif()

# =========== 3rdparty msgpack ==================
if(NOT 3RD_PARTY_MSGPACK_BASE_DIR)
    set (3RD_PARTY_MSGPACK_BASE_DIR ${CMAKE_CURRENT_LIST_DIR})
endif()
set (3RD_PARTY_MSGPACK_PKG_DIR "${3RD_PARTY_MSGPACK_BASE_DIR}/pkg")

set (3RD_PARTY_MSGPACK_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/prebuilt/${PLATFORM_BUILD_PLATFORM_NAME}")

set(3RD_PARTY_MSGPACK_VERSION "3.1.1")
set(3RD_PARTY_MSGPACK_MSVC_PATCH "master")

if (Msgpack_ROOT)
    set(MSGPACK_ROOT ${Msgpack_ROOT})
endif()

if (MSGPACK_ROOT)
    set(MSGPACK_INCLUDE_DIRS "${MSGPACK_ROOT}/include")

    if (EXISTS "${MSGPACK_INCLUDE_DIRS}/msgpack.hpp")
        set(MSGPACK_FOUND YES)
    endif()
endif()

if(NOT MSGPACK_FOUND)
    if(NOT EXISTS ${3RD_PARTY_MSGPACK_PKG_DIR})
        message(STATUS "mkdir 3RD_PARTY_MSGPACK_PKG_DIR=${3RD_PARTY_MSGPACK_PKG_DIR}")
        file(MAKE_DIRECTORY ${3RD_PARTY_MSGPACK_PKG_DIR})
    endif()

    if(NOT EXISTS "${3RD_PARTY_MSGPACK_ROOT_DIR}/include/msgpack.hpp")
        if (MSVC)
            find_package(Git)
            if (NOT GIT_FOUND)
                message(FATAL_ERROR "git is required for fetch msgpack-c")
            endif()

            if (EXISTS "${3RD_PARTY_MSGPACK_PKG_DIR}/msgpack-${3RD_PARTY_MSGPACK_MSVC_PATCH}")
                execute_process(COMMAND ${GIT_EXECUTABLE} reset --hard
                    WORKING_DIRECTORY "${3RD_PARTY_MSGPACK_PKG_DIR}/msgpack-${3RD_PARTY_MSGPACK_MSVC_PATCH}"
                )
            else()
                execute_process(COMMAND ${GIT_EXECUTABLE} clone --depth=1 -b ${3RD_PARTY_MSGPACK_MSVC_PATCH} "https://github.com/owent-contrib/msgpack-c.git" "msgpack-${3RD_PARTY_MSGPACK_MSVC_PATCH}"
                    WORKING_DIRECTORY ${3RD_PARTY_MSGPACK_PKG_DIR}
                )
            endif()
        else()
            if(NOT EXISTS "${3RD_PARTY_MSGPACK_PKG_DIR}/msgpack-${3RD_PARTY_MSGPACK_VERSION}.tar.gz")
                FindConfigurePackageDownloadFile("https://github.com/msgpack/msgpack-c/releases/download/cpp-${3RD_PARTY_MSGPACK_VERSION}/msgpack-${3RD_PARTY_MSGPACK_VERSION}.tar.gz" "${3RD_PARTY_MSGPACK_PKG_DIR}/msgpack-${3RD_PARTY_MSGPACK_VERSION}.tar.gz")
            endif()
            find_program(TAR_EXECUTABLE tar)
            if(APPLE)
                execute_process(COMMAND ${TAR_EXECUTABLE} -xvf "msgpack-${3RD_PARTY_MSGPACK_VERSION}.tar.gz"
                    WORKING_DIRECTORY ${3RD_PARTY_MSGPACK_PKG_DIR}
                )
            else()
                execute_process(COMMAND ${TAR_EXECUTABLE} -axvf "msgpack-${3RD_PARTY_MSGPACK_VERSION}.tar.gz"
                    WORKING_DIRECTORY ${3RD_PARTY_MSGPACK_PKG_DIR}
                )
            endif()
        endif()
    endif()

    if(NOT EXISTS "${3RD_PARTY_MSGPACK_ROOT_DIR}/include/msgpack.hpp")
        file(MAKE_DIRECTORY ${3RD_PARTY_MSGPACK_ROOT_DIR})
        if (EXISTS "${3RD_PARTY_MSGPACK_PKG_DIR}/msgpack-${3RD_PARTY_MSGPACK_VERSION}")
            file(RENAME "${3RD_PARTY_MSGPACK_PKG_DIR}/msgpack-${3RD_PARTY_MSGPACK_VERSION}/include" "${3RD_PARTY_MSGPACK_ROOT_DIR}/include")
        else()
            file(RENAME "${3RD_PARTY_MSGPACK_PKG_DIR}/msgpack-${3RD_PARTY_MSGPACK_MSVC_PATCH}/include" "${3RD_PARTY_MSGPACK_ROOT_DIR}/include")
        endif()
    endif()

    set(MSGPACK_INCLUDE_DIRS "${3RD_PARTY_MSGPACK_ROOT_DIR}/include")
    if (EXISTS "${MSGPACK_INCLUDE_DIRS}/msgpack.hpp")
        set(MSGPACK_FOUND YES)
        set(MSGPACK_ROOT ${3RD_PARTY_MSGPACK_ROOT_DIR})
    endif()
endif()

if(MSGPACK_FOUND)
    EchoWithColor(COLOR GREEN "-- Dependency: Msgpack found.(${MSGPACK_INCLUDE_DIRS})")
else()
    EchoWithColor(COLOR RED "-- Dependency: Msgpack is required")
    message(FATAL_ERROR "Msgpack not found")
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if (NOT CMAKE_CXX_STANDARD OR CMAKE_CXX_STANDARD LESS 11)
        add_definitions(-Wno-pragmas)
    endif()
endif()

set (3RD_PARTY_MSGPACK_INC_DIR ${MSGPACK_INCLUDE_DIRS})

include_directories(${3RD_PARTY_MSGPACK_INC_DIR})
