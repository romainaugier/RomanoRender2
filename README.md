# RomanoRender2

RomanoRender2 is the second iteration of a pathtracer written in C++ and OSL which aims to help me learn rendering, maths, C++, software engineering and more about computer graphics in general.

It relies on [Embree](https://github.com/embree/embree) for geometry intersection and its common library, [Open Shading Language](https://github.com/AcademySoftwareFoundation/OpenShadingLanguage) for shading, [TBB](https://github.com/oneapi-src/oneTBB) for tasking and multithreading and [ImGui](https://github.com/ocornut/imgui) for the interface.

Soon there will be a building system with [CMAKE](https://github.com/Kitware/CMake) and [Vcpkg](https://github.com/microsoft/vcpkg).

# Roadmap

(Still in construction, I'm not sure of what I want to implement or not)

- Embree :
  - :heavy_check_mark: Triangle geometry :  
    - :heavy_check_mark: Smooth Normals
    - :heavy_check_mark: Uv coordinates
  - :x: Curve geometry
  - :x: Quad geometry
  - :x: Subdivision surface
  - :x: Instances
  - :x: Displacement
  - :x: Opacity handling

- OSL / Shading : 
  - :heavy_check_mark: Lambert Diffuse Brdf
  - :x: Sheen Brdf
  - :x: Microfacet GGX Brdf/Btdf
  - :x: Microfacet Beckmann Brdf/Btdf
  - :x: Dielectric (with Microfacet Btdf)
  - :x: Thin Dielectric
  - :x: Random Walk SubSurface Scattering Bssrdf
  - :x: Normal mapping
  - :x: Displacement mapping
  - :x: Transparency

- Lighting :
  - :heavy_check_mark: Environment light
    - :heavy_check_mark: Constant
    - :x: OSL Environment Shader
  - :x: Square Area Light
    - :x: OSL Square Light Shader
  - :x: Sky Light (I haven't choosen yet which model I want to implement)  

- I/O / Data :
  - :heavy_check_mark: Obj File format
  - :x: USD (I know that's a big boy)
  - :x: Custom scenefile format to save/load scenes
  - :x: Output to any image file format with OpenImageIO

- Interface :
  - :x: Node-Based RenderGraph
  - :x: Text Editor with syntax highlighting for OSL Shaders 

# Showcase

First OSL shader running 
![first osl shader](https://i.imgur.com/hCHEfVZ.png)
