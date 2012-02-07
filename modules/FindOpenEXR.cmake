#-*-cmake-*-
#
# Test for OpenEXR
#
# Once loaded this will define
#  OPENEXR_FOUND        - system has OpenEXR
#  OPENEXR_INCLUDE_DIR  - include directory for OpenEXR
#  OPENEXR_LIBRARY_DIR  - library directory for OpenEXR
#  OPENEXR_LIBRARIES    - libraries you need to link to
#

SET(OPENEXR_FOUND "NO")

IF( OPENEXR_LIBRARY_DIR )
  SET( SEARCH_DIRS "${OPENEXR_LIBRARY_DIR}" )
ELSE( OPENEXR_LIBRARY_DIR )
  SET( SEARCH_DIRS 
    $ENV{OPENEXR_ROOT}/lib
    $ENV{OPENEXR_ROOT}/lib/Release
    $ENV{OPENEXR_ROOT}/lib/Debug
    $ENV{OPENEXR_ROOT}/bin/Release
    $ENV{OPENEXR_ROOT}/bin/Debug
    /usr/local/lib${CMAKE_BUILD_ARCH}
    /usr/lib${CMAKE_BUILD_ARCH}
    )
ENDIF( OPENEXR_LIBRARY_DIR )


FIND_PATH( OPENEXR_INCLUDE_DIR ImfHeader.h
  "$ENV{OPENEXR_ROOT}/include/OpenEXR"
  "$ENV{OPENEXR_ROOT}/include"
  /usr/local/include/OpenEXR
  /usr/include/OpenEXR
  DOC   "OpenEXR includes"
  )

FIND_LIBRARY( IlmImf 
  NAMES IlmImf_dll IlmImf_dll_d IlmImf IlmImfd libIlmImf 
  PATHS ${SEARCH_DIRS}
  DOC   "OpenEXR IlmImf library"
)

FIND_LIBRARY( Imath 
  NAMES Imath_dll Imath_dll_d Imath Imathd libImath 
  PATHS ${SEARCH_DIRS}
  DOC   "OpenEXR Imath library"
)

FIND_LIBRARY( Iex
  NAMES Iex_dll Iex_dll_d Iex Iexd libIex 
  PATHS ${SEARCH_DIRS}
  DOC   "OpenEXR Iex library"
)

FIND_LIBRARY( Half
  NAMES Half_dll Half_dll_d Half Halfd libHalf 
  PATHS ${SEARCH_DIRS}
  DOC   "OpenEXR Half library"
)


SET(OPENEXR_LIBRARIES ${IlmImf} ${Imath} ${Iex} ${Half} )

IF(WIN32)
  ADD_DEFINITIONS( "-DOPENEXR_DLL" )
ENDIF(WIN32)


IF(NOT OPENEXR_FOUND)
  IF (OPENEXR_INCLUDE_DIR)
    IF(OPENEXR_LIBRARIES)
      SET(OPENEXR_FOUND "YES")
      IF( NOT OPENEXR_LIBRARY_DIR )
	GET_FILENAME_COMPONENT(OPENEXR_LIBRARY_DIR "${IlmImf}" PATH)
      ENDIF( NOT OPENEXR_LIBRARY_DIR )
    ENDIF(OPENEXR_LIBRARIES)
  ENDIF(OPENEXR_INCLUDE_DIR)
ENDIF(NOT OPENEXR_FOUND)

IF(NOT OPENEXR_FOUND)
  # make FIND_PACKAGE friendly
  IF(NOT OPENEXR_FIND_QUIETLY)
    IF(OPENEXR_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR
              "OpenEXR required, please specify its location with OPENEXR_ROOT.")
    ELSE(OPENEXR_FIND_REQUIRED)
      MESSAGE( STATUS "OpenEXR was not found!!! " ${OPENEXR_INCLUDE_DIR})
      MESSAGE( STATUS ${OPENEXR_LIBRARIES} )
    ENDIF(OPENEXR_FIND_REQUIRED)
  ENDIF(NOT OPENEXR_FIND_QUIETLY)
ENDIF(NOT OPENEXR_FOUND)


#
# We search for IlmThread only in the directory of IlmImf
#
# This is to avoid picking IlmThread for a wrong version of IlmImf.
#
FIND_LIBRARY( IlmThread
  NAMES IlmThread_dll IlmThread_dll_d IlmThread IlmThreadd libIlmThread 
  PATHS ${OPENEXR_LIBRARY_DIR}
  NO_DEFAULT_PATH
  DOC   "OpenEXR IlmThread library (1.5 or later)"
  )

IF( NOT IlmThread )
  SET( IlmThread "" )
ENDIF( NOT IlmThread )

SET(OPENEXR_LIBRARIES ${OPENEXR_LIBRARIES} ${IlmThread} )

#####
