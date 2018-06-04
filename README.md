# rover
ROVER: an open source hybrid-parallel library for volume rendering and simulated radiography

ROVER is an application for distributed-memory volume rendering and simulated radiography focused on many-core architectures (e.g., GPU, CPU, and MIC). Volume rendering, also known as volume visualization, is a commonly used scientific visualization algorithm that enables users to visualize an entire scalar field at once by mapping scalar values to colors and transparency. Simulated radiography is a generalization of the volume rendering algorithm from four color channels (i.e., red, green, blue, and transparency) to an arbitrary number of channels representing material opacities in discrete energy groups, measured in electron volts. 

To execute on many-core architectures, ROVER leverages [VTK-m](https://gitlab.kitware.com/vtk/vtk-m). During the course of ROVER's development, we have contributed a large amount of ray tracing code to VTK-m. Moving forward, the ROVER code base will be integrated into VTK-m and [Ascent](https://github.com/Alpine-DAV/ascent), an open source in situ visualization library, so that it can be used in Exascale Computing Project applications.
