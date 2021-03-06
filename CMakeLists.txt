cmake_minimum_required(VERSION 2.8)

project(qvplayer)

set(qvplayer_MAJOR_VERSION 0)
set(qvplayer_MINOR_VERSION 2)
set(qvplayer_PATCH_VERSION 1)
set(qvplayer_VERSION
  "${qvplayer_MAJOR_VERSION}.${qvplayer_MINOR_VERSION}.${qvplayer_PATCH_VERSION}")

set(qvplayer_SOURCES
    src/main.cpp
    src/qvplayer.cpp)

set(qvplayer_HEADERS
    src/qvplayer.h)

set(qvplayer_FORMS
    ui/qvplayer.ui)

set(qvplayer_QRC
    qrc/main.qrc)

set(qvplayer_TRANSLATIONS
    ts/de.ts
    ts/en.ts
    ts/ru.ts
    ts/ua.ts)    
    
set(QT_USE_QTWEBKIT TRUE)
set(QT_USE_PHONON TRUE)
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

find_package(Qtvk REQUIRED)
include_directories(${Qtvk_INCLUDE_DIR})
link_directories(${Qtvk_LIBRARY_DIR})

qt4_wrap_cpp(qvplayer_MOC_SOURCES ${qvplayer_HEADERS})
qt4_wrap_ui(qvplayer_UI_SOURCES ${qvplayer_FORMS})
qt4_add_resources(qvplayer_RESOURCES ${qvplayer_QRC})
qt4_add_translation(qvplayer_QM ${qvplayer_TRANSLATIONS})

if(WIN32 AND NOT CYGWIN)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")
endif()

if( CLOSE_INSTEAD_OF_HIDE )
  add_definitions(-DCLOSE_INSTEAD_OF_HIDE)
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(qvplayer ${qvplayer_SOURCES} ${qvplayer_MOC_SOURCES} ${qvplayer_UI_SOURCES}
                        ${qvplayer_HEADERS} ${qvplayer_RESOURCES} ${qvplayer_QM})
target_link_libraries(qvplayer ${QT_LIBRARIES} ${Qtvk_LIBRARIES} ${PHONON_LIBRARY})

install(TARGETS qvplayer 
        RUNTIME DESTINATION bin)
install(FILES qrc/qvplayer.desktop
        DESTINATION share/applications)
install(FILES qrc/img/qvplayer.png
        DESTINATION share/pixmaps)
        
include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_NAME "qvplayer")
set(CPACK_PACKAGE_VENDOR "Alexey Shmalko")
set(CPACK_PACKAGE_VERSION ${qvplayer_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Vkontakte audio player.")
set(CPACK_PACKAGE_DESCRIPTION "Vkontakte audio player.")
set(CPACK_GENERATOR "DEB;TBZ2")

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Alexey Shmalko")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqtvk (= 0.1.2), phonon, libqt4-network, libqtcore4, libqtgui4")
set(DEBIAN_PACKAGE_BUILDS_DEPENDS "libphonon-dev")
set(CPACK_DEBIAN_PACKAGE_SECTION "Sound")

include(CPack)
