# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.19

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /snap/clion/152/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /snap/clion/152/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/hzy/Projects/System/SimpleWebServer

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/hzy/Projects/System/SimpleWebServer/cmake-build-release

# Include any dependencies generated for this target.
include CMakeFiles/SimpleSystem.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/SimpleSystem.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/SimpleSystem.dir/flags.make

CMakeFiles/SimpleSystem.dir/src/main.cpp.o: CMakeFiles/SimpleSystem.dir/flags.make
CMakeFiles/SimpleSystem.dir/src/main.cpp.o: ../src/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/hzy/Projects/System/SimpleWebServer/cmake-build-release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/SimpleSystem.dir/src/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/SimpleSystem.dir/src/main.cpp.o -c /home/hzy/Projects/System/SimpleWebServer/src/main.cpp

CMakeFiles/SimpleSystem.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SimpleSystem.dir/src/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hzy/Projects/System/SimpleWebServer/src/main.cpp > CMakeFiles/SimpleSystem.dir/src/main.cpp.i

CMakeFiles/SimpleSystem.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SimpleSystem.dir/src/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hzy/Projects/System/SimpleWebServer/src/main.cpp -o CMakeFiles/SimpleSystem.dir/src/main.cpp.s

CMakeFiles/SimpleSystem.dir/src/transmission/webserver/WebServer.cpp.o: CMakeFiles/SimpleSystem.dir/flags.make
CMakeFiles/SimpleSystem.dir/src/transmission/webserver/WebServer.cpp.o: ../src/transmission/webserver/WebServer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/hzy/Projects/System/SimpleWebServer/cmake-build-release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/SimpleSystem.dir/src/transmission/webserver/WebServer.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/SimpleSystem.dir/src/transmission/webserver/WebServer.cpp.o -c /home/hzy/Projects/System/SimpleWebServer/src/transmission/webserver/WebServer.cpp

CMakeFiles/SimpleSystem.dir/src/transmission/webserver/WebServer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SimpleSystem.dir/src/transmission/webserver/WebServer.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hzy/Projects/System/SimpleWebServer/src/transmission/webserver/WebServer.cpp > CMakeFiles/SimpleSystem.dir/src/transmission/webserver/WebServer.cpp.i

CMakeFiles/SimpleSystem.dir/src/transmission/webserver/WebServer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SimpleSystem.dir/src/transmission/webserver/WebServer.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hzy/Projects/System/SimpleWebServer/src/transmission/webserver/WebServer.cpp -o CMakeFiles/SimpleSystem.dir/src/transmission/webserver/WebServer.cpp.s

CMakeFiles/SimpleSystem.dir/src/transmission/webserver/Http.cpp.o: CMakeFiles/SimpleSystem.dir/flags.make
CMakeFiles/SimpleSystem.dir/src/transmission/webserver/Http.cpp.o: ../src/transmission/webserver/Http.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/hzy/Projects/System/SimpleWebServer/cmake-build-release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/SimpleSystem.dir/src/transmission/webserver/Http.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/SimpleSystem.dir/src/transmission/webserver/Http.cpp.o -c /home/hzy/Projects/System/SimpleWebServer/src/transmission/webserver/Http.cpp

CMakeFiles/SimpleSystem.dir/src/transmission/webserver/Http.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SimpleSystem.dir/src/transmission/webserver/Http.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hzy/Projects/System/SimpleWebServer/src/transmission/webserver/Http.cpp > CMakeFiles/SimpleSystem.dir/src/transmission/webserver/Http.cpp.i

CMakeFiles/SimpleSystem.dir/src/transmission/webserver/Http.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SimpleSystem.dir/src/transmission/webserver/Http.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hzy/Projects/System/SimpleWebServer/src/transmission/webserver/Http.cpp -o CMakeFiles/SimpleSystem.dir/src/transmission/webserver/Http.cpp.s

CMakeFiles/SimpleSystem.dir/src/utils/concurrence/ThreadPool.cpp.o: CMakeFiles/SimpleSystem.dir/flags.make
CMakeFiles/SimpleSystem.dir/src/utils/concurrence/ThreadPool.cpp.o: ../src/utils/concurrence/ThreadPool.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/hzy/Projects/System/SimpleWebServer/cmake-build-release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/SimpleSystem.dir/src/utils/concurrence/ThreadPool.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/SimpleSystem.dir/src/utils/concurrence/ThreadPool.cpp.o -c /home/hzy/Projects/System/SimpleWebServer/src/utils/concurrence/ThreadPool.cpp

CMakeFiles/SimpleSystem.dir/src/utils/concurrence/ThreadPool.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SimpleSystem.dir/src/utils/concurrence/ThreadPool.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hzy/Projects/System/SimpleWebServer/src/utils/concurrence/ThreadPool.cpp > CMakeFiles/SimpleSystem.dir/src/utils/concurrence/ThreadPool.cpp.i

CMakeFiles/SimpleSystem.dir/src/utils/concurrence/ThreadPool.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SimpleSystem.dir/src/utils/concurrence/ThreadPool.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hzy/Projects/System/SimpleWebServer/src/utils/concurrence/ThreadPool.cpp -o CMakeFiles/SimpleSystem.dir/src/utils/concurrence/ThreadPool.cpp.s

CMakeFiles/SimpleSystem.dir/src/utils/buffer/Buffer.cpp.o: CMakeFiles/SimpleSystem.dir/flags.make
CMakeFiles/SimpleSystem.dir/src/utils/buffer/Buffer.cpp.o: ../src/utils/buffer/Buffer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/hzy/Projects/System/SimpleWebServer/cmake-build-release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/SimpleSystem.dir/src/utils/buffer/Buffer.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/SimpleSystem.dir/src/utils/buffer/Buffer.cpp.o -c /home/hzy/Projects/System/SimpleWebServer/src/utils/buffer/Buffer.cpp

CMakeFiles/SimpleSystem.dir/src/utils/buffer/Buffer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SimpleSystem.dir/src/utils/buffer/Buffer.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hzy/Projects/System/SimpleWebServer/src/utils/buffer/Buffer.cpp > CMakeFiles/SimpleSystem.dir/src/utils/buffer/Buffer.cpp.i

CMakeFiles/SimpleSystem.dir/src/utils/buffer/Buffer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SimpleSystem.dir/src/utils/buffer/Buffer.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hzy/Projects/System/SimpleWebServer/src/utils/buffer/Buffer.cpp -o CMakeFiles/SimpleSystem.dir/src/utils/buffer/Buffer.cpp.s

CMakeFiles/SimpleSystem.dir/src/utils/nio/Epoll.cpp.o: CMakeFiles/SimpleSystem.dir/flags.make
CMakeFiles/SimpleSystem.dir/src/utils/nio/Epoll.cpp.o: ../src/utils/nio/Epoll.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/hzy/Projects/System/SimpleWebServer/cmake-build-release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/SimpleSystem.dir/src/utils/nio/Epoll.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/SimpleSystem.dir/src/utils/nio/Epoll.cpp.o -c /home/hzy/Projects/System/SimpleWebServer/src/utils/nio/Epoll.cpp

CMakeFiles/SimpleSystem.dir/src/utils/nio/Epoll.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SimpleSystem.dir/src/utils/nio/Epoll.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hzy/Projects/System/SimpleWebServer/src/utils/nio/Epoll.cpp > CMakeFiles/SimpleSystem.dir/src/utils/nio/Epoll.cpp.i

CMakeFiles/SimpleSystem.dir/src/utils/nio/Epoll.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SimpleSystem.dir/src/utils/nio/Epoll.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hzy/Projects/System/SimpleWebServer/src/utils/nio/Epoll.cpp -o CMakeFiles/SimpleSystem.dir/src/utils/nio/Epoll.cpp.s

CMakeFiles/SimpleSystem.dir/src/utils/Config.cpp.o: CMakeFiles/SimpleSystem.dir/flags.make
CMakeFiles/SimpleSystem.dir/src/utils/Config.cpp.o: ../src/utils/Config.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/hzy/Projects/System/SimpleWebServer/cmake-build-release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/SimpleSystem.dir/src/utils/Config.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/SimpleSystem.dir/src/utils/Config.cpp.o -c /home/hzy/Projects/System/SimpleWebServer/src/utils/Config.cpp

CMakeFiles/SimpleSystem.dir/src/utils/Config.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SimpleSystem.dir/src/utils/Config.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hzy/Projects/System/SimpleWebServer/src/utils/Config.cpp > CMakeFiles/SimpleSystem.dir/src/utils/Config.cpp.i

CMakeFiles/SimpleSystem.dir/src/utils/Config.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SimpleSystem.dir/src/utils/Config.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hzy/Projects/System/SimpleWebServer/src/utils/Config.cpp -o CMakeFiles/SimpleSystem.dir/src/utils/Config.cpp.s

CMakeFiles/SimpleSystem.dir/src/utils/Error.cpp.o: CMakeFiles/SimpleSystem.dir/flags.make
CMakeFiles/SimpleSystem.dir/src/utils/Error.cpp.o: ../src/utils/Error.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/hzy/Projects/System/SimpleWebServer/cmake-build-release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/SimpleSystem.dir/src/utils/Error.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/SimpleSystem.dir/src/utils/Error.cpp.o -c /home/hzy/Projects/System/SimpleWebServer/src/utils/Error.cpp

CMakeFiles/SimpleSystem.dir/src/utils/Error.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SimpleSystem.dir/src/utils/Error.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hzy/Projects/System/SimpleWebServer/src/utils/Error.cpp > CMakeFiles/SimpleSystem.dir/src/utils/Error.cpp.i

CMakeFiles/SimpleSystem.dir/src/utils/Error.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SimpleSystem.dir/src/utils/Error.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hzy/Projects/System/SimpleWebServer/src/utils/Error.cpp -o CMakeFiles/SimpleSystem.dir/src/utils/Error.cpp.s

# Object files for target SimpleSystem
SimpleSystem_OBJECTS = \
"CMakeFiles/SimpleSystem.dir/src/main.cpp.o" \
"CMakeFiles/SimpleSystem.dir/src/transmission/webserver/WebServer.cpp.o" \
"CMakeFiles/SimpleSystem.dir/src/transmission/webserver/Http.cpp.o" \
"CMakeFiles/SimpleSystem.dir/src/utils/concurrence/ThreadPool.cpp.o" \
"CMakeFiles/SimpleSystem.dir/src/utils/buffer/Buffer.cpp.o" \
"CMakeFiles/SimpleSystem.dir/src/utils/nio/Epoll.cpp.o" \
"CMakeFiles/SimpleSystem.dir/src/utils/Config.cpp.o" \
"CMakeFiles/SimpleSystem.dir/src/utils/Error.cpp.o"

# External object files for target SimpleSystem
SimpleSystem_EXTERNAL_OBJECTS =

../bin/SimpleSystem: CMakeFiles/SimpleSystem.dir/src/main.cpp.o
../bin/SimpleSystem: CMakeFiles/SimpleSystem.dir/src/transmission/webserver/WebServer.cpp.o
../bin/SimpleSystem: CMakeFiles/SimpleSystem.dir/src/transmission/webserver/Http.cpp.o
../bin/SimpleSystem: CMakeFiles/SimpleSystem.dir/src/utils/concurrence/ThreadPool.cpp.o
../bin/SimpleSystem: CMakeFiles/SimpleSystem.dir/src/utils/buffer/Buffer.cpp.o
../bin/SimpleSystem: CMakeFiles/SimpleSystem.dir/src/utils/nio/Epoll.cpp.o
../bin/SimpleSystem: CMakeFiles/SimpleSystem.dir/src/utils/Config.cpp.o
../bin/SimpleSystem: CMakeFiles/SimpleSystem.dir/src/utils/Error.cpp.o
../bin/SimpleSystem: CMakeFiles/SimpleSystem.dir/build.make
../bin/SimpleSystem: CMakeFiles/SimpleSystem.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/hzy/Projects/System/SimpleWebServer/cmake-build-release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Linking CXX executable ../bin/SimpleSystem"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/SimpleSystem.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/SimpleSystem.dir/build: ../bin/SimpleSystem

.PHONY : CMakeFiles/SimpleSystem.dir/build

CMakeFiles/SimpleSystem.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/SimpleSystem.dir/cmake_clean.cmake
.PHONY : CMakeFiles/SimpleSystem.dir/clean

CMakeFiles/SimpleSystem.dir/depend:
	cd /home/hzy/Projects/System/SimpleWebServer/cmake-build-release && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/hzy/Projects/System/SimpleWebServer /home/hzy/Projects/System/SimpleWebServer /home/hzy/Projects/System/SimpleWebServer/cmake-build-release /home/hzy/Projects/System/SimpleWebServer/cmake-build-release /home/hzy/Projects/System/SimpleWebServer/cmake-build-release/CMakeFiles/SimpleSystem.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/SimpleSystem.dir/depend

