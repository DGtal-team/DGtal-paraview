# DGtal-paraview

## Overview

This repository provides a [ParaView](https://www.paraview.org/) plugin for the [DGtal Library](https://www.dgtal.org/), enabling the visualization and processing of digital object within the ParaView environment. 

## Installation

### Building the Plugin

1. Build paraview (see relevant instruction in [paraview website](https://gitlab.kitware.com/paraview/paraview/blob/master/Documentation/dev/build.md)). 
2. Build DGtal (see relevant instructions in [DGtal repository](https://github.com/DGtal-team/DGtal)).

3. Clone this repository:
   ```sh
   git clone https://github.com/DGtal-team/DGtal-paraview.git
   cd DGtal-paraview
   ```

4. Create a build directory and configure the project with CMake:
   ```sh
   mkdir build && cd build
   cmake .. -DParaView_DIR=/path/to/paraview/build
   ```

   Depending on your configuration, you may also have to set CMAKE_MODULE_PATH and CMAKE_PREFIX_PATH to point to DGtal. 

5. Compile and install the plugin:
   ```sh
   make -j $(nproc)
   make install
   ```

## Usage

1. Launch ParaView.
2. Load the plugin via **Tools > Manage Plugins**.
3. New filters and tools are available under DGtal:: namespace.

## License

This project is licensed under the GNU GPL License. See the `LICENSE` file for details.
