# ToyEngine

My toy game/graphics engine project, **I do not intend to use it for commercial projects and/or make it production-ready**, 
my main goal with this project is to have to a good testbed for my engine and graphics programming experiments and learn a lot from building it.

## Project Goals

* Keep my code simple and readable, avoid unnecessary abstractions
* Use good and small libraries to boost development, avoid huge and intrusive libraries.
* Implement from scratch things that I want to learn. 
* Try to write a devblog on [my website](https://caioteixeira.com.br/blog/).

## Current Features

* Simple Direct3D 12 Multithreaded Renderer
* Simple Phong Lighting (Point and ambient lights only)
* OBJ meshes and MTL materials loading, using [TinyObjLoader](https://github.com/syoyo/tinyobjloader)
* Entity Component System architecture, based on [EntityX](https://github.com/alecthomas/entityx)
* Integrated profiler, using [EasyProfiler](https://github.com/yse/easy_profiler)
* [Dear ImGui](https://github.com/ocornut/imgui) integration
* CMake build system

## Planned Features

* Event-driven input system
* Realtime shadows
* Frustrum Culling
* Physics API
* Scripting API

## How to build it

* Use the provided CMakeLists file to build it with **Visual Studio 2019**

## Screenshot

![Sponza](https://raw.githubusercontent.com/caioteixeira/ToyEngine/master/Sponza.jpg)
