cmake_minimum_required(VERSION 3.1)
project(bin-dump-sql-text)

# Boost
find_package( Boost 1.61.0 COMPONENTS system program_options )
include_directories( ${Boost_INCLUDE_DIR} )

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    find_package( lib-dump )
    find_package( lib-dump-sql )
    find_package( lib-sql )
    find_package( lib-sql-text )
endif()


# SQL interpreter binary
set(BINARY_SOURCE_FILES
        src/frontend.cpp)

add_executable(dump-sql-text ${BINARY_SOURCE_FILES})
target_link_libraries(dump-sql-text
                      sql sql_text_parser
                      dump dump_sql_dumper
                      ${Boost_LIBRARIES})

set_property(TARGET dump-sql-text PROPERTY CXX_STANDARD 14)
set_property(TARGET dump-sql-text PROPERTY CXX_STANDARD_REQUIRED ON)
