# klotter
A 3d rendering library + sdl starter project

### Name
Klotter in swedish could be translated as doodle, graffiti or scribbling.


### Goals
* Make a 3d renderer that produce pretty pictures
* reduce main bloat and provide a easy way to write a renderer
* only goal is to provide pretty pictures


### Non goals
* versatile data driven rendering library that fits all purposes (use specific interfaces)
* have a good api design with great memory management (use smart pointers)
* modern c++ dependencies (use iostream instead of fmt)


### todo stage 1
* split mesh generation from adding in examples
* change transform of objects with a scene outline and properties - still relevant with example setup?
* add debug render modes (onlyt render diffuse, only render normals), like sketchfab model inspector views
* add immediate mode styled debug renderer that can render lines, basic shapes(shphere, rect) and text
* lights
* better lightning
* complete rest of learn opengl
* allow example to take a argument to start with a certain example instead iof "latest"


## todo stage 2
* replace shared ptr with proper owners
* remove "globals"
* add particle effects

## todo stage 3
* scriptable render pipeline [mobius with ben day dots]
  or some render graph setup:
  - https://logins.github.io/graphics/2021/05/31/RenderGraphs.html
  - https://levelup.gitconnected.com/organizing-gpu-work-with-directed-acyclic-graphs-f3fd5f2c2af3
* support different light types (cookies, attenuation setups, flickering, frustum spotlights with projections)
* post processing:
  - anti aliasing
  - depth of field
  - camera motion blur
  - panini projection
  - bloom
  - lens distortion
  - chromatic aberration
  - color grading and tonemapping
  - vignette
  - film grain
  - 8 bit dithering
  - custom effects

## todo stage 4
* replace opengl with different "drivers"