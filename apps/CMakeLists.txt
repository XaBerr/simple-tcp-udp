set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_executable("exampleServer" exampleServer.cpp)
target_link_libraries("exampleServer" ${PROJECT_NAME} ${SUBMODULES_NAME})
add_executable("exampleClient" exampleClient.cpp)
target_link_libraries("exampleClient" ${PROJECT_NAME} ${SUBMODULES_NAME})