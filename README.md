# ReflexMinimap

A work-in-progress application that displays a minimap through Raylib while playing Kirby and the Amazing Mirror on an emulator. It works through starting a server, to which a client Lua script on mGBA connects to and sends all relevant information.

As of now, the networking code has only been written for POSIX compliant systems (for example, Linux), so only on these systems does the application work.

# Build
CMake, Ninja and GCC must be installed. Then, enter:

```console
$ mkdir build
$ cd build
$ cmake -S .. --preset Release
$ cmake --build release
```

`ReflexMinimap` will then be located in the `build/release` directory.

# Usage

If the application has been started, it should display a window that it is listening on a port.
Then on mGBA, go to `Tools` > `Scripting...` > `File` > `Load script...` and select `ReflexMinimap-mgba.lua` here in the `res` directory.

If a significant performance decrease is noticeable, it is recommended to increase the relevant refresh timers at the top of the Lua script for better performance.
This will however also make the minimap application run not as smoothly.
