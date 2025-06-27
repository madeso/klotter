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

## todo

### todo: part of the klotter project
- simplify example app, remove the concept of multiple examples...
- complete rest of learn opengl
- expand "C"/convert functions with asserts
- add debug render modes (only render diffuse/normals/etc), like sketchfab model inspector views
- fix current camera/mesh/frustum light rotation functions to use a "transform" setup
- material sorting: https://realtimecollisiondetection.net/blog/?p=86
- frustum culling
- different worlds (loose octree / portal / etc..)
- implement "virtual textures" for use in in-game cameras, mirrors, portals and computer screens
- postproc: improve blur shader:
  - https://www.ronja-tutorials.com/post/023-postprocessing-blur/
  - https://blog.innogames.com/shader-exploration-the-art-of-blurring/
- replace hardcoded materials with something more data driven and flexible
- scriptable render pipeline [mobius with ben day dots]
  or some render graph setup:
  - https://logins.github.io/graphics/2021/05/31/RenderGraphs.html
  - https://levelup.gitconnected.com/organizing-gpu-work-with-directed-acyclic-graphs-f3fd5f2c2af3


### todo: before merginging to euphoria/cleanup
- replace shared ptr with proper owners
- remove "globals"
- render always klotter world to texture so it can be handled as a  dear imgui widget

### todo: after merging to euphoria
- implement better outlines that handles more meshes and hoepfully specify outline in screenspace: https://ameye.dev/notes/rendering-outlines/
- postproc: add depth (just a optional additional texture) to simulate depth of field of no-glasses
- material: framebuffer distortions to simulate reflection/refraction
- upgrade debug renderer with basic shapes(shphere, rect) and text
- postproc: add chromatic abberation to simulate damage/drugs
- add more/improve post processing:
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
- flickering lights: https://www.alanzucconi.com/2021/06/15/valve-flickering-lights/
- river system, with spline and polygon editor, vertex color painting, alpha for joining paths and "flow map" painting (another vertex attribute?)
  https://twitter.com/JasperRLZ/status/1511951178753445896?t=yhT2wJVw8-iJlyBqADf-Yw&s=09
  > A few people were asking me how the dynamic water surface worked. Quite simple! Two sine waves, one going across the river, the other going down the river. With heights and speeds dialed into tasteful values, and some extra texture scrolling, the result is surprisingly dynamic.
- postproc/material: Add shader permutation tool, change options (int slider, enum dropdown, bool) to select current permutation
- support different light types (cookies, attenuation setups, flickering, frustum spotlights with projections)
- look into different attenuation algorithms
- material: use cubemap + color
- replace opengl with different "drivers"
