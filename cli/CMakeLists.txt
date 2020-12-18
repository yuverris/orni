cmake_minimum_required(VERSION 3.14)

add_executable(cli main.cpp)

target_link_libraries(cli
        PUBLIC
            orni::orni
)
