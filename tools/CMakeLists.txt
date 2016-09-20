

EchoWithColor(COLOR GREEN "-- Configure Tools ${CMAKE_CURRENT_LIST_DIR}")

# ============ TOOLS - [...] ============
include_directories(${PROJECT_TOOLS_INC_DIR})

# ================ multi thread ================
if ( NOT MSVC )
    add_definitions(-D_POSIX_MT_)
    set (PROJECT_TOOLS_LIB_LINK ${PROJECT_TOOLS_LIB_LINK} pthread)
endif()


file(GLOB TOOLS_SRC_LIST
    ${PROJECT_TOOLS_SRC_DIR}/*.cpp
    ${PROJECT_TOOLS_SRC_DIR}/*.cc
    ${PROJECT_TOOLS_SRC_DIR}/*.c
    ${PROJECT_TOOLS_SRC_DIR}/*.cxx
)

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/tools")

if (MINGW)
    list(APPEND EXTENTION_LINK_LIB stdc++)
endif()

foreach(TOOLS_SRC_FILE IN LISTS TOOLS_SRC_LIST)
    get_filename_component(TOOLS_SRC_BIN_NAME "${TOOLS_SRC_FILE}" NAME_WE)

    add_executable("${TOOLS_SRC_BIN_NAME}" ${TOOLS_SRC_FILE})
    target_link_libraries("${TOOLS_SRC_BIN_NAME}"
	    ${PROJECT_LIB_LINK}
		${PROJECT_TOOLS_LIB_LINK}
        ${3RD_PARTY_LIBUV_LINK_NAME}
        ${3RD_PARTY_ATFRAME_UTILS_LINK_NAME}
        ${EXTENTION_LINK_LIB}
	)

	install(TARGETS "${TOOLS_SRC_BIN_NAME}"
        RUNTIME DESTINATION tools
        LIBRARY DESTINATION lib${PLATFORM_SUFFIX}
        ARCHIVE DESTINATION lib${PLATFORM_SUFFIX}
    )

endforeach()
