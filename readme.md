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

### todo stage 1

- expand "C"/convert functions with asserts
- expand scene sample with selected object and transform/rotate/scale widget to change transform of objects with a scene outline and simple properties
- add debug render modes (only render diffuse/normals/etc), like sketchfab model inspector views
- add immediate mode styled debug renderer that can render lines, basic shapes(shphere, rect) and text
- fix current camera/mesh/frustum light rotation functions to use a "transform" setup with same logic all over
- material sorting
- frustum culling
- different worlds (loose octree / portal)
- different camera setups (fps, rotate around origin, editor?)
- complete rest of learn opengl

  1. Different lights
  1. Multiple lights
  1. Model loading?

  1. (feat. outlining) - Stencil testing (Advanced OpenGL)
  1. (feat. transparency) - Blending (Advanced OpenGL)
  1. (done?) Face culling (Advanced OpenGL)
  1. (feat. full screen effects) Framebuffers (Advanced OpenGL)
  1. (feat. depth in fse) Depth testing (Advanced OpenGL)
  1. (feat. skybox, reflection/refraction, point light cookies) Cubemaps (Advanced OpenGL)
  1. Advanced Data (Advanced OpenGL)
  1. (optimize uniforms) Advanced GLSL (Advanced OpenGL)
  1. Geometry Shader (Advanced OpenGL)
  1. (render many objects, grass billboards) Instancing (Advanced OpenGL)
  1. (render setting) Anti Aliasing (Advanced OpenGL)

  1. (feat. phong => blinnn phong) Advanced Lighting (Advanced Lighting)
  1. Gamma Correction (Advanced Lighting)
  1. Shadows - Shadow Mapping (Advanced Lighting)
  1. Shadows - Point Shadows (Advanced Lighting)
  1. Normal Mapping (Advanced Lighting)
  1. Parallax Mapping (Advanced Lighting)
  1. HDR (Advanced Lighting)
  1. Bloom (Advanced Lighting)
  1. Deferred Shading (Advanced Lighting)
  1. SSAO (Advanced Lighting)

  1. Theory (PBR)
  1. Lighting (PBR)
  1. IBL - Diffuse irradiance (PBR)
  1. IBL - Specular IBL (PBR)

  1. OIT - Introduction (2020)
  1. OIT - Weighted Blended (2020)
  1. Skeletal Animation (2020)
  1. (bugfix: shadows) CSM (2021)
  1. Scene - Scene Graph (2021)
  1. Scene - Frustum Culling (2021)
  1. Tessellation - Height map (2021)
  1. Tessellation - Tessellation (2021)
  1. DSA (2021)
  1. Compute Shaders (2022)
  1. Phys. Based Bloom (2022)
  1. Area Lights (2022)

- different attenuation
- allow example to take a argument to start with a certain example instead of "latest"

## todo stage 2

- replace shared ptr with proper owners
- remove "globals"
- add particle effects

## todo stage 3

- scriptable render pipeline [mobius with ben day dots]
  or some render graph setup:
  - https://logins.github.io/graphics/2021/05/31/RenderGraphs.html
  - https://levelup.gitconnected.com/organizing-gpu-work-with-directed-acyclic-graphs-f3fd5f2c2af3
- support different light types (cookies, attenuation setups, flickering, frustum spotlights with projections)
- post processing:
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

- replace opengl with different "drivers"
