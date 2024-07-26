# Rays2D
It's a simple 2D circle ray tracing simulation.  
If you're interested in the math that goes on behind the scenes take a look at the PDF I've put in `Rays/src`.

# Build
If you're on windows you can simply use the provided binary that can be found in the release section.  
Build it yourself:
```
git clone https://github.com/cunterrainer/PlayingWithRays.git
```
```
cd PlayingWithRays
```
Visual studio (works aswell for vs2019)  
```
vendor/premake5 vs2022
```
gcc  
```
vendor/premake5 gmake --cc=gcc
```
clang
```
vendor/premake5 gmake --cc=clang
```
gcc/clang
```
make <-j> config=<configuration>
```
Configurations:
 - debug_x86
 - debug_x64 (default, the same as just using `make`)
 - release_x86
 - release_x64

The `-j` flag uses multithreaded compilation. Visual Studio builds are multithreaded by default.
