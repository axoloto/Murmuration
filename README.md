# Murmuration

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
git clone https://github.com/axoloto/Boids.git
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
