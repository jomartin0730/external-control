set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Core Network REQUIRED)

add_library(${PROJECT_NAME}-core
        client.cpp
)
target_link_libraries(${PROJECT_NAME}-core PUBLIC
        Qt5::Core
        Qt5::Network
)
