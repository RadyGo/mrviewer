if (WIN32)
  set( generator "NMake Makefiles" )
else()
  set( generator "Unix Makefiles" )
endif()

ExternalProject_Add(
  ${OCIO_NAME}
  URL "https://github.com/imageworks/OpenColorIO/tarball/master"
  CMAKE_GENERATOR ${generator}
  DEPENDS OpenEXR
  CMAKE_ARGS
  -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
  -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
  -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
  -DBUILD_SHARED_LIBS=ON
  -DOCIO_BUILD_APPS=ON
  -DOCIO_BUILD_NUKE=OFF
  -DOCIO_BUILD_DOCS=OFF
  -DOCIO_BUILD_TESTS=OFF
  -DOCIO_BUILD_GPU_TESTS=OFF
  -DOCIO_BUILD_PYTHON=OFF
  -DOCIO_BUILD_JAVA=OFF
  -DOCIO_WARNING_AS_ERROR=OFF)
