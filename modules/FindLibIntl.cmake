#-*-cmake-*-
#
# Test for LIBINTL (Color Transform Language)
#
# Once loaded this will define
#  LIBINTL_FOUND        - system has OpenEXR
#  LIBINTL_INCLUDE_DIR  - include directory for OpenEXR
#  LIBINTL_LIBRARIES    - libraries you need to link to
#

SET(LIBINTL_FOUND "NO")

IF( LIBINTL_LIBRARY_DIR )
  SET( SEARCH_DIRS "${LIBINTL_LIBRARY_DIR}" )
ELSE( LIBINTL_LIBRARY_DIR )
  SET( SEARCH_DIRS 
    "$ENV{LIBINTL_ROOT}/lib/x${CMAKE_BUILD_ARCH}/Release"
    "$ENV{LIBINTL_ROOT}/lib/Win32/Release"
    "$ENV{LIBINTL_ROOT}/lib/Release"
    "$ENV{LIBINTL_ROOT}/lib"
    "$ENV{LIBINTL_ROOT}/lib/x${CMAKE_BUILD_ARCH}/Debug"
    "$ENV{LIBINTL_ROOT}/lib/Debug"
    "$ENV{LIBINTL_ROOT}/bin/x${CMAKE_BUILD_ARCH}/Release"
    "$ENV{LIBINTL_ROOT}/bin/Release"
    "$ENV{LIBINTL_ROOT}/bin/x${CMAKE_BUILD_ARCH}/Debug"
    "$ENV{LIBINTL_ROOT}/bin/Win32/Release"
    "$ENV{LIBINTL_ROOT}/bin/Debug"
    "$ENV{LIBINTL_ROOT}/bin"
    /usr/local/lib${CMAKE_BUILD_ARCH}
    /usr/lib${CMAKE_BUILD_ARCH}
    )
ENDIF( LIBINTL_LIBRARY_DIR )


FIND_PATH( LIBINTL_INCLUDE_DIR libintl.h
  "$ENV{LIBINTL_ROOT}/include"
  /usr/local/include/
  /usr/include/
  DOC   "libintl includes" 
 )

IF( WIN32 )
  FIND_LIBRARY( libintl_library
    NAMES libintl 
    PATHS ${SEARCH_DIRS}
    DOC   "LIBINTL library"
    )

  SET(LIBINTL_LIBRARIES ${libintl_library} )
ELSE( WIN32 )
  SET( LIBINTL_LIBRARIES "" )  # on linux, it resides on libc
ENDIF( WIN32 )

IF(NOT LIBINTL_FOUND)
  IF (LIBINTL_INCLUDE_DIR)
    IF(LIBINTL_LIBRARIES)
      SET(LIBINTL_FOUND "YES")
    ENDIF(LIBINTL_LIBRARIES)
  ENDIF(LIBINTL_INCLUDE_DIR)
ENDIF(NOT LIBINTL_FOUND)

IF(NOT LIBINTL_FOUND)
  # make FIND_PACKAGE friendly
  IF(NOT LIBINTL_FIND_QUIETLY)
    IF(LIBINTL_FIND_REQUIRED)
      MESSAGE( STATUS "LIBINTL_INCLUDE_DIR ${LIBINTL_INCLUDE_DIR}" )
      MESSAGE( STATUS "LIBINTL_LIBRARIES   ${LIBINTL_LIBRARIES}" )
      MESSAGE(FATAL_ERROR
              "LIBINTL required, please specify its location with LIBINTL_ROOT.")
    ELSE(LIBINTL_FIND_REQUIRED)
      MESSAGE(STATUS "LIBINTL was not found.")
    ENDIF(LIBINTL_FIND_REQUIRED)
  ENDIF(NOT LIBINTL_FIND_QUIETLY)
ENDIF(NOT LIBINTL_FOUND)

#####

