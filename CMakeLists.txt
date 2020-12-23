cmake_minimum_required(VERSION 3.14)

project(orni
        VERSION
            0.2.3
        DESCRIPTION
            "A minimalist & lightweight web framework for modern C++ inspired from expressjs."
        LANGUAGES
            CXX
)

set(default_build_type "Release")

option(WITH_EXAMPLES "Compile the project with examples." ON)
option(WITH_CLI "Compile the project with the CLI utility." ON)

add_subdirectory(src)

if(WITH_EXAMPLES)
    add_subdirectory(examples)
endif()

if(WITH_CLI)
    add_subdirectory(cli)
endif()
