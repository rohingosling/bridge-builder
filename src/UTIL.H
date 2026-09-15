//---------------------------------------------------------------------------
// Name    - UTIL.H
// Version - 1.4a
// Year    - 2000
// Author  - Rohin A Gosling
//
// Description
//
//   Utility function declarations for sound and animation feedback.
//
//---------------------------------------------------------------------------

#ifndef UTIL_H
#define UTIL_H

#include "BTYPES.H"

void Sound             ( int Sound );
void FlashTitleBar     ( int Speed, LayoutConfig *Layout );
void FlashSelectionBar ( int Speed, LayoutConfig *Layout, int DialogItem );
void FlashWindow       ( int Speed, LayoutConfig *Layout, WindowGeometry *Geo, int ActiveWindow );

#endif
