# SFML_Projects
This repository contains a nubmer of small projects that use the Simple and Fast Multimedia Library [SFML](https://www.sfml-dev.org/).
# How to set-up the project
Download the [SFML SDK](https://www.sfml-dev.org/download.php) and put them into a folder named SFML next to the project's folder. Make sure that the project you want to run is [configured correctly](https://www.sfml-dev.org/tutorials/2.5/start-vc.php). One of the project needs the OpenCl SDK. make sure to set it up as well if you want to run it.
# The Projects
## DrawingApp
This is the main application is was working on with SFML. The initial idea was to create a drawing app where you can draw with normals and different materials. It turned oyt that drawing with normals is not as pleasant as I imagined :D
But I learned how to write simple OpenGl shaders and had to get more familiar with many aspects of C++.
## FlowerGenerator
It generates beautiful flowers.
## HeightMapGenerator
First attempts at generating height maps.
## LSystems
A small program that allows the user to generate [Lindenmayer systems](https://en.wikipedia.org/wiki/L-system) with SFML.
## Mandelbrot
My first attempt at drawing the Mandelbrot. It is running entirely on the CPU.
## OpenClMandelbrot
My second attempt at drawing the Mandelbrot. This time using OpenCl.
## PathFindingExperiments
My first attempts at path finding. This is an implementation of the A* algorithm. One can draw obstacles, set a start and a target point and it will render the path if it can be found.
## PerlinNoise
Perlin noise turned out to be a wonderful thing. Simple Project where the entire implementation is in C++ and rendered with SFML.
