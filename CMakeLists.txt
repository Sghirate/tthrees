cmake_minimum_required(VERSION 3.10)

# Compiler setup
set_property(GLOBAL PROPERTY CXX_EXCEPTIONS OFF)
set_property(GLOBAL PROPERTY CXX_RTTI OFF)
set_property(GLOBAL PROPERTY CXX_STATIC_RUNTIME OFF)
set_property(GLOBAL PROPERTY CXX_WARNINGS ON)
set_property(GLOBAL PROPERTY CXX_WARNINGS_AS_ERRORS ON)
if(MSVC)
  # Purge unconditional use of /MDd, /MD and /EHsc.
  foreach(flag
          CMAKE_C_FLAGS                CMAKE_CXX_FLAGS
          CMAKE_C_FLAGS_DEBUG          CMAKE_CXX_FLAGS_DEBUG
          CMAKE_C_FLAGS_RELEASE        CMAKE_CXX_FLAGS_RELEASE
          CMAKE_C_FLAGS_MINSIZEREL     CMAKE_CXX_FLAGS_MINSIZEREL
          CMAKE_C_FLAGS_RELWITHDEBINFO CMAKE_CXX_FLAGS_RELWITHDEBINFO
          )
    string(REPLACE "/MDd"  "" ${flag} "${${flag}}")
    string(REPLACE "/MD"   "" ${flag} "${${flag}}")
    string(REPLACE "/EHsc" "" ${flag} "${${flag}}")
    string(REPLACE "/GR" "" ${flag} "${${flag}}")
  endforeach()
  # Restore those same, but now selected by the properties
  add_compile_options(
    $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_EXCEPTIONS>>,ON>:/EHsc>
    $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_RTTI>>,OFF>:/GR->
    $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_STATIC_RUNTIME>>,OFF>:$<$<CONFIG:Debug>:/MDd>$<$<NOT:$<CONFIG:Debug>>:/MD>>
    $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_STATIC_RUNTIME>>,ON>:$<$<CONFIG:Debug>:/MTd>$<$<NOT:$<CONFIG:Debug>>:/MT>>
    $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_WARNINGS>>,ON>:/W3>
    $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_WARNINGS>>,OFF>:/W0>
    $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_WARNINGS>>,ALL>:/W4>
    $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_WARNINGS_AS_ERRORS>>,ON>:/WX>
  )
else()
  add_compile_options(
    $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_EXCEPTIONS>>,OFF>:-fno-exceptions>
    $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_RTTI>>,OFF>:-fno-rtti>
    $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_STATIC_RUNTIME>>,ON>:-static>
    $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_WARNINGS>>,ON>:-Wall>
    $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_WARNINGS>>,OFF>:-w>
    $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_WARNINGS>>,ALL>:-Wall>
    $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_WARNINGS>>,ALL>:-pedantic>
    $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_WARNINGS_AS_ERRORS>>,ON>:-Werror>
  )
  if (CMAKE_${COMPILER}_COMPILER_ID MATCHES "Clang")
    add_compile_options(
      $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_WARNINGS>>,ALL>:-Weverything>
      $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_WARNINGS>>,ALL>:-Wno-c++98-compat>
      $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_WARNINGS>>,ALL>:-Wno-c++98-compat-pedantic>
    )
  endif()
endif()
# ~Compiler setup

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

set_target_properties(tthrees PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

if(THREES_NCURSES)
	target_link_libraries(tthrees PUBLIC
		ncurses)
endif()

target_include_directories(tthrees PRIVATE
	"${PROJECT_SOURCE_DIR}/src"
)
