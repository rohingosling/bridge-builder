//---------------------------------------------------------------------------
// Name    - GEOMETRY.H
// Version - 1.4a
// Year    - 2000
// Author  - Rohin A Gosling
//
// Description
//
//   Bridge generation function declarations for parametric bridge model
//   construction.
//
//---------------------------------------------------------------------------

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "BTYPES.H"

void GenerateBridge    ( ModelData *Model, BridgeDataStruct *Bridge, ColorTheme *Colors, LayoutConfig *Layout );
void NormalizeCoords   ( ModelData *Model );
int  GetMaterialLength ( ModelData *Model, BridgeDataStruct *Bridge );

#endif
