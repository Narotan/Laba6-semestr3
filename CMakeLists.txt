cmake_minimum_required(VERSION 3.14)
project(lab6)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PostgreSQL_INCLUDE_DIR "/opt/homebrew/include/postgresql@14")
set(PostgreSQL_LIB_DIR "/opt/homebrew/lib/postgresql@14")

include_directories(${PostgreSQL_INCLUDE_DIR})
link_directories(${PostgreSQL_LIB_DIR})

add_executable(lab6
    src/main.cpp
    src/db_manager.cpp
)

# Link libpq
target_link_libraries(lab6 pq)
