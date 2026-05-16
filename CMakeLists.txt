cmake_minimum_required(VERSION 3.25)
project(DestructorEspacial)

set(CMAKE_CXX_STANDARD 17)

set(SFML_DIR "C:/Users/edgar/Downloads/SFML-2.5.1-windows-gcc-7.3.0-mingw-64-bit/SFML-2.5.1/lib/cmake/SFML")

find_package(SFML 2.5 REQUIRED COMPONENTS graphics window system audio)

add_executable(DestructorEspacial
        Principal.cpp
        Juego.cpp
        Jugador.cpp
        Enemigo.cpp
        Bala.cpp
)

target_link_libraries(DestructorEspacial
        sfml-graphics
        sfml-window
        sfml-system
        sfml-audio
)