# My First Game Using C/SDL

To build the project you need to get the SDL source code. The only platform built for so far is MacOS. The project is preconfigured for this command to work:
`git clone https://github.com/libsdl-org/SDL vendored/SDL`

Then build using CMake:
`cmake -B build`

To compile:
`cmake --build build`

To run:
`./build/pong`
