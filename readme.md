# klotter

A 3d rendering library + sdl starter project

### Name

Klotter in swedish could be translated as doodle, graffiti or scribbling.

### Goals

- Make a 3d renderer that produce pretty pictures
- reduce main bloat and provide a easy way to write a renderer
- only goal is to provide pretty pictures

### Non goals

- versatile data driven rendering library that fits all purposes (use specific interfaces)
- have a good api design with great memory management (use smart pointers)
- modern c++ dependencies (use iostream instead of fmt)

# coding convention

* use `from` functions instead of `to` functions
* all uniforms have `_uniform` suffix
* texture uniforms have `tex_` prefix
