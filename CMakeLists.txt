cmake_minimum_required(VERSION 3.25)
project(DestructorEspacial)

set(CMAKE_CXX_STANDARD 17)

set(SFML_DIR "C:/SFML/SFML-2.6.2/lib/cmake/SFML")

find_package(SFML 2.5 REQUIRED COMPONENTS graphics window system audio PATHS ${SFML_DIR} NO_DEFAULT_PATH)

add_executable(DestructorEspacial
        Funciones/Principal.cpp
        Funciones/Juego.cpp
        Funciones/Jugador.cpp
        Funciones/Enemigo.cpp
        Funciones/Bala.cpp
)

target_link_libraries(DestructorEspacial
        sfml-graphics
        sfml-window
        sfml-system
        sfml-audio
)