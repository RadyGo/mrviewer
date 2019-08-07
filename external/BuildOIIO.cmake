ExternalProject_Add(
  OIIO
  URL "https://github.com/OpenImageIO/oiio/archive/release.zip"
  DEPENDS OCIO OpenEXR ${LibRaw} ${LibWebP}
  CMAKE_ARGS
  -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
  -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
  -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
  -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
  -DBUILD_SHARED_LIBS=ON
  -DOIIO_BUILD_TESTS=OFF)
