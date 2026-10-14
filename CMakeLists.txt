cmake_minimum_required(VERSION 3.23)

project(dublang C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(dub)

target_sources(dub
	PRIVATE
	ast.c
	lexer.c
	eval.c
	logging.c
	state.c
	main.c
)

target_compile_options(dub PRIVATE -g -O0)
