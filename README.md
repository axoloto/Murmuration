# Murmuration

Hacked version of RealTimeParticles made in two days during [SAT/MusicMotion HackLab21](https://musicmotion.org/hacklab21). 

Main differences compared to RealTimeParticles :
- Fluid model is deactivated
- Boids are generated on command through MIDI and OSC protocols
- MIDI and OSC readers are running on their own threads, read/write is thread-safe
- Particles have a lifespan and will disappear when they reach their end of life! GPU buffering allocation remains static, CPU allocator is smart enough to pick free/dead slots to generate new particles.

The project behind this version was quite unique: 

A live audiovisual performance centered on a painter whose hands movements automatically generated sounds and particles while painting. Movements were capted via the OSC protocol by accelerometers disposed on two hand rings. The particles lived on their own on a huge screen behind the painter. They were generated out of strong accelerations, disappearing when the lifespan was exhausted, coalescing for precise movements and running free as the painter pondered the next move.


**For best performance, make sure that the application runs on a discrete GPU and not default IGPU.**

# Requirements

- [Gitbash](https://git-scm.com/downloads)
- [Python](https://www.python.org/) (>= 3.5) and [pip](https://pypi.org/project/pip)
- [Conan](https://conan.io/)
- [CMake](https://cmake.org/download/)
- [NSIS](http://nsis.sourceforge.net/) (only for packaging the installer)
- C++ compiler, tested with [MSVC 15/19](https://visualstudio.microsoft.com/vs/features/cplusplus/) only for now 
- Device (GPU, IGPU or else) supporting OpenGL and OpenCL 1.2 or higher

# Build and Run

```bash
pip install conan
conan remote add conan-center https://conan.io/center/
git clone https://github.com/axoloto/Murmuration.git
cd Boids
./runApp.sh
```

# References

- [CMake](https://cmake.org/)
- [ImGui](https://github.com/ocornut/imgui)
- [Conan](https://conan.io/)
- [OpenCL](https://www.khronos.org/opencl/)
- [SDL2](https://libsdl.org/index.php)
- [Glad](https://glad.dav1d.de/)
- [spdlog](https://github.com/gabime/spdlog)
- [NSIS](http://nsis.sourceforge.net/)
- [OpenCL radix sort](https://github.com/modelflat/OCLRadixSort)
- [Simon Green N-body simulation paper](https://developer.download.nvidia.com/assets/cuda/files/particles.pdf)
- [Perlin Noise C++ implementation](https://github.com/sol-prog/Perlin_Noise)

# Notes
This is an hacked version, don't expect everything to be clean!

