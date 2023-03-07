# klotter
A 3d rendering library + sdl starter project

### Name
Klotter in swedish could be translated as doodlge, graffiti or scribbling.


### Goals
* Make a 3d renderer that produce pretty pictures
* reduce main bloat and provide a easy way to write a renderer
* only goal is to provide pretty pictures


### Non goals
* versatile data driven rendering library that fits all purposes (use specific interfaces)
* have a good api design with great memory management (use smart pointers)
* modern c++ dependencies (use iostream instead of fmt)


### todo stage 1
* move scene concept from klotter to example
* allow example to take a argument to start with a certain example instead iof "latest"
* split mesh generation from adding in examples
* integrate dear imgui with "debug mode"
* change transform of objects with a scene outline and properties
* add debug renderer
* lights
* better lightning
* complete rest of learn opengl


## todo stage 2
* replace shared ptr with proper owners
* remove "globals"
* replace opengl with different "drivers"

