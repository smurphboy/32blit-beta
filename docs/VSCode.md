# Setting up Visual Studio Code <!-- omit in toc -->

These instructions cover setting up Visual Studio Code to build the CMake project.

A working knowledge of using and configuring Visual Studio Code is assumed.

- [Requirements](#requirements)
- [Windows specific setup](#windows-specific-setup)
- [Mac specific setup](#mac-specific-setup)
- [Initial setup for local builds](#initial-setup-for-local-builds)
- [IntelliSense](#intellisense)
- [CMake Arguments](#cmake-arguments)
- [Debugger configuration](#debugger-configuration)
- [Building for 32Blit](#building-for-32blit)

## Requirements

You'll need to install:

 - [Visual Studio Code](https://code.visualstudio.com/)
 - The [C/C++ extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
 - The [CMake Tools extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)

Then open the cloned repository with "Open folder...".

## Windows specific setup

Windows needs a little help to find the required dependencies for local builds.

 - First download SDL as described in the [Visual Studio](Windows-VisualStudio.md) docs.
 - Add `"SDL2_DIR": "${workspaceRoot}/vs/sdl/"` to `configureSettings` (See "CMake Arguments" below)

## Mac specific setup

If you are running Catalina or higher, you may find difficulty in debugging local builds. To fix this, you need to install the [CodeLLDB](https://marketplace.visualstudio.com/items?itemName=vadimcn.vscode-lldb) extension. Add a 'Build and attach' configuration like so:

- Add a configuration with Debug > Add Configuration
- Paste in the following

``` json
{
   "name": "Launch and Debug",
   "type": "lldb",
   "request": "launch",
   "program": "${command:cmake.launchTargetPath}"
}
```
Now, when you want to attach the debugger, run with that configuration and now your breakpoints will be respected 🎉

## Initial setup for local builds
You should get a notification asking if you want to configure the project. Click "Yes" and select "[Unspecified]" from the "Select a Kit" dropdown for a local build with the default compiler.

You should now be able to build by pressing the "⚙ Build:" button, or `F7`. You can also run an example by pressing `Shift` + `F5` and debug by pressing the "Debug" button or `Ctrl` + `F5`.

## IntelliSense

After configuring the project a "CMake Tools would like to configure IntelliSense for this folder." notification should appear, click "Allow" to configure IntelliSense. If the notification does not appear, open the command palette (`Ctrl`/`Cmd` + `Shift` + `P`) run "C/C++: Change Configuration Provider..." and select "CMake Tools".

## CMake Arguments

To set CMake arguments (like `-D32BLIT_PATH` for out-of-tree builds), you need to add them to `.vscode/settings.json`:

```json
{
  // other options...
  "cmake.configureSettings": {
    "32BLIT_PATH": "/path/to/32blit-beta"
  },
}
```

## Debugger configuration

You will need to create a `launch.json` file for debugging on some platforms, you should set `program` to `${command:cmake.launchTargetPath}` and `cwd` to `${workspaceFolder}/build`. See the [CMake Tools documentation](https://vector-of-bool.github.io/docs/vscode-cmake-tools/debugging.html#debugging-with-cmake-tools-and-launch-json) for more details.

## Building for 32Blit

Open the command palette (`Ctrl`/`Cmd` + `Shift` + `P`) and run "CMake: Edit User-Local CMake Kits".

Add this to the list:
```json
  {
    "name": "32Blit",
    "toolchainFile": "/path/to/32blit-beta/32blit.toolchain"
  },
```
(Replacing `/path/to/32blit-beta`, with the actual path.)

You should now be able to select "32Blit" as a kit. ("CMake: Change Kit" from the command palette or the button displaying the current kit at the bottom of the window). If you select a target ending with .flash from the list next to the "⚙ Build:" button, that example will be flashed to your device when you build.
