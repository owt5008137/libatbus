if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.10")
    include_guard(GLOBAL)
endif()

# =========== 3rdparty libuv ==================
if(NOT 3RD_PARTY_LIBUV_BASE_DIR)
    set (3RD_PARTY_LIBUV_BASE_DIR ${CMAKE_CURRENT_LIST_DIR})
endif()

set (3RD_PARTY_LIBUV_DEFAULT_VERSION "v1.28.0")

if (NOT Libuv_ROOT AND NOT LIBUV_ROOT AND EXISTS "${3RD_PARTY_LIBUV_BASE_DIR}/prebuilt/include")
    set (Libuv_ROOT "${3RD_PARTY_LIBUV_BASE_DIR}/prebuilt/${PLATFORM_BUILD_PLATFORM_NAME}")
endif ()

find_package(Libuv)
if(Libuv_FOUND)
    EchoWithColor(COLOR GREEN "-- Dependency: Libuv prebuilt found.(${Libuv_LIBRARIES})")
else()
    set (Libuv_ROOT "${3RD_PARTY_LIBUV_BASE_DIR}/prebuilt")
    set (3RD_PARTY_LIBUV_REPO_DIR "${3RD_PARTY_LIBUV_BASE_DIR}/repo-${3RD_PARTY_LIBUV_DEFAULT_VERSION}")
    if (NOT EXISTS ${3RD_PARTY_LIBUV_REPO_DIR})
        find_package(Git)
        execute_process(COMMAND ${GIT_EXECUTABLE} clone --depth=1 -b ${3RD_PARTY_LIBUV_DEFAULT_VERSION} "https://github.com/libuv/libuv.git" ${3RD_PARTY_LIBUV_REPO_DIR}
            WORKING_DIRECTORY ${3RD_PARTY_ATFRAME_UTILS_BASE_DIR}
        )
    endif ()

    if (EXISTS "${3RD_PARTY_LIBUV_REPO_DIR}/CMakeLists.txt")
        file(MAKE_DIRECTORY "${3RD_PARTY_LIBUV_REPO_DIR}/build_obj_dir")
        execute_process(COMMAND 
            ${CMAKE_COMMAND} ${3RD_PARTY_LIBUV_REPO_DIR} "-DCMAKE_INSTALL_PREFIX=${Libuv_ROOT}"
            "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}" "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
            "-DCMAKE_C_FLAGS=-fPIC ${CMAKE_C_FLAGS}" "-DCMAKE_C_FLAGS_DEBUG=${CMAKE_C_FLAGS_DEBUG}"
            "-DCMAKE_C_FLAGS_RELEASE=${CMAKE_C_FLAGS_RELEASE}" "-DCMAKE_C_FLAGS_RELWITHDEBINFO=${CMAKE_C_FLAGS_RELWITHDEBINFO}"
            "-DCMAKE_C_FLAGS_MINSIZEREL=${CMAKE_C_FLAGS_MINSIZEREL}" "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
            WORKING_DIRECTORY "${3RD_PARTY_LIBUV_REPO_DIR}/build_obj_dir"
        )
        execute_process(COMMAND ${CMAKE_COMMAND} --build . --target install --config ${CMAKE_BUILD_TYPE}
            WORKING_DIRECTORY "${3RD_PARTY_LIBUV_REPO_DIR}/build_obj_dir"
        )
        unset (Libuv_FOUND CACHE)
        find_package(Libuv)
    endif()
endif()

if (NOT Libuv_FOUND)
    EchoWithColor(COLOR RED "-- Dependency: Libuv is required, we can not find prebuilt for libuv and can not find git to clone the sources")
    message(FATAL_ERROR "Libuv not found")
endif()

set (3RD_PARTY_LIBUV_INC_DIR ${Libuv_INCLUDE_DIRS})
set (3RD_PARTY_LIBUV_LINK_NAME ${Libuv_LIBRARIES})

include_directories(${3RD_PARTY_LIBUV_INC_DIR})

# mingw
unset(3RD_PARTY_LIBUV_LINK_DEPS)
if (MINGW)
    EchoWithColor(COLOR GREEN "-- MinGW: custom add lib advapi32 iphlpapi psapi shell32 user32 userenv ws2_32.")
    list(APPEND 3RD_PARTY_LIBUV_LINK_DEPS advapi32 iphlpapi psapi shell32 user32 userenv ws2_32)
elseif (WIN32)
    EchoWithColor(COLOR GREEN "-- Win32: custom add lib advapi32 iphlpapi psapi shell32 user32 userenv ws2_32")
    list(APPEND 3RD_PARTY_LIBUV_LINK_DEPS advapi32 iphlpapi psapi shell32 user32 userenv ws2_32)
endif()
