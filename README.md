# GLAB

![Preview image](/assets/Capture.E.JPG)
![Preview image](/assets/Capture.F.JPG)

This serves as my final project for **Computer Graphics** course **2017 Fall**.

I will continue improving it for fun. But it's weird for me to continue the OpenGL project on Windows. Meanwhile, it's possible migrating it to Mac/Linux, since it's not using any Windows API. Considering CMake. 

## Features
- [ ] C++ 11~17
- [x] OBJ geometry
- [x] Deferred shading
- [x] Point light
    - [x] Bloom
    - [ ] Volumetric Effects
    - [ ] Directional shadow mapping
    - [ ] Omnidirectional shadow mapping.
- [x] Physically Based Rendering
    - [x] HDR environment
    - [x] Image-based lighting
    - [x] PBR metallic roughness
    - [x] Fresnel Schlick
    - [x] Cook-Torrance BRDF
    - [x] Microfacet GGX+Smith
    - [ ] Multiple-Scattering Microfacet BSDFs with the Smith Model
- [x] [**Separable Subsurface Scattering**](http://www.iryoku.com/separable-sss/)
    - [x] Phong lighting
    - [ ] PBR lighting
    - [x] Gaussian blur 
    - [ ] Transmittance
    - [ ] Shadow
- [x] Post-process
    - [x] ACES tone mapping
    - [x] Basic eye adoption
    - [x] Gamma correction
    - [ ] [UE4.25 Auto Exposure](https://www.unrealengine.com/en-US/tech-blog/how-epic-games-is-handling-auto-exposure-in-4-25)

## To-fix
- Light source should be brighter.
- Adjust bloom range based on depth.
- Redone the pipeline, reduce VRAM cost.

## Dependencies
- [GLEW](http://glew.sourceforge.net/) - OpenGL loading library
- [SDL2](https://www.libsdl.org/) - Window & Context 
- [ImGui](https://github.com/ocornut/imgui) - User interface
- [GLM](https://glm.g-truc.net/) - Math operations
- [STD](https://github.com/nothings/stb) - Image loading
- [Magic Enum C++](https://github.com/Neargye/magic_enum)

## References

[opengl-tutorial.org](http://www.opengl-tutorial.org/)\
[Learn OpenGL by Joey de Vries](https://learnopengl.com/)\
[A Guide to Modern OpenGL Functions](https://github.com/Fendroidus/Guide-to-Modern-OpenGL-Functions)

[FREE PBR](https://freepbr.com/)\
[HDRI Haven](https://hdrihaven.com/)\
[CCOTextures](https://cc0textures.com/)\
[McGuire Computer Graphics Archive](http://casual-effects.com/data/index.html)