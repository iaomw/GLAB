# GLAB

![Preview image](/capture/Capture.G.JPG)
![Preview image](/capture/Capture.F.JPG)
![Preview image](/capture/Capture.H.JPG)

This serves as my final project for **Computer Graphics** course **2017 Fall**.

I will continue improving it for fun. But it's weird for me to continue the OpenGL project on Windows. Meanwhile, it's possible migrating it to Mac/Linux, since it's not using any Windows API. Considering CMake. 

## Features
- [ ] C++ 11~17
- [x] Wavefront.obj
- [ ] Bindless Texture
- [ ] Batch Rendering
- [ ] Approaching Zero Driver Overhead
- [x] Deferred Rendering
- [ ] Quaternion Rotation
- [ ] Cascaded Shadow Maps
- [x] Point light
    - [x] Bloom
    - [ ] Volumetric Effects
    - [ ] Directional shadow mapping
    - [x] Omnidirectional shadow mapping
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
    - [x] Transmittance
    - [x] Shadow
- [x] Post-process
    - [x] ACES tone mapping
    - [x] Basic eye adoption
    - [x] Gamma correction
    - [ ] [UE4.25 Auto Exposure](https://www.unrealengine.com/en-US/tech-blog/how-epic-games-is-handling-auto-exposure-in-4-25)

## To-fix
- Light source should be brighter.
- Adjust bloom range based on depth.
- Redone the pipeline, reduce VRAM cost.
- After switching raw pointer to smart pointer, the code runs noticeable slower.

## Dependencies
- [GLEW](http://glew.sourceforge.net/) - OpenGL loading library
- [SDL2](https://www.libsdl.org/) - Window & Context 
- [ImGui](https://github.com/ocornut/imgui) - User interface
- [GLM](https://glm.g-truc.net/) - Math operations
- [STB](https://github.com/nothings/stb) - Image loading
- [Magic Enum C++](https://github.com/Neargye/magic_enum)

## References

[opengl-tutorial.org](http://www.opengl-tutorial.org/)\
[Learn OpenGL by Joey de Vries](https://learnopengl.com/)\
[A Guide to Modern OpenGL Functions](https://github.com/Fendroidus/Guide-to-Modern-OpenGL-Functions)

[FREE PBR](https://freepbr.com/)\
[HDRI Haven](https://hdrihaven.com/)\
[CCOTextures](https://cc0textures.com/)\
[Material ball in 3D-Coat](https://sketchfab.com/3d-models/material-ball-in-3d-coat-a6bdf1d11d714e07b9dd99dda02de965)\
[McGuire Computer Graphics Archive](http://casual-effects.com/data/index.html)
