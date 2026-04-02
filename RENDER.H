//---------------------------------------------------------------------------
// Name    - RENDER.H
// Version - 1.4a
// Year    - 2000
// Author  - Rohin A Gosling
//
// Description
//
//   Rendering function declarations for 2D and 3D bridge model display.
//
//---------------------------------------------------------------------------

#ifndef RENDER_H
#define RENDER_H

#include "BTYPES.H"

void Show3DModel        ( ModelData *Model, WindowGeometry *Geo, BridgeDataStruct *Bridge, int HeadingTextColor );
void Show2DModel        ( ModelData *Model, WindowGeometry *Geo, BridgeDataStruct *Bridge, int HeadingTextColor, int ModelColor1 );
void ShowBeam           ( int Beam, int Color, ModelData *Model, WindowGeometry *Geo, BridgeDataStruct *Bridge );
void Clear2DModelWindow ( WindowGeometry *Geo );

#endif
