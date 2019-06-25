ExternalProject_Add(
  JPEG-TURBO
  GIT_REPOSITORY "https://github.com/libjpeg-turbo/libjpeg-turbo.git"
  CMAKE_ARGS
  -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
  -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
  -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
  -DBUILD_SHARED_LIBS=ON
  -DWITH_12BIT=1)
