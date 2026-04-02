//---------------------------------------------------------------------------
// Name    - FILEIO.H
// Version - 1.4a
// Year    - 2000
// Author  - Rohin A Gosling
//
// Description
//
//   File I/O function declarations for saving and loading bridge data.
//
// Bridge File Format
//
//   +------+------+--------------------------+-----------------------------------------------------------------+
//   | Offs | Size | Field                    | Description                                                     |
//   +------+------+--------------------------+-----------------------------------------------------------------+
//   |    0 |   20 | ID String                | `"Bridge Builder"` followed by CR, LF, NUL, padded to 20 bytes  |
//   |   20 |    2 | Major Version            | Major version number (16-bit integer, e.g. `1` for version 1.4) |
//   |   22 |    2 | Minor Version            | Minor version number (16-bit integer, e.g. `4` for version 1.4) |
//   |   24 |    4 | Height                   | Bridge height in mm (32-bit float)                              |
//   |   28 |    4 | Length                   | Bridge length in mm (32-bit float)                              |
//   |   32 |    4 | Width                    | Bridge width in mm (32-bit float)                               |
//   |   36 |    2 | Material Length          | Total construction material length in mm (16-bit integer)       |
//   |   38 |    4 | Joint Overlap            | Joint overlap distance in mm (32-bit float)                     |
//   |   42 |    2 | Num Segments             | Number of arch segments (16-bit integer)                        |
//   |   44 |    2 | Num Cross-Supported Segs | Number of cross-supported segments (16-bit integer)             |
//   |   46 |    2 | Configuration            | 0 = Hanging, 1 = Support, 2 = Zig-zag (16-bit integer)          |
//   |   48 |    2 | Split Vertical Beams     | 0 = No, 1 = Yes (16-bit enum)                                   |
//   |   50 |    2 | Split Diagonal Beams     | 0 = No, 1 = Yes (16-bit enum)                                   |
//   |   52 |    2 | Num Vertical Beams       | Total vertical beams (16-bit integer, computed)                 |
//   |   54 |    2 | Num Diagonal Beams       | Total diagonal beams (16-bit integer, computed)                 |
//   |   56 |    2 | Num Cross Beams          | Total cross-support beams (16-bit integer, computed)            |
//   |   58 |    2 | Num Arch Beams           | Total arch beams (16-bit integer, computed)                     |
//   |   60 |    2 | Num Joints               | Total joints (16-bit integer, computed)                         |
//   +------+------+--------------------------+-----------------------------------------------------------------+
//
//---------------------------------------------------------------------------

#ifndef FILEIO_H
#define FILEIO_H

#include "BTYPES.H"

void CheckFiles ( void );
void SaveFile   ( int ID, FileState *FileInfo, BridgeDataStruct *Bridge, ProgramState *State, WindowGeometry *Geo, LayoutConfig *Layout, ColorTheme *Colors, ModelData *Model );
void LoadFile   ( int ID, FileState *FileInfo, BridgeDataStruct *Bridge, ProgramState *State, WindowGeometry *Geo, LayoutConfig *Layout, ColorTheme *Colors, ModelData *Model );

#endif
