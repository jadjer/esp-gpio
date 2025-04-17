cmake_minimum_required(VERSION 3.30.5)

set(SOURCES
        src/InputPin.cpp
        src/OutputPin.cpp
)

set(COMPONENTS
        driver
        esp_timer
)

idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS include REQUIRES ${COMPONENTS})
