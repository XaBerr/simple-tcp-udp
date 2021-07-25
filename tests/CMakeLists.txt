set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include(FetchContent)
FetchContent_Declare(
  catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v2.13.4
)
FetchContent_MakeAvailable(catch2)
file(GLOB autopilot_sources "src/**.cpp")
SET(autopilot ${autopilot_sources})
add_executable(
  "${PROJECT_NAME}Test"
  test.cpp
  ${autopilot}
)
target_link_libraries(
  "${PROJECT_NAME}Test"
  PRIVATE
  ${PROJECT_NAME}
  ${SUBMODULES_NAME}
  Catch2::Catch2
)