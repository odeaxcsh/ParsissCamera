# ParsissCamera

## Build 
+ Create a new folder for build `mkdir build && cd build`
+ Configure build files using Cmake by running `cmake ..`
+ Build project using Make by running `make all`

## Run Sample Test
After running above commands ther would be appeared `SimulatedTest.exe` which by running it you can see the position of tools in the test scene.

## Use as an external library
After building project you can run `make install` to create an standalone library in the `Library` folder of project.
You can choose between a shared libary of a static one in the cmake configuration.
After Creating library one can compress files as an nuget pacakge using `ParsissCamera.nuspec` and nuget command.(Works only in windows)
