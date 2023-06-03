# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.8)

set(PROJNAME VerilogC)

project (${PROJNAME} LANGUAGES CXX)

# include output dir, to include the generated files is there are
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# set qt path
set(CMAKE_PREFIX_PATH $ENV{QT_DIRS})
message(STATUS env:$ENV{QT_DIRS})

# find packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)

# open Qt compiler
set(CMAKE_AUTOMOC ON)	# for meta-obj
set(CMAKE_AUTORCC ON) 	# for resource
set(CMAKE_AUTOUIC ON)	# for ui

# build
aux_source_directory(elaborate	elab_src)
aux_source_directory(netlistDB	ndb_src)
aux_source_directory(parse		parse_src)
aux_source_directory(tree		tree_src)
aux_source_directory(utils		util_src)
aux_source_directory(gui		gui_src)
aux_source_directory(app		app_src)


include_directories(elaborate netlistDB parse tree utils gui app)

add_executable(${PROJNAME} ${app_src} ${elab_src} ${gui_src} ${parse_src} ${util_src} ${tree_src} ${ndb_src} "app/app.cpp" "app/app.h")
target_link_libraries(${PROJNAME}
	PUBLIC
	Qt6::Core
	Qt6::Gui
	Qt6::Widgets
)