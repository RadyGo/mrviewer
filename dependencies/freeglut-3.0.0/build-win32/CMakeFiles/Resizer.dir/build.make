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
CMAKE_SOURCE_DIR = D:\code\applications\mrViewer\dependencies\freeglut-3.0.0

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = D:\code\applications\mrViewer\dependencies\freeglut-3.0.0\build-win32

# Include any dependencies generated for this target.
include CMakeFiles\Resizer.dir\depend.make

# Include the progress variables for this target.
include CMakeFiles\Resizer.dir\progress.make

# Include the compile flags for this target's objects.
include CMakeFiles\Resizer.dir\flags.make

CMakeFiles\Resizer.dir\progs\demos\Resizer\Resizer.cpp.obj: CMakeFiles\Resizer.dir\flags.make
CMakeFiles\Resizer.dir\progs\demos\Resizer\Resizer.cpp.obj: ..\progs\demos\Resizer\Resizer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\code\applications\mrViewer\dependencies\freeglut-3.0.0\build-win32\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Resizer.dir/progs/demos/Resizer/Resizer.cpp.obj"
	C:\PROGRA~2\MICROS~2.0\VC\bin\cl.exe  @<<
 /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) /FoCMakeFiles\Resizer.dir\progs\demos\Resizer\Resizer.cpp.obj /FdCMakeFiles\Resizer.dir\ /FS -c D:\code\applications\mrViewer\dependencies\freeglut-3.0.0\progs\demos\Resizer\Resizer.cpp
<<

CMakeFiles\Resizer.dir\progs\demos\Resizer\Resizer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Resizer.dir/progs/demos/Resizer/Resizer.cpp.i"
	C:\PROGRA~2\MICROS~2.0\VC\bin\cl.exe  > CMakeFiles\Resizer.dir\progs\demos\Resizer\Resizer.cpp.i @<<
 /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\code\applications\mrViewer\dependencies\freeglut-3.0.0\progs\demos\Resizer\Resizer.cpp
<<

CMakeFiles\Resizer.dir\progs\demos\Resizer\Resizer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Resizer.dir/progs/demos/Resizer/Resizer.cpp.s"
	C:\PROGRA~2\MICROS~2.0\VC\bin\cl.exe  @<<
 /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) /FoNUL /FAs /FaCMakeFiles\Resizer.dir\progs\demos\Resizer\Resizer.cpp.s /c D:\code\applications\mrViewer\dependencies\freeglut-3.0.0\progs\demos\Resizer\Resizer.cpp
<<

CMakeFiles\Resizer.dir\progs\demos\Resizer\Resizer.cpp.obj.requires:

.PHONY : CMakeFiles\Resizer.dir\progs\demos\Resizer\Resizer.cpp.obj.requires

CMakeFiles\Resizer.dir\progs\demos\Resizer\Resizer.cpp.obj.provides: CMakeFiles\Resizer.dir\progs\demos\Resizer\Resizer.cpp.obj.requires
	$(MAKE) -f CMakeFiles\Resizer.dir\build.make /nologo -$(MAKEFLAGS) CMakeFiles\Resizer.dir\progs\demos\Resizer\Resizer.cpp.obj.provides.build
.PHONY : CMakeFiles\Resizer.dir\progs\demos\Resizer\Resizer.cpp.obj.provides

CMakeFiles\Resizer.dir\progs\demos\Resizer\Resizer.cpp.obj.provides.build: CMakeFiles\Resizer.dir\progs\demos\Resizer\Resizer.cpp.obj


# Object files for target Resizer
Resizer_OBJECTS = \
"CMakeFiles\Resizer.dir\progs\demos\Resizer\Resizer.cpp.obj"

# External object files for target Resizer
Resizer_EXTERNAL_OBJECTS =

bin\Resizer.exe: CMakeFiles\Resizer.dir\progs\demos\Resizer\Resizer.cpp.obj
bin\Resizer.exe: CMakeFiles\Resizer.dir\build.make
bin\Resizer.exe: lib\freeglut.lib
bin\Resizer.exe: CMakeFiles\Resizer.dir\objects1.rsp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=D:\code\applications\mrViewer\dependencies\freeglut-3.0.0\build-win32\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable bin\Resizer.exe"
	"C:\Program Files\CMake\bin\cmake.exe" -E vs_link_exe --intdir=CMakeFiles\Resizer.dir --manifests  -- C:\PROGRA~2\MICROS~2.0\VC\bin\link.exe /nologo @CMakeFiles\Resizer.dir\objects1.rsp @<<
 /out:bin\Resizer.exe /implib:lib\Resizer.lib /pdb:D:\code\applications\mrViewer\dependencies\freeglut-3.0.0\build-win32\bin\Resizer.pdb /version:0.0   /machine:X86 /INCREMENTAL:NO /subsystem:console  glu32.lib opengl32.lib winmm.lib lib\freeglut.lib opengl32.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib 
<<

# Rule to build all files generated by this target.
CMakeFiles\Resizer.dir\build: bin\Resizer.exe

.PHONY : CMakeFiles\Resizer.dir\build

CMakeFiles\Resizer.dir\requires: CMakeFiles\Resizer.dir\progs\demos\Resizer\Resizer.cpp.obj.requires

.PHONY : CMakeFiles\Resizer.dir\requires

CMakeFiles\Resizer.dir\clean:
	$(CMAKE_COMMAND) -P CMakeFiles\Resizer.dir\cmake_clean.cmake
.PHONY : CMakeFiles\Resizer.dir\clean

CMakeFiles\Resizer.dir\depend:
	$(CMAKE_COMMAND) -E cmake_depends "NMake Makefiles" D:\code\applications\mrViewer\dependencies\freeglut-3.0.0 D:\code\applications\mrViewer\dependencies\freeglut-3.0.0 D:\code\applications\mrViewer\dependencies\freeglut-3.0.0\build-win32 D:\code\applications\mrViewer\dependencies\freeglut-3.0.0\build-win32 D:\code\applications\mrViewer\dependencies\freeglut-3.0.0\build-win32\CMakeFiles\Resizer.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles\Resizer.dir\depend

