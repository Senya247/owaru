# minimum CMake version required
cmake_minimum_required(VERSION 3.15)

project(owaru VERSION 1.0 DESCRIPTION "Owaru")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
 
include(FetchContent)

FetchContent_Declare(dpp
    GIT_REPOSITORY      https://github.com/brainboxdotcc/DPP.git
    GIT_TAG             master
) 

FetchContent_Declare(cpr
    GIT_REPOSITORY      https://github.com/libcpr/cpr.git
    GIT_TAG             master
) 

FetchContent_Declare(fmt
    GIT_REPOSITORY      https://github.com/fmtlib/fmt.git
    GIT_TAG             master
) 

FetchContent_Declare(json
    GIT_REPOSITORY      https://github.com/nlohmann/json.git
    GIT_TAG             master
) 

FetchContent_Declare(cpp-dotenv
    GIT_REPOSITORY      https://github.com/Senya247/cpp-dotenv.git
    GIT_TAG             master
) 

FetchContent_Declare(rapidfuzz-cpp
    GIT_REPOSITORY      https://github.com/maxbachmann/rapidfuzz-cpp.git
    GIT_TAG             main
) 

FetchContent_MakeAvailable(dpp)
FetchContent_MakeAvailable(cpr)
FetchContent_MakeAvailable(fmt)
FetchContent_MakeAvailable(json)
FetchContent_MakeAvailable(cpp-dotenv)
FetchContent_MakeAvailable(rapidfuzz-cpp)

set(SOURCES
    main.cpp

    src/owaru/owaru.cpp
    src/owaru/commands/ping.cpp

    src/command/command.cpp

    src/rcon/rcon.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
 
target_link_libraries(${PROJECT_NAME}
    dpp
    cpr
    fmt
    nlohmann_json
    cpp_dotenv
    rapidfuzz
    -lmpg123
)
 
target_include_directories(${PROJECT_NAME} PRIVATE
    libs/DPP/include
    libs/cpr/include
    libs/fmt/include
    libs/json/include
    libs/cpp-dotenv/include
    libs/rapidfuzz-cpp/rapidfuzz

    include
)
 

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
