# NavSoS - Navigation Software Stuttgart

Navigation Software of the Institut of Navigation (University of Stuttgart)

## Description

This software provides real-time and post processing functionality for navigational tasks. It can read from sensors and fuse together the data. It can fuse GNSS data with IMU data and do advanced functions like RTK, RAIM, ...

The software consists of 2 parts
* Main console program "```navsos```": Reads in instructions from boost program options config files
* GUI program "```navsos-gui```": Data flow programming Qt5 GUI which exports the config file for the main program. It has main 2 advantages:
    * Completely automatically generated. The GUI Code needs no maintenance, it is generated from the Node Interfaces which are provided by the main program.
    * Completely optional because the boost program option config files can be provided manually.

## Getting Started

### Dependencies

* Needed:
    * [cmake](https://cmake.org/) A cross-platform open-source make system
    * [make](https://www.gnu.org/software/make/) GNU make utility to maintain groups of programs
    * [clang-format](https://clang.llvm.org/docs/ClangFormat.html) Code formatting Tool
    * C++ compiler ([clang](https://clang.llvm.org/) is recommended, but others work as well) for compiling the project
* Optional:
    * [Conan](https://conan.io) A distributed, open source, C/C++ package manager
    * [gnuplot](http://www.gnuplot.info/) Plotting package. Needed if any plotting is intended
    * [ccache](https://ccache.dev/) Compiler cache that speeds up recompilation by caching previous compilations
    * [valgrind](http://valgrind.org/) CPU profiling & leak detection
    * [kcachegrind](http://kcachegrind.sourceforge.net) Visualization of Performance Profiling Data
    * [doxygen](http://www.doxygen.nl/) Documentation system for C++, C, Java, IDL and PHP
    * [clang-tidy](https://clang.llvm.org/extra/clang-tidy/) Clang-based C++ "linter" tool
    * [cppcheck](http://cppcheck.sourceforge.net/) A tool for static C/C++ code analysis
* Libraries (Install yourself and change cmake link targets or let them automatically be installed by Conan):
    * [spdlog](https://github.com/gabime/spdlog) Fast C++ logging library
    * [fmt](https://github.com/fmtlib/fmt) A modern formatting library https://fmt.dev
    * [Boost](https://www.boost.org/) Free peer-reviewed portable C++ source libraries
    * [Eigen](http://eigen.tuxfamily.org) C++ template library for linear algebra: matrices, vectors, numerical solvers, and related algorithms
    * [Catch2](https://github.com/catchorg/Catch2) Modern, C++-native, header-only, test framework for unit-tests, TDD and BDD
* GUI (optional):
    * [Qt](https://www.qt.io/) A cross-platform application and UI framework

### Installing (Linux)

Most library dependencies are managed by Conan.io, so you just need to install the basics.

ArchLinux:
```
# Needed
sudo pacman -S base-devel cmake clang

# Optional
trizen -S conan # AUR package
sudo pacman -S ccache valgrind kcachegrind doxygen gnuplot cppcheck

# GUI (optional)
sudo pacman -S qt5-base
```

Ubuntu:
```
# Needed
sudo apt-get install build-essential cmake clang

# Optional
pip install conan
sudo apt-get install ccache valgrind kcachegrind doxygen gnuplot gnuplot-x11 cppcheck

# GUI (optional)
sudo apt-get install qt5-default 
```

### VSCode Configuration

It is strongly recommended to use [Visual Studio Code](https://code.visualstudio.com/) as IDE, as the needed project files are provided in the ```.vscode``` folder.

Recommended plugins for working with this project
* [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools): C/C++ IntelliSense, debugging, and code browsing.
* [CMake](https://marketplace.visualstudio.com/items?itemName=twxs.cmake): CMake langage support for Visual Studio Code
* [Doxygen Documentation Generator](https://marketplace.visualstudio.com/items?itemName=cschlosser.doxdocgen): Automatic Doxygen generation by typing ```/** + [Enter]```
* [Todo Tree](https://marketplace.visualstudio.com/items?itemName=Gruntfuggly.todo-tree): Show TODO, FIXME, etc. comment tags in a tree view
* [Code Spell Checker](https://marketplace.visualstudio.com/items?itemName=streetsidesoftware.code-spell-checker): Spelling checker for source code
* [Log File Highlighter](https://marketplace.visualstudio.com/items?itemName=emilast.LogFileHighlighter): Adds color highlighting to log files to make it easier to follow the flow of log events and identify problems.
* [Clang-Tidy](https://marketplace.visualstudio.com/items?itemName=notskm.clang-tidy) Integrates clang-tidy into VS Code
* [cmake-format](https://marketplace.visualstudio.com/items?itemName=cheshirekow.cmake-format) Format listfiles so they don't look like crap

Recommended changes to the User's ```settings.json``` (not the project .vscode/settings.json)
```
"editor.formatOnType": true,
"doxdocgen.generic.authorEmail": "your.name@nav.uni-stuttgart.de",
"doxdocgen.generic.authorName": "Y. Name",
```

Recommended changes to the User's ```keybindings.json```
```
// Place your key bindings in this file to override the defaultsauto[]
[
    {
        "key": "f6",
        "command": "workbench.action.tasks.build",
        "when": "!inDebugMode"
    },
    {
        "key": "f8",
        "command": "-editor.action.marker.nextInFiles",
        "when": "editorFocus && !editorReadonly"
    },
    {
        "key": "f8",
        "command": "workbench.action.tasks.runTask"
    }
]
```

### Executing the program

| Hotkey   | Action                                     | Default       |
| :------: | :----------------------------------------- | ------------- |
| ```F5``` | Debug the project                          | Default debug |
| ```F6``` | Run Task: ```DEBUG: Build project```       | Default build |
| ```F7``` | Run Task: ```DEBUG: Build & run project``` | Default test  |
| ```F8``` | Open Task List                             |               |

* To start the GUI, execute the Task ```GUI: Build & run project```
* If you have problems with the build, execute the Task ```CLEAN: Remove build files```
* If you want to provide tests, place them in the ```tests``` directory and execute them with the task ```TEST: Build & run```

## Help

The help function can be shown by calling the binary with the ```-h | --help``` parameter
```
navsos --help
```

## Authors

* [M.Sc. Thomas Topp](mailto:thomas.topp@nav.uni-stuttgart.de?subject=[GitLab/NavSoS]%20)
* [M.Sc. Tomke Lambertus](mailto:tomke.lambertus@nav.uni-stuttgart.de?subject=[GitLab/NavSoS]%20)
* [M.Sc. Rui Wang](mailto:rui.wang@nav.uni-stuttgart.de?subject=[GitLab/NavSoS]%20)

## Version History

```This is only a placeholder so far...```
* 0.2
    * Various bug fixes and optimizations
    * See [commit change]() or See [release history]()
* 0.1
    * Initial Release

## License

```This is only a placeholder so far...```

This project is licensed under the [NAME HERE] License - see the LICENSE.md file for details

## Acknowledgments

```This is only a placeholder so far...```

Inspiration, code snippets, etc.
* [awesome-readme](https://github.com/matiassingers/awesome-readme)
* [PurpleBooth](https://gist.github.com/PurpleBooth/109311bb0361f32d87a2)
* [dbader](https://github.com/dbader/readme-template)
* [zenorocha](https://gist.github.com/zenorocha/4526327)
* [fvcproductions](https://gist.github.com/fvcproductions/1bfc2d4aecb01a834b46)