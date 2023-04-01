# minimum CMake version required
cmake_minimum_required(VERSION 3.15)
# Project name, version and description
project(owaru VERSION 1.0 DESCRIPTION "A Discord Bot")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
 
# Add DPP as dependency
add_subdirectory(libs/DPP)
add_subdirectory(libs/cpr)
add_subdirectory(libs/fmt)
add_subdirectory(libs/json)
add_subdirectory(libs/cpp-dotenv)
add_subdirectory(libs/rapidfuzz-cpp)
# You can also add any other libs you want to use
 
set(SOURCES
    main.cpp

    src/owaru/commands.cpp
    src/owaru/owaru.cpp

    src/command/command.cpp

    src/rcon/rcon.cpp
)

# Create an executable
add_executable(${PROJECT_NAME} ${SOURCES})
add_executable(test test.cpp)
 
# Linking libraries
target_link_libraries(${PROJECT_NAME}
    dpp
    cpr
    fmt
    nlohmann_json
    cpp_dotenv
    rapidfuzz
    -lmpg123
)
 
# Specify includes
target_include_directories(${PROJECT_NAME} PRIVATE
    libs/DPP/include
    libs/cpr/include
    libs/fmt/include
    libs/json/include
    libs/cpp-dotenv/include
    libs/rapidfuzz-cpp/rapidfuzz

    include
)
 
target_include_directories(test PRIVATE include)

# Set C++ version
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
