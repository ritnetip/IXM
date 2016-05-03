cmake_minimum_required(VERSION 2.8)

INCLUDE_DIRECTORIES("/usr/local/include/opencv2")
INCLUDE_DIRECTORIES("/usr/local/include/opencv")

add_compile_options(-std=c++11)

project( IXM )
find_package( OpenCV REQUIRED )

set( NAME_SRC
        src/main.cpp
        src/VideoWrapper.cpp
        src/VideoWindow.cpp
)

set( NAME_HEADERS       
        include/VideoWrapper.hpp
        include/VideoWindow.hpp
)

add_executable( IXM ${NAME_SRC} ${NAME_HEADERS} )
target_link_libraries( IXM ${OpenCV_LIBS} )
