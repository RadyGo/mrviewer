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
CMAKE_SOURCE_DIR = D:\code\applications\mrViewer\dependencies\OpenEXR

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = D:\code\applications\mrViewer\dependencies\OpenEXR\build-win32

# Include any dependencies generated for this target.
include exrheader\CMakeFiles\exrheader.dir\depend.make

# Include the progress variables for this target.
include exrheader\CMakeFiles\exrheader.dir\progress.make

# Include the compile flags for this target's objects.
include exrheader\CMakeFiles\exrheader.dir\flags.make

exrheader\CMakeFiles\exrheader.dir\main.cpp.obj: exrheader\CMakeFiles\exrheader.dir\flags.make
exrheader\CMakeFiles\exrheader.dir\main.cpp.obj: ..\exrheader\main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\code\applications\mrViewer\dependencies\OpenEXR\build-win32\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object exrheader/CMakeFiles/exrheader.dir/main.cpp.obj"
	cd D:\code\applications\mrViewer\dependencies\OpenEXR\build-win32\exrheader
	C:\PROGRA~2\MICROS~2.0\VC\bin\cl.exe  @<<
 /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) /FoCMakeFiles\exrheader.dir\main.cpp.obj /FdCMakeFiles\exrheader.dir\ /FS -c D:\code\applications\mrViewer\dependencies\OpenEXR\exrheader\main.cpp
<<
	cd D:\code\applications\mrViewer\dependencies\OpenEXR\build-win32

exrheader\CMakeFiles\exrheader.dir\main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/exrheader.dir/main.cpp.i"
	cd D:\code\applications\mrViewer\dependencies\OpenEXR\build-win32\exrheader
	C:\PROGRA~2\MICROS~2.0\VC\bin\cl.exe  > CMakeFiles\exrheader.dir\main.cpp.i @<<
 /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\code\applications\mrViewer\dependencies\OpenEXR\exrheader\main.cpp
<<
	cd D:\code\applications\mrViewer\dependencies\OpenEXR\build-win32

exrheader\CMakeFiles\exrheader.dir\main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/exrheader.dir/main.cpp.s"
	cd D:\code\applications\mrViewer\dependencies\OpenEXR\build-win32\exrheader
	C:\PROGRA~2\MICROS~2.0\VC\bin\cl.exe  @<<
 /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) /FoNUL /FAs /FaCMakeFiles\exrheader.dir\main.cpp.s /c D:\code\applications\mrViewer\dependencies\OpenEXR\exrheader\main.cpp
<<
	cd D:\code\applications\mrViewer\dependencies\OpenEXR\build-win32

exrheader\CMakeFiles\exrheader.dir\main.cpp.obj.requires:

.PHONY : exrheader\CMakeFiles\exrheader.dir\main.cpp.obj.requires

exrheader\CMakeFiles\exrheader.dir\main.cpp.obj.provides: exrheader\CMakeFiles\exrheader.dir\main.cpp.obj.requires
	$(MAKE) -f exrheader\CMakeFiles\exrheader.dir\build.make /nologo -$(MAKEFLAGS) exrheader\CMakeFiles\exrheader.dir\main.cpp.obj.provides.build
.PHONY : exrheader\CMakeFiles\exrheader.dir\main.cpp.obj.provides

exrheader\CMakeFiles\exrheader.dir\main.cpp.obj.provides.build: exrheader\CMakeFiles\exrheader.dir\main.cpp.obj


# Object files for target exrheader
exrheader_OBJECTS = \
"CMakeFiles\exrheader.dir\main.cpp.obj"

# External object files for target exrheader
exrheader_EXTERNAL_OBJECTS =

exrheader\exrheader.exe: exrheader\CMakeFiles\exrheader.dir\main.cpp.obj
exrheader\exrheader.exe: exrheader\CMakeFiles\exrheader.dir\build.make
exrheader\exrheader.exe: IlmImf\IlmImf-2_2.lib
exrheader\exrheader.exe: d:\code\lib\Windows_32\lib\zdll.lib
exrheader\exrheader.exe: d:\code\lib\Windows_32\lib\zdll.lib
exrheader\exrheader.exe: exrheader\CMakeFiles\exrheader.dir\objects1.rsp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=D:\code\applications\mrViewer\dependencies\OpenEXR\build-win32\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable exrheader.exe"
	cd D:\code\applications\mrViewer\dependencies\OpenEXR\build-win32\exrheader
	"C:\Program Files\CMake\bin\cmake.exe" -E vs_link_exe --intdir=CMakeFiles\exrheader.dir --manifests  -- C:\PROGRA~2\MICROS~2.0\VC\bin\link.exe /nologo @CMakeFiles\exrheader.dir\objects1.rsp @<<
 /out:exrheader.exe /implib:exrheader.lib /pdb:D:\code\applications\mrViewer\dependencies\OpenEXR\build-win32\exrheader\exrheader.pdb /version:0.0   /machine:X86 /INCREMENTAL:NO /subsystem:console  -LIBPATH:d:\code\lib\Windows_32\lib  ..\IlmImf\IlmImf-2_2.lib Iex-2_2.lib IlmThread-2_2.lib Half.lib d:\code\lib\Windows_32\lib\zdll.lib Imath-2_2.lib IlmThread-2_2.lib d:\code\lib\Windows_32\lib\zdll.lib kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib 
<<
	cd D:\code\applications\mrViewer\dependencies\OpenEXR\build-win32

# Rule to build all files generated by this target.
exrheader\CMakeFiles\exrheader.dir\build: exrheader\exrheader.exe

.PHONY : exrheader\CMakeFiles\exrheader.dir\build

exrheader\CMakeFiles\exrheader.dir\requires: exrheader\CMakeFiles\exrheader.dir\main.cpp.obj.requires

.PHONY : exrheader\CMakeFiles\exrheader.dir\requires

exrheader\CMakeFiles\exrheader.dir\clean:
	cd D:\code\applications\mrViewer\dependencies\OpenEXR\build-win32\exrheader
	$(CMAKE_COMMAND) -P CMakeFiles\exrheader.dir\cmake_clean.cmake
	cd D:\code\applications\mrViewer\dependencies\OpenEXR\build-win32
.PHONY : exrheader\CMakeFiles\exrheader.dir\clean

exrheader\CMakeFiles\exrheader.dir\depend:
	$(CMAKE_COMMAND) -E cmake_depends "NMake Makefiles" D:\code\applications\mrViewer\dependencies\OpenEXR D:\code\applications\mrViewer\dependencies\OpenEXR\exrheader D:\code\applications\mrViewer\dependencies\OpenEXR\build-win32 D:\code\applications\mrViewer\dependencies\OpenEXR\build-win32\exrheader D:\code\applications\mrViewer\dependencies\OpenEXR\build-win32\exrheader\CMakeFiles\exrheader.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : exrheader\CMakeFiles\exrheader.dir\depend

