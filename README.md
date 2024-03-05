# Small engine in Vulkan API
## Table of contents
* [General info](#general-info)
* [Technologies](#technologies)
* [Setup](#setup)
* [Controls](#controls)

## General info
This project is simple engine featuring:
* Phong-Blinn lighting model
* Flat, Gouraud, Phong shading methods
* Fog and day/night effect
* Static, following object, third-person and unlocked camera modes
* Reflectors attached to a moving object with changeable light direction.
	
## Technologies
Project is created with:
* C++ 17
* Vulkan 1.3
* CMake
* GLFW, GLM libraries
	
## Setup
To run this project:
* Change paths in .env.cmake file to match your directories 
* Open solution located in build folder
* Build shaders project
* Set VcuEngine project as the startup project
* Build and run in Release mode

## Controls
To switch between features:
- Use <kbd>space</kbd> to change shading method
- Use <kbd>c</kbd> to change camera mode
- Use <kbd>f</kbd> to enable/disable fog effect
- Use <kbd>n</kbd> to switch between day/night mode
- Use <kbd>[</kbd> to move reflector direction forward
- Use <kbd>]</kbd> to move reflector direction backward
  
For unlocked camera mode:
- Use <kbd>w</kbd>|<kbd>s</kbd>|<kbd>a</kbd>|<kbd>d</kbd> to move forward/backward/left/right
- Use <kbd>q</kbd>|<kbd>e</kbd> to move down/up
- Use <kbd>←</kbd>|<kbd>↑</kbd>|<kbd>→</kbd>|<kbd>↓</kbd> to look left/up/right/down
