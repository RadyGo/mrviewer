# Install script for directory: /media/gga/Datos/code/applications/mrViewer/dependencies/freeglut-3.0.0

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libglut.so.3.10.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libglut.so.3"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libglut.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/media/gga/Datos/code/applications/mrViewer/dependencies/freeglut-3.0.0/build-linux64/lib/libglut.so.3.10.0"
    "/media/gga/Datos/code/applications/mrViewer/dependencies/freeglut-3.0.0/build-linux64/lib/libglut.so.3"
    "/media/gga/Datos/code/applications/mrViewer/dependencies/freeglut-3.0.0/build-linux64/lib/libglut.so"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libglut.so.3.10.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libglut.so.3"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libglut.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/media/gga/Datos/code/applications/mrViewer/dependencies/freeglut-3.0.0/build-linux64/lib/libglut.a")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/GL" TYPE FILE FILES
    "/media/gga/Datos/code/applications/mrViewer/dependencies/freeglut-3.0.0/include/GL/freeglut.h"
    "/media/gga/Datos/code/applications/mrViewer/dependencies/freeglut-3.0.0/include/GL/freeglut_ext.h"
    "/media/gga/Datos/code/applications/mrViewer/dependencies/freeglut-3.0.0/include/GL/freeglut_std.h"
    "/media/gga/Datos/code/applications/mrViewer/dependencies/freeglut-3.0.0/include/GL/glut.h"
    )
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE RENAME "freeglut.pc" FILES "/media/gga/Datos/code/applications/mrViewer/dependencies/freeglut-3.0.0/build-linux64/freeglut.pc")
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/media/gga/Datos/code/applications/mrViewer/dependencies/freeglut-3.0.0/build-linux64/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
