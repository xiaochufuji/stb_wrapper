# CMakeList.txt: stb_wrapper 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#

if (CMAKE_VERSION VERSION_GREATER 3.12)
	set(CMAKE_CXX_STANDARD 20)
endif()
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
# 将源代码添加到此项目的可执行文件。
add_library(stb_wrapper SHARED "stb_wrapper.cpp")
target_include_directories(
	stb_wrapper 
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin")
add_executable(main "main.cpp")
set_target_properties(
	main PROPERTIES
	INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include
	LINK_LIBRARIES stb_wrapper
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

target_compile_options(stb_wrapper PRIVATE $<$<CONFIG:Release>:-O2>)
# TODO: 如有需要，请添加测试并安装目标。
