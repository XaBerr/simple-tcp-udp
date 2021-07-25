set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_executable(
  "${PROJECT_NAME}App"
  example.cpp
)
target_link_libraries(
  "${PROJECT_NAME}App"
  ${PROJECT_NAME}
  ${SUBMODULES_NAME}
)