cmake_minimum_required(VERSION 3.25)
project(DestructorEspacial)

set(CMAKE_CXX_STANDARD 17)

set(SFML_DIR "C:/SFML/SFML-2.6.2/lib/cmake/SFML")
set(CURL_INCLUDE_DIR "C:/curl/include")
set(CURL_LIBRARY "C:/curl/lib/libcurl.dll.a")

find_package(SFML 2.5 REQUIRED COMPONENTS graphics window system audio)

add_executable(DestructorEspacial
        Logica/Principal.cpp
        Logica/Juego.cpp
        Logica/Jugador.cpp
        Logica/Enemigo.cpp
        Logica/Bala.cpp
        Logica/json.hpp
        Logica/BalaEnemiga.cpp
        logica/ApiClient.cpp
        Heders/BalaEnemiga.h

)

target_include_directories(DestructorEspacial PRIVATE
        ${CMAKE_SOURCE_DIR}/Heders
        ${CURL_INCLUDE_DIR}
)

target_link_libraries(DestructorEspacial
        sfml-graphics
        sfml-window
        sfml-system
        sfml-audio
        ${CURL_LIBRARY}

)

add_custom_command(TARGET DestructorEspacial POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "C:/SFML/SFML-2.6.2/bin"
        $<TARGET_FILE_DIR:DestructorEspacial>
)