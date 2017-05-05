cmake_minimum_required(VERSION 3.3.0)
cmake_policy(SET CMP0054 NEW)

project(libatbus)

# 准备下载依赖组件
include ("${CMAKE_CURRENT_LIST_DIR}/3rd_party/atframe_utils/libatframe_utils.prepare.cmake")

########################################################################
# CMake 模块 (递归包含模块, 带颜色输出模块, 平台检测模块)
set (PROJECT_CMAKE_MODULE_DIR "${3RD_PARTY_ATFRAME_UTILS_PKG_DIR}/project/cmake")
list(APPEND CMAKE_MODULE_PATH "${PROJECT_CMAKE_MODULE_DIR}/modules")


include("${PROJECT_CMAKE_MODULE_DIR}/modules/IncludeDirectoryRecurse.cmake")
include("${PROJECT_CMAKE_MODULE_DIR}/modules/EchoWithColor.cmake")
include("${PROJECT_CMAKE_MODULE_DIR}/modules/FindConfigurePackage.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/project/cmake/ProjectBuildOption.cmake")
include("${PROJECT_CMAKE_MODULE_DIR}/FindPlatform.cmake")
include("${PROJECT_CMAKE_MODULE_DIR}/ProjectTools.cmake")


#####################################################################
# 导入编译器和编译选项配置
include("${PROJECT_CMAKE_MODULE_DIR}/CompilerOption.cmake")
include("${PROJECT_CMAKE_MODULE_DIR}/TargetOption.cmake")
EchoWithColor(COLOR GREEN "-- Build Type: ${CMAKE_BUILD_TYPE}")

########################################################################
# 导入项目配置
## 导入所有 macro 定义
include_macro_recurse(${CMAKE_CURRENT_LIST_DIR})

## 导入工程项目
if ( "${CMAKE_CURRENT_LIST_DIR}/3rd_party/atframe_utils/repo" STREQUAL ${3RD_PARTY_ATFRAME_UTILS_PKG_DIR})
    add_subdirectory(${3RD_PARTY_ATFRAME_UTILS_PKG_DIR})
endif()
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/src")

if (PROJECT_ENABLE_SAMPLE)
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/sample")
endif()

if (PROJECT_ENABLE_UNITTEST)
    include ("${3RD_PARTY_ATFRAME_UTILS_PKG_DIR}/test/test.utils-macro.cmake")
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test")
endif()

if (PROJECT_ENABLE_TOOLS)
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/tools")
endif()