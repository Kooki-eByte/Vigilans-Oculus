# Vigilans-Oculus
Vigilans Oculus | "Watchful Eye" is a Scene Viewer using modern industry 3D-Grpahics API Vulkan and C-Style C++ coding


## Installing (Arch Linux)

When installing the vulkan-devel package I had to specific the env variable
`VK_ICD_FILENAMES` to be assigned to the path /usr/share/vulkan/icd.d/{graphics_card}_icd.json

## Make build system

MINGW is the build system choosen for windows for simplicity in building up an executable and ease of cross platform handling.

To build this exe:

[INFO] Ensure you have made a `bin/` directory in the root directory of the project

`mingw32-make` - Will compile using g++ and create the exe in the bin directory.
`mingw32-make test` - Will run the executable for you after compiling.
`mingw32-make clean` - Will remove the executable allowing you to compile if any changes were made.