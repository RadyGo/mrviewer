# CMAKE generated file: DO NOT EDIT!
# Generated by "NMake Makefiles" Generator, CMake Version 3.6

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE
NULL=nul
!ENDIF
SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\CMake\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\CMake\bin\cmake.exe" -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = D:\code\applications\mrViewer\dependencies\zlib-1.2.8

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = D:\code\applications\mrViewer\dependencies\zlib-1.2.8\build-win32

# Include any dependencies generated for this target.
include CMakeFiles\zlib.dir\depend.make

# Include the progress variables for this target.
include CMakeFiles\zlib.dir\progress.make

# Include the compile flags for this target's objects.
include CMakeFiles\zlib.dir\flags.make

CMakeFiles\zlib.dir\adler32.obj: CMakeFiles\zlib.dir\flags.make
CMakeFiles\zlib.dir\adler32.obj: ..\adler32.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\code\applications\mrViewer\dependencies\zlib-1.2.8\build-win32\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/zlib.dir/adler32.obj"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoCMakeFiles\zlib.dir\adler32.obj /FdCMakeFiles\zlib.dir\ /FS -c D:\code\applications\mrViewer\dependencies\zlib-1.2.8\adler32.c
<<

CMakeFiles\zlib.dir\adler32.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/zlib.dir/adler32.i"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  > CMakeFiles\zlib.dir\adler32.i @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E D:\code\applications\mrViewer\dependencies\zlib-1.2.8\adler32.c
<<

CMakeFiles\zlib.dir\adler32.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/zlib.dir/adler32.s"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoNUL /FAs /FaCMakeFiles\zlib.dir\adler32.s /c D:\code\applications\mrViewer\dependencies\zlib-1.2.8\adler32.c
<<

CMakeFiles\zlib.dir\adler32.obj.requires:

.PHONY : CMakeFiles\zlib.dir\adler32.obj.requires

CMakeFiles\zlib.dir\adler32.obj.provides: CMakeFiles\zlib.dir\adler32.obj.requires
	$(MAKE) -f CMakeFiles\zlib.dir\build.make /nologo -$(MAKEFLAGS) CMakeFiles\zlib.dir\adler32.obj.provides.build
.PHONY : CMakeFiles\zlib.dir\adler32.obj.provides

CMakeFiles\zlib.dir\adler32.obj.provides.build: CMakeFiles\zlib.dir\adler32.obj


CMakeFiles\zlib.dir\compress.obj: CMakeFiles\zlib.dir\flags.make
CMakeFiles\zlib.dir\compress.obj: ..\compress.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\code\applications\mrViewer\dependencies\zlib-1.2.8\build-win32\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/zlib.dir/compress.obj"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoCMakeFiles\zlib.dir\compress.obj /FdCMakeFiles\zlib.dir\ /FS -c D:\code\applications\mrViewer\dependencies\zlib-1.2.8\compress.c
<<

CMakeFiles\zlib.dir\compress.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/zlib.dir/compress.i"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  > CMakeFiles\zlib.dir\compress.i @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E D:\code\applications\mrViewer\dependencies\zlib-1.2.8\compress.c
<<

CMakeFiles\zlib.dir\compress.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/zlib.dir/compress.s"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoNUL /FAs /FaCMakeFiles\zlib.dir\compress.s /c D:\code\applications\mrViewer\dependencies\zlib-1.2.8\compress.c
<<

CMakeFiles\zlib.dir\compress.obj.requires:

.PHONY : CMakeFiles\zlib.dir\compress.obj.requires

CMakeFiles\zlib.dir\compress.obj.provides: CMakeFiles\zlib.dir\compress.obj.requires
	$(MAKE) -f CMakeFiles\zlib.dir\build.make /nologo -$(MAKEFLAGS) CMakeFiles\zlib.dir\compress.obj.provides.build
.PHONY : CMakeFiles\zlib.dir\compress.obj.provides

CMakeFiles\zlib.dir\compress.obj.provides.build: CMakeFiles\zlib.dir\compress.obj


CMakeFiles\zlib.dir\crc32.obj: CMakeFiles\zlib.dir\flags.make
CMakeFiles\zlib.dir\crc32.obj: ..\crc32.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\code\applications\mrViewer\dependencies\zlib-1.2.8\build-win32\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/zlib.dir/crc32.obj"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoCMakeFiles\zlib.dir\crc32.obj /FdCMakeFiles\zlib.dir\ /FS -c D:\code\applications\mrViewer\dependencies\zlib-1.2.8\crc32.c
<<

CMakeFiles\zlib.dir\crc32.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/zlib.dir/crc32.i"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  > CMakeFiles\zlib.dir\crc32.i @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E D:\code\applications\mrViewer\dependencies\zlib-1.2.8\crc32.c
<<

CMakeFiles\zlib.dir\crc32.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/zlib.dir/crc32.s"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoNUL /FAs /FaCMakeFiles\zlib.dir\crc32.s /c D:\code\applications\mrViewer\dependencies\zlib-1.2.8\crc32.c
<<

CMakeFiles\zlib.dir\crc32.obj.requires:

.PHONY : CMakeFiles\zlib.dir\crc32.obj.requires

CMakeFiles\zlib.dir\crc32.obj.provides: CMakeFiles\zlib.dir\crc32.obj.requires
	$(MAKE) -f CMakeFiles\zlib.dir\build.make /nologo -$(MAKEFLAGS) CMakeFiles\zlib.dir\crc32.obj.provides.build
.PHONY : CMakeFiles\zlib.dir\crc32.obj.provides

CMakeFiles\zlib.dir\crc32.obj.provides.build: CMakeFiles\zlib.dir\crc32.obj


CMakeFiles\zlib.dir\deflate.obj: CMakeFiles\zlib.dir\flags.make
CMakeFiles\zlib.dir\deflate.obj: ..\deflate.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\code\applications\mrViewer\dependencies\zlib-1.2.8\build-win32\CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/zlib.dir/deflate.obj"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoCMakeFiles\zlib.dir\deflate.obj /FdCMakeFiles\zlib.dir\ /FS -c D:\code\applications\mrViewer\dependencies\zlib-1.2.8\deflate.c
<<

CMakeFiles\zlib.dir\deflate.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/zlib.dir/deflate.i"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  > CMakeFiles\zlib.dir\deflate.i @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E D:\code\applications\mrViewer\dependencies\zlib-1.2.8\deflate.c
<<

CMakeFiles\zlib.dir\deflate.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/zlib.dir/deflate.s"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoNUL /FAs /FaCMakeFiles\zlib.dir\deflate.s /c D:\code\applications\mrViewer\dependencies\zlib-1.2.8\deflate.c
<<

CMakeFiles\zlib.dir\deflate.obj.requires:

.PHONY : CMakeFiles\zlib.dir\deflate.obj.requires

CMakeFiles\zlib.dir\deflate.obj.provides: CMakeFiles\zlib.dir\deflate.obj.requires
	$(MAKE) -f CMakeFiles\zlib.dir\build.make /nologo -$(MAKEFLAGS) CMakeFiles\zlib.dir\deflate.obj.provides.build
.PHONY : CMakeFiles\zlib.dir\deflate.obj.provides

CMakeFiles\zlib.dir\deflate.obj.provides.build: CMakeFiles\zlib.dir\deflate.obj


CMakeFiles\zlib.dir\gzclose.obj: CMakeFiles\zlib.dir\flags.make
CMakeFiles\zlib.dir\gzclose.obj: ..\gzclose.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\code\applications\mrViewer\dependencies\zlib-1.2.8\build-win32\CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object CMakeFiles/zlib.dir/gzclose.obj"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoCMakeFiles\zlib.dir\gzclose.obj /FdCMakeFiles\zlib.dir\ /FS -c D:\code\applications\mrViewer\dependencies\zlib-1.2.8\gzclose.c
<<

CMakeFiles\zlib.dir\gzclose.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/zlib.dir/gzclose.i"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  > CMakeFiles\zlib.dir\gzclose.i @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E D:\code\applications\mrViewer\dependencies\zlib-1.2.8\gzclose.c
<<

CMakeFiles\zlib.dir\gzclose.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/zlib.dir/gzclose.s"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoNUL /FAs /FaCMakeFiles\zlib.dir\gzclose.s /c D:\code\applications\mrViewer\dependencies\zlib-1.2.8\gzclose.c
<<

CMakeFiles\zlib.dir\gzclose.obj.requires:

.PHONY : CMakeFiles\zlib.dir\gzclose.obj.requires

CMakeFiles\zlib.dir\gzclose.obj.provides: CMakeFiles\zlib.dir\gzclose.obj.requires
	$(MAKE) -f CMakeFiles\zlib.dir\build.make /nologo -$(MAKEFLAGS) CMakeFiles\zlib.dir\gzclose.obj.provides.build
.PHONY : CMakeFiles\zlib.dir\gzclose.obj.provides

CMakeFiles\zlib.dir\gzclose.obj.provides.build: CMakeFiles\zlib.dir\gzclose.obj


CMakeFiles\zlib.dir\gzlib.obj: CMakeFiles\zlib.dir\flags.make
CMakeFiles\zlib.dir\gzlib.obj: ..\gzlib.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\code\applications\mrViewer\dependencies\zlib-1.2.8\build-win32\CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object CMakeFiles/zlib.dir/gzlib.obj"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoCMakeFiles\zlib.dir\gzlib.obj /FdCMakeFiles\zlib.dir\ /FS -c D:\code\applications\mrViewer\dependencies\zlib-1.2.8\gzlib.c
<<

CMakeFiles\zlib.dir\gzlib.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/zlib.dir/gzlib.i"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  > CMakeFiles\zlib.dir\gzlib.i @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E D:\code\applications\mrViewer\dependencies\zlib-1.2.8\gzlib.c
<<

CMakeFiles\zlib.dir\gzlib.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/zlib.dir/gzlib.s"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoNUL /FAs /FaCMakeFiles\zlib.dir\gzlib.s /c D:\code\applications\mrViewer\dependencies\zlib-1.2.8\gzlib.c
<<

CMakeFiles\zlib.dir\gzlib.obj.requires:

.PHONY : CMakeFiles\zlib.dir\gzlib.obj.requires

CMakeFiles\zlib.dir\gzlib.obj.provides: CMakeFiles\zlib.dir\gzlib.obj.requires
	$(MAKE) -f CMakeFiles\zlib.dir\build.make /nologo -$(MAKEFLAGS) CMakeFiles\zlib.dir\gzlib.obj.provides.build
.PHONY : CMakeFiles\zlib.dir\gzlib.obj.provides

CMakeFiles\zlib.dir\gzlib.obj.provides.build: CMakeFiles\zlib.dir\gzlib.obj


CMakeFiles\zlib.dir\gzread.obj: CMakeFiles\zlib.dir\flags.make
CMakeFiles\zlib.dir\gzread.obj: ..\gzread.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\code\applications\mrViewer\dependencies\zlib-1.2.8\build-win32\CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building C object CMakeFiles/zlib.dir/gzread.obj"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoCMakeFiles\zlib.dir\gzread.obj /FdCMakeFiles\zlib.dir\ /FS -c D:\code\applications\mrViewer\dependencies\zlib-1.2.8\gzread.c
<<

CMakeFiles\zlib.dir\gzread.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/zlib.dir/gzread.i"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  > CMakeFiles\zlib.dir\gzread.i @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E D:\code\applications\mrViewer\dependencies\zlib-1.2.8\gzread.c
<<

CMakeFiles\zlib.dir\gzread.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/zlib.dir/gzread.s"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoNUL /FAs /FaCMakeFiles\zlib.dir\gzread.s /c D:\code\applications\mrViewer\dependencies\zlib-1.2.8\gzread.c
<<

CMakeFiles\zlib.dir\gzread.obj.requires:

.PHONY : CMakeFiles\zlib.dir\gzread.obj.requires

CMakeFiles\zlib.dir\gzread.obj.provides: CMakeFiles\zlib.dir\gzread.obj.requires
	$(MAKE) -f CMakeFiles\zlib.dir\build.make /nologo -$(MAKEFLAGS) CMakeFiles\zlib.dir\gzread.obj.provides.build
.PHONY : CMakeFiles\zlib.dir\gzread.obj.provides

CMakeFiles\zlib.dir\gzread.obj.provides.build: CMakeFiles\zlib.dir\gzread.obj


CMakeFiles\zlib.dir\gzwrite.obj: CMakeFiles\zlib.dir\flags.make
CMakeFiles\zlib.dir\gzwrite.obj: ..\gzwrite.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\code\applications\mrViewer\dependencies\zlib-1.2.8\build-win32\CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building C object CMakeFiles/zlib.dir/gzwrite.obj"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoCMakeFiles\zlib.dir\gzwrite.obj /FdCMakeFiles\zlib.dir\ /FS -c D:\code\applications\mrViewer\dependencies\zlib-1.2.8\gzwrite.c
<<

CMakeFiles\zlib.dir\gzwrite.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/zlib.dir/gzwrite.i"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  > CMakeFiles\zlib.dir\gzwrite.i @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E D:\code\applications\mrViewer\dependencies\zlib-1.2.8\gzwrite.c
<<

CMakeFiles\zlib.dir\gzwrite.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/zlib.dir/gzwrite.s"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoNUL /FAs /FaCMakeFiles\zlib.dir\gzwrite.s /c D:\code\applications\mrViewer\dependencies\zlib-1.2.8\gzwrite.c
<<

CMakeFiles\zlib.dir\gzwrite.obj.requires:

.PHONY : CMakeFiles\zlib.dir\gzwrite.obj.requires

CMakeFiles\zlib.dir\gzwrite.obj.provides: CMakeFiles\zlib.dir\gzwrite.obj.requires
	$(MAKE) -f CMakeFiles\zlib.dir\build.make /nologo -$(MAKEFLAGS) CMakeFiles\zlib.dir\gzwrite.obj.provides.build
.PHONY : CMakeFiles\zlib.dir\gzwrite.obj.provides

CMakeFiles\zlib.dir\gzwrite.obj.provides.build: CMakeFiles\zlib.dir\gzwrite.obj


CMakeFiles\zlib.dir\inflate.obj: CMakeFiles\zlib.dir\flags.make
CMakeFiles\zlib.dir\inflate.obj: ..\inflate.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\code\applications\mrViewer\dependencies\zlib-1.2.8\build-win32\CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building C object CMakeFiles/zlib.dir/inflate.obj"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoCMakeFiles\zlib.dir\inflate.obj /FdCMakeFiles\zlib.dir\ /FS -c D:\code\applications\mrViewer\dependencies\zlib-1.2.8\inflate.c
<<

CMakeFiles\zlib.dir\inflate.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/zlib.dir/inflate.i"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  > CMakeFiles\zlib.dir\inflate.i @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E D:\code\applications\mrViewer\dependencies\zlib-1.2.8\inflate.c
<<

CMakeFiles\zlib.dir\inflate.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/zlib.dir/inflate.s"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoNUL /FAs /FaCMakeFiles\zlib.dir\inflate.s /c D:\code\applications\mrViewer\dependencies\zlib-1.2.8\inflate.c
<<

CMakeFiles\zlib.dir\inflate.obj.requires:

.PHONY : CMakeFiles\zlib.dir\inflate.obj.requires

CMakeFiles\zlib.dir\inflate.obj.provides: CMakeFiles\zlib.dir\inflate.obj.requires
	$(MAKE) -f CMakeFiles\zlib.dir\build.make /nologo -$(MAKEFLAGS) CMakeFiles\zlib.dir\inflate.obj.provides.build
.PHONY : CMakeFiles\zlib.dir\inflate.obj.provides

CMakeFiles\zlib.dir\inflate.obj.provides.build: CMakeFiles\zlib.dir\inflate.obj


CMakeFiles\zlib.dir\infback.obj: CMakeFiles\zlib.dir\flags.make
CMakeFiles\zlib.dir\infback.obj: ..\infback.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\code\applications\mrViewer\dependencies\zlib-1.2.8\build-win32\CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building C object CMakeFiles/zlib.dir/infback.obj"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoCMakeFiles\zlib.dir\infback.obj /FdCMakeFiles\zlib.dir\ /FS -c D:\code\applications\mrViewer\dependencies\zlib-1.2.8\infback.c
<<

CMakeFiles\zlib.dir\infback.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/zlib.dir/infback.i"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  > CMakeFiles\zlib.dir\infback.i @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E D:\code\applications\mrViewer\dependencies\zlib-1.2.8\infback.c
<<

CMakeFiles\zlib.dir\infback.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/zlib.dir/infback.s"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoNUL /FAs /FaCMakeFiles\zlib.dir\infback.s /c D:\code\applications\mrViewer\dependencies\zlib-1.2.8\infback.c
<<

CMakeFiles\zlib.dir\infback.obj.requires:

.PHONY : CMakeFiles\zlib.dir\infback.obj.requires

CMakeFiles\zlib.dir\infback.obj.provides: CMakeFiles\zlib.dir\infback.obj.requires
	$(MAKE) -f CMakeFiles\zlib.dir\build.make /nologo -$(MAKEFLAGS) CMakeFiles\zlib.dir\infback.obj.provides.build
.PHONY : CMakeFiles\zlib.dir\infback.obj.provides

CMakeFiles\zlib.dir\infback.obj.provides.build: CMakeFiles\zlib.dir\infback.obj


CMakeFiles\zlib.dir\inftrees.obj: CMakeFiles\zlib.dir\flags.make
CMakeFiles\zlib.dir\inftrees.obj: ..\inftrees.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\code\applications\mrViewer\dependencies\zlib-1.2.8\build-win32\CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building C object CMakeFiles/zlib.dir/inftrees.obj"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoCMakeFiles\zlib.dir\inftrees.obj /FdCMakeFiles\zlib.dir\ /FS -c D:\code\applications\mrViewer\dependencies\zlib-1.2.8\inftrees.c
<<

CMakeFiles\zlib.dir\inftrees.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/zlib.dir/inftrees.i"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  > CMakeFiles\zlib.dir\inftrees.i @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E D:\code\applications\mrViewer\dependencies\zlib-1.2.8\inftrees.c
<<

CMakeFiles\zlib.dir\inftrees.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/zlib.dir/inftrees.s"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoNUL /FAs /FaCMakeFiles\zlib.dir\inftrees.s /c D:\code\applications\mrViewer\dependencies\zlib-1.2.8\inftrees.c
<<

CMakeFiles\zlib.dir\inftrees.obj.requires:

.PHONY : CMakeFiles\zlib.dir\inftrees.obj.requires

CMakeFiles\zlib.dir\inftrees.obj.provides: CMakeFiles\zlib.dir\inftrees.obj.requires
	$(MAKE) -f CMakeFiles\zlib.dir\build.make /nologo -$(MAKEFLAGS) CMakeFiles\zlib.dir\inftrees.obj.provides.build
.PHONY : CMakeFiles\zlib.dir\inftrees.obj.provides

CMakeFiles\zlib.dir\inftrees.obj.provides.build: CMakeFiles\zlib.dir\inftrees.obj


CMakeFiles\zlib.dir\inffast.obj: CMakeFiles\zlib.dir\flags.make
CMakeFiles\zlib.dir\inffast.obj: ..\inffast.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\code\applications\mrViewer\dependencies\zlib-1.2.8\build-win32\CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building C object CMakeFiles/zlib.dir/inffast.obj"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoCMakeFiles\zlib.dir\inffast.obj /FdCMakeFiles\zlib.dir\ /FS -c D:\code\applications\mrViewer\dependencies\zlib-1.2.8\inffast.c
<<

CMakeFiles\zlib.dir\inffast.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/zlib.dir/inffast.i"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  > CMakeFiles\zlib.dir\inffast.i @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E D:\code\applications\mrViewer\dependencies\zlib-1.2.8\inffast.c
<<

CMakeFiles\zlib.dir\inffast.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/zlib.dir/inffast.s"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoNUL /FAs /FaCMakeFiles\zlib.dir\inffast.s /c D:\code\applications\mrViewer\dependencies\zlib-1.2.8\inffast.c
<<

CMakeFiles\zlib.dir\inffast.obj.requires:

.PHONY : CMakeFiles\zlib.dir\inffast.obj.requires

CMakeFiles\zlib.dir\inffast.obj.provides: CMakeFiles\zlib.dir\inffast.obj.requires
	$(MAKE) -f CMakeFiles\zlib.dir\build.make /nologo -$(MAKEFLAGS) CMakeFiles\zlib.dir\inffast.obj.provides.build
.PHONY : CMakeFiles\zlib.dir\inffast.obj.provides

CMakeFiles\zlib.dir\inffast.obj.provides.build: CMakeFiles\zlib.dir\inffast.obj


CMakeFiles\zlib.dir\trees.obj: CMakeFiles\zlib.dir\flags.make
CMakeFiles\zlib.dir\trees.obj: ..\trees.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\code\applications\mrViewer\dependencies\zlib-1.2.8\build-win32\CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Building C object CMakeFiles/zlib.dir/trees.obj"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoCMakeFiles\zlib.dir\trees.obj /FdCMakeFiles\zlib.dir\ /FS -c D:\code\applications\mrViewer\dependencies\zlib-1.2.8\trees.c
<<

CMakeFiles\zlib.dir\trees.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/zlib.dir/trees.i"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  > CMakeFiles\zlib.dir\trees.i @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E D:\code\applications\mrViewer\dependencies\zlib-1.2.8\trees.c
<<

CMakeFiles\zlib.dir\trees.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/zlib.dir/trees.s"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoNUL /FAs /FaCMakeFiles\zlib.dir\trees.s /c D:\code\applications\mrViewer\dependencies\zlib-1.2.8\trees.c
<<

CMakeFiles\zlib.dir\trees.obj.requires:

.PHONY : CMakeFiles\zlib.dir\trees.obj.requires

CMakeFiles\zlib.dir\trees.obj.provides: CMakeFiles\zlib.dir\trees.obj.requires
	$(MAKE) -f CMakeFiles\zlib.dir\build.make /nologo -$(MAKEFLAGS) CMakeFiles\zlib.dir\trees.obj.provides.build
.PHONY : CMakeFiles\zlib.dir\trees.obj.provides

CMakeFiles\zlib.dir\trees.obj.provides.build: CMakeFiles\zlib.dir\trees.obj


CMakeFiles\zlib.dir\uncompr.obj: CMakeFiles\zlib.dir\flags.make
CMakeFiles\zlib.dir\uncompr.obj: ..\uncompr.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\code\applications\mrViewer\dependencies\zlib-1.2.8\build-win32\CMakeFiles --progress-num=$(CMAKE_PROGRESS_14) "Building C object CMakeFiles/zlib.dir/uncompr.obj"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoCMakeFiles\zlib.dir\uncompr.obj /FdCMakeFiles\zlib.dir\ /FS -c D:\code\applications\mrViewer\dependencies\zlib-1.2.8\uncompr.c
<<

CMakeFiles\zlib.dir\uncompr.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/zlib.dir/uncompr.i"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  > CMakeFiles\zlib.dir\uncompr.i @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E D:\code\applications\mrViewer\dependencies\zlib-1.2.8\uncompr.c
<<

CMakeFiles\zlib.dir\uncompr.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/zlib.dir/uncompr.s"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoNUL /FAs /FaCMakeFiles\zlib.dir\uncompr.s /c D:\code\applications\mrViewer\dependencies\zlib-1.2.8\uncompr.c
<<

CMakeFiles\zlib.dir\uncompr.obj.requires:

.PHONY : CMakeFiles\zlib.dir\uncompr.obj.requires

CMakeFiles\zlib.dir\uncompr.obj.provides: CMakeFiles\zlib.dir\uncompr.obj.requires
	$(MAKE) -f CMakeFiles\zlib.dir\build.make /nologo -$(MAKEFLAGS) CMakeFiles\zlib.dir\uncompr.obj.provides.build
.PHONY : CMakeFiles\zlib.dir\uncompr.obj.provides

CMakeFiles\zlib.dir\uncompr.obj.provides.build: CMakeFiles\zlib.dir\uncompr.obj


CMakeFiles\zlib.dir\zutil.obj: CMakeFiles\zlib.dir\flags.make
CMakeFiles\zlib.dir\zutil.obj: ..\zutil.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\code\applications\mrViewer\dependencies\zlib-1.2.8\build-win32\CMakeFiles --progress-num=$(CMAKE_PROGRESS_15) "Building C object CMakeFiles/zlib.dir/zutil.obj"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoCMakeFiles\zlib.dir\zutil.obj /FdCMakeFiles\zlib.dir\ /FS -c D:\code\applications\mrViewer\dependencies\zlib-1.2.8\zutil.c
<<

CMakeFiles\zlib.dir\zutil.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/zlib.dir/zutil.i"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  > CMakeFiles\zlib.dir\zutil.i @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E D:\code\applications\mrViewer\dependencies\zlib-1.2.8\zutil.c
<<

CMakeFiles\zlib.dir\zutil.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/zlib.dir/zutil.s"
	C:\PROGRA~2\MICROS~1.0\VC\bin\cl.exe  @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoNUL /FAs /FaCMakeFiles\zlib.dir\zutil.s /c D:\code\applications\mrViewer\dependencies\zlib-1.2.8\zutil.c
<<

CMakeFiles\zlib.dir\zutil.obj.requires:

.PHONY : CMakeFiles\zlib.dir\zutil.obj.requires

CMakeFiles\zlib.dir\zutil.obj.provides: CMakeFiles\zlib.dir\zutil.obj.requires
	$(MAKE) -f CMakeFiles\zlib.dir\build.make /nologo -$(MAKEFLAGS) CMakeFiles\zlib.dir\zutil.obj.provides.build
.PHONY : CMakeFiles\zlib.dir\zutil.obj.provides

CMakeFiles\zlib.dir\zutil.obj.provides.build: CMakeFiles\zlib.dir\zutil.obj


CMakeFiles\zlib.dir\win32\zlib1.res: CMakeFiles\zlib.dir\flags.make
CMakeFiles\zlib.dir\win32\zlib1.res: ..\win32\zlib1.rc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\code\applications\mrViewer\dependencies\zlib-1.2.8\build-win32\CMakeFiles --progress-num=$(CMAKE_PROGRESS_16) "Building RC object CMakeFiles/zlib.dir/win32/zlib1.res"
	C:\PROGRA~2\WI3CF2~1\8.1\bin\x86\rc.exe  $(RC_DEFINES) $(RC_INCLUDES) $(RC_FLAGS) /foCMakeFiles\zlib.dir\win32\zlib1.res D:\code\applications\mrViewer\dependencies\zlib-1.2.8\win32\zlib1.rc

CMakeFiles\zlib.dir\win32\zlib1.res.requires:

.PHONY : CMakeFiles\zlib.dir\win32\zlib1.res.requires

CMakeFiles\zlib.dir\win32\zlib1.res.provides: CMakeFiles\zlib.dir\win32\zlib1.res.requires
	$(MAKE) -f CMakeFiles\zlib.dir\build.make /nologo -$(MAKEFLAGS) CMakeFiles\zlib.dir\win32\zlib1.res.provides.build
.PHONY : CMakeFiles\zlib.dir\win32\zlib1.res.provides

CMakeFiles\zlib.dir\win32\zlib1.res.provides.build: CMakeFiles\zlib.dir\win32\zlib1.res


# Object files for target zlib
zlib_OBJECTS = \
"CMakeFiles\zlib.dir\adler32.obj" \
"CMakeFiles\zlib.dir\compress.obj" \
"CMakeFiles\zlib.dir\crc32.obj" \
"CMakeFiles\zlib.dir\deflate.obj" \
"CMakeFiles\zlib.dir\gzclose.obj" \
"CMakeFiles\zlib.dir\gzlib.obj" \
"CMakeFiles\zlib.dir\gzread.obj" \
"CMakeFiles\zlib.dir\gzwrite.obj" \
"CMakeFiles\zlib.dir\inflate.obj" \
"CMakeFiles\zlib.dir\infback.obj" \
"CMakeFiles\zlib.dir\inftrees.obj" \
"CMakeFiles\zlib.dir\inffast.obj" \
"CMakeFiles\zlib.dir\trees.obj" \
"CMakeFiles\zlib.dir\uncompr.obj" \
"CMakeFiles\zlib.dir\zutil.obj" \
"CMakeFiles\zlib.dir\win32\zlib1.res"

# External object files for target zlib
zlib_EXTERNAL_OBJECTS =

zlib.dll: CMakeFiles\zlib.dir\adler32.obj
zlib.dll: CMakeFiles\zlib.dir\compress.obj
zlib.dll: CMakeFiles\zlib.dir\crc32.obj
zlib.dll: CMakeFiles\zlib.dir\deflate.obj
zlib.dll: CMakeFiles\zlib.dir\gzclose.obj
zlib.dll: CMakeFiles\zlib.dir\gzlib.obj
zlib.dll: CMakeFiles\zlib.dir\gzread.obj
zlib.dll: CMakeFiles\zlib.dir\gzwrite.obj
zlib.dll: CMakeFiles\zlib.dir\inflate.obj
zlib.dll: CMakeFiles\zlib.dir\infback.obj
zlib.dll: CMakeFiles\zlib.dir\inftrees.obj
zlib.dll: CMakeFiles\zlib.dir\inffast.obj
zlib.dll: CMakeFiles\zlib.dir\trees.obj
zlib.dll: CMakeFiles\zlib.dir\uncompr.obj
zlib.dll: CMakeFiles\zlib.dir\zutil.obj
zlib.dll: CMakeFiles\zlib.dir\win32\zlib1.res
zlib.dll: CMakeFiles\zlib.dir\build.make
zlib.dll: CMakeFiles\zlib.dir\objects1.rsp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=D:\code\applications\mrViewer\dependencies\zlib-1.2.8\build-win32\CMakeFiles --progress-num=$(CMAKE_PROGRESS_17) "Linking C shared library zlib.dll"
	"C:\Program Files\CMake\bin\cmake.exe" -E vs_link_dll --intdir=CMakeFiles\zlib.dir --manifests  -- C:\PROGRA~2\MICROS~1.0\VC\bin\link.exe /nologo @CMakeFiles\zlib.dir\objects1.rsp @<<
 /out:zlib.dll /implib:zlib.lib /pdb:D:\code\applications\mrViewer\dependencies\zlib-1.2.8\build-win32\zlib.pdb /dll /version:1.2  /machine:X86 /INCREMENTAL:NO kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib  
<<

# Rule to build all files generated by this target.
CMakeFiles\zlib.dir\build: zlib.dll

.PHONY : CMakeFiles\zlib.dir\build

CMakeFiles\zlib.dir\requires: CMakeFiles\zlib.dir\adler32.obj.requires
CMakeFiles\zlib.dir\requires: CMakeFiles\zlib.dir\compress.obj.requires
CMakeFiles\zlib.dir\requires: CMakeFiles\zlib.dir\crc32.obj.requires
CMakeFiles\zlib.dir\requires: CMakeFiles\zlib.dir\deflate.obj.requires
CMakeFiles\zlib.dir\requires: CMakeFiles\zlib.dir\gzclose.obj.requires
CMakeFiles\zlib.dir\requires: CMakeFiles\zlib.dir\gzlib.obj.requires
CMakeFiles\zlib.dir\requires: CMakeFiles\zlib.dir\gzread.obj.requires
CMakeFiles\zlib.dir\requires: CMakeFiles\zlib.dir\gzwrite.obj.requires
CMakeFiles\zlib.dir\requires: CMakeFiles\zlib.dir\inflate.obj.requires
CMakeFiles\zlib.dir\requires: CMakeFiles\zlib.dir\infback.obj.requires
CMakeFiles\zlib.dir\requires: CMakeFiles\zlib.dir\inftrees.obj.requires
CMakeFiles\zlib.dir\requires: CMakeFiles\zlib.dir\inffast.obj.requires
CMakeFiles\zlib.dir\requires: CMakeFiles\zlib.dir\trees.obj.requires
CMakeFiles\zlib.dir\requires: CMakeFiles\zlib.dir\uncompr.obj.requires
CMakeFiles\zlib.dir\requires: CMakeFiles\zlib.dir\zutil.obj.requires
CMakeFiles\zlib.dir\requires: CMakeFiles\zlib.dir\win32\zlib1.res.requires

.PHONY : CMakeFiles\zlib.dir\requires

CMakeFiles\zlib.dir\clean:
	$(CMAKE_COMMAND) -P CMakeFiles\zlib.dir\cmake_clean.cmake
.PHONY : CMakeFiles\zlib.dir\clean

CMakeFiles\zlib.dir\depend:
	$(CMAKE_COMMAND) -E cmake_depends "NMake Makefiles" D:\code\applications\mrViewer\dependencies\zlib-1.2.8 D:\code\applications\mrViewer\dependencies\zlib-1.2.8 D:\code\applications\mrViewer\dependencies\zlib-1.2.8\build-win32 D:\code\applications\mrViewer\dependencies\zlib-1.2.8\build-win32 D:\code\applications\mrViewer\dependencies\zlib-1.2.8\build-win32\CMakeFiles\zlib.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles\zlib.dir\depend

