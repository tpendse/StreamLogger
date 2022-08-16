# set the project name
project(StreamLogger)

file(GLOB SOURCES
        depends.h
        includes/includes.h
)

# add the executable
add_executable(StreamLogger main.cpp)
