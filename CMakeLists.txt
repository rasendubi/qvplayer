cmake_minimum_required(VERSION 2.8)

project(qvplayer)

set(qvplayer_SOURCES
    src/main.cpp
    src/qvplayer.cpp)

set(qvplayer_HEADERS
    src/qvplayer.h)

set(qvplayer_FORMS
    ui/qvplayer.ui)

set(qvplayer_QRC
    qrc/main.qrc)

set(QT_USE_QTWEBKIT TRUE)
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

find_package(Phonon REQUIRED)
include_directories(${PHONON_INCLUDES})

qt4_wrap_cpp(qvplayer_MOC_SOURCES ${qvplayer_HEADERS})
qt4_wrap_ui(qvplayer_UI_SOURCES ${qvplayer_FORMS})
qt4_add_resources(qvplayer_RESOURCES ${qvplayer_QRC})

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(qvplayer ${qvplayer_SOURCES} ${qvplayer_MOC_SOURCES} ${qvplayer_UI_SOURCES}
                        ${qvplayer_HEADERS} ${qvplayer_RESOURCES})
target_link_libraries(qvplayer ${QT_LIBRARIES} qtvk ${PHONON_LIBRARY})

install(TARGETS qvplayer 
        RUNTIME DESTINATION bin)
install(FILES qrc/qvplayer.desktop
        DESTINATION share/applications)
install(FILES qrc/img/qvplayer.png
        DESTINATION share/pixmaps)
        
include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_NAME "qvplayer")
set(CPACK_PACKAGE_VENDOR "Alexey Shmalko")
set(CPACK_PACKAGE_VERSION "0.1.3")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Vkontakte audio player.")
set(CPACK_PACKAGE_DESCRIPTION "Vkontakte audio player.")
set(CPACK_GENERATOR "DEB;TBZ2")

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Alexey Shmalko")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqtvk (= 0.1.2), phonon, libqt4-network, libqtcore4, libqtgui4")
set(DEBIAN_PACKAGE_BUILDS_DEPENDS "libphonon-dev")
set(CPACK_DEBIAN_PACKAGE_SECTION "Sound")

include(CPack)