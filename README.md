# quartz
Quartz is a plugin based generic cross-platform application. It intends to be a platform for writing wide varity of plugins which need not be related. One of the design goals is to provide as many as plugin hooks without any assumptions about the plugins that will use the platform.

Quartz is in its initial stages and very much a work in progress. There is no guarantee that the code will build.

#Build System
Quartz uses a CMake based build system. I have written a thin layer of CMake macros to make it easy to build different libraries independently. Before running CMake a environment variable QUARTZ_ROOT should be set to the root of the Quartz directory.
The build system does not allow in-source builds, it is recommended that the build be done in $QUARTZ_CORE/builds The generated binaries and headers in case of libraries are installed to $QUARTZ_CORE/output directory. Any module can be built independently provided that the dependency modules are already built and their generated output is installed in $QUARTZ_CORE/output.

#Building 
Right now Quartz is not in a state to build. But the build procedure is as follows (in Bash):
```
export QUARTZ_ROOT=<path_to_quartz_root>
cd $QUARTZ_ROOT/builds
mkdir -p quartz_full_build/debug
cd quartz_full_build/debug
cmake -G "Unix Makefile" -DCMKAE_BUILD_TYPE=Debug ../../../source
make -j4
```

#Modules
At this time Quartz has following modules:
- vqcore: A C++14 library which provides basic utility classes. [Current focus]
- quartz_core: Quartz's default plugin point providers
- quartz_common: Common UI classes in Qt5
- plugins: default plugins to be built with Quartz
- app: The application





