cmake_minimum_required(VERSION 3.10)

project(tthrees)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

if (WIN32)
	set(TTHREES_NCURSES_DEFAULT OFF)
else()
	set(TTHREES_NCURSES_DEFAULT ON)
endif()
option(THREES_NCURSES "use ncurses backend" ${TTHREES_NCURSES_DEFAULT})
if (THREES_NCURSES)
	find_package(Curses)
	CHECK_LIBRARY_EXISTS("${CURSES_NCURSES_LIBRARY}"
						 nodelay "" CURSES_NCURSES_HAS_NODELAY)
	if(NOT CURSES_NCURSES_HAS_NODELAY)
			find_library(CURSES_EXTRA_LIBRARY tinfo)
			CHECK_LIBRARY_EXISTS("${CURSES_EXTRA_LIBRARY}"
									nodelay "" CURSES_TINFO_HAS_NODELAY)
	endif()
	if(CURSES_EXTRA_LIBRARY)
			set(CURSES_LIBRARIES ${CURSES_LIBRARIES} ${CURSES_EXTRA_LIBRARY})
	endif()
endif()
if (Curses_FOUND)
	add_compile_definitions(HAS_NCURSES)
else()
	set(THREES_NCURSES OFF)
endif()
	
file(GLOB_RECURSE tthrees_FILES
	"${PROJECT_SOURCE_DIR}/src/*.h"
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
)
	
add_executable(tthrees
	${tthrees_FILES}
)

target_compile_features(tthrees PUBLIC 
	cxx_std_17
)

if(THREES_NCURSES)
	target_link_libraries(tthrees PUBLIC
		ncurses)
endif()

target_include_directories(tthrees PRIVATE
	"${PROJECT_SOURCE_DIR}/src"
)
