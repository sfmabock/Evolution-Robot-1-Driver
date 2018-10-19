cmake_minimum_required(VERSION 2.8.3)
project(er1driver)

# 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



set(er1driver_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include/er1driver/)

set (er1driver_HEADERS 
${er1driver_INCLUDE_DIR}/commandquery.hpp
${er1driver_INCLUDE_DIR}/commandresponse.hpp
${er1driver_INCLUDE_DIR}/controlmodule.hpp
${er1driver_INCLUDE_DIR}/er1driver.hpp
${er1driver_INCLUDE_DIR}/misc.hpp
${er1driver_INCLUDE_DIR}/result.hpp
${er1driver_INCLUDE_DIR}/stringify.hpp)

set(er1driver_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src/)

set(er1driver_SOURCES 
${er1driver_SOURCE_DIR}/commandquery.cpp     
${er1driver_SOURCE_DIR}/controlmodule.cpp  
${er1driver_SOURCE_DIR}/er1driver.cpp
${er1driver_SOURCE_DIR}/commandresponse.cpp
${er1driver_SOURCE_DIR}/misc.cpp
${er1driver_SOURCE_DIR}/result.cpp
)

add_library(er1driver ${er1driver_SOURCES} ${er1driver_HEADERS})
target_include_directories(er1driver PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

if (${ER1DRIVER_BUILD_CONTROLAPP})
    set(SDL2_LIBRARY /usr/lib/x86_64-linux-gnu/libSDL2-2.0.so)
    set(SDL2_INCLUDE_DIR /usr/include/})
    include_directories(${SDL2_INCLUDE_DIR})
    set(er1controlapp_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/er1controlapp.cpp)
    add_executable(er1controlapp ${er1controlapp_SOURCES})
    add_dependencies(er1controlapp er1driver)
    target_link_libraries(er1controlapp PUBLIC er1driver ${SDL2_LIBRARY})
    target_include_directories(er1controlapp PUBLIC ${SDL2_INCLUDE_DIR})
endif()



