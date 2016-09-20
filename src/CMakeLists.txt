

EchoWithColor(COLOR GREEN "-- Configure ${CMAKE_CURRENT_LIST_DIR}")

include_directories(${PROJECT_ROOT_SRC_DIR})

file(RELATIVE_PATH PROJECT_ROOT_RELINC_DIR ${CMAKE_CURRENT_LIST_DIR} ${PROJECT_ROOT_INC_DIR})

file(GLOB_RECURSE PROJECT_LIB_SRC_LIST
    ${PROJECT_ROOT_RELINC_DIR}/*.h
    ${PROJECT_ROOT_RELINC_DIR}/*.hpp
    ${PROJECT_ROOT_RELINC_DIR}/*.hxx
    ${PROJECT_ROOT_SRC_DIR}/*.h
    ${PROJECT_ROOT_SRC_DIR}/*.hpp
    ${PROJECT_ROOT_SRC_DIR}/*.c
    ${PROJECT_ROOT_SRC_DIR}/*.cpp
    ${PROJECT_ROOT_SRC_DIR}/*.cc
    ${PROJECT_ROOT_SRC_DIR}/*.cxx
)
source_group_by_dir(PROJECT_LIB_SRC_LIST)


# ================ multi thread ================
if ( NOT MSVC )
    add_definitions(-D_POSIX_MT_)
endif()

# ============ libatbus - src ============
add_library(${PROJECT_LIB_LINK} ${PROJECT_LIB_SRC_LIST})

install(TARGETS ${PROJECT_LIB_LINK}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib${PLATFORM_SUFFIX}
    ARCHIVE DESTINATION lib${PLATFORM_SUFFIX}
)

install(DIRECTORY ${PROJECT_ROOT_INC_DIR}
    DESTINATION .
    FILES_MATCHING REGEX ".+\\.h(pp)?$"
    PATTERN ".svn" EXCLUDE
    PATTERN ".git" EXCLUDE
)
