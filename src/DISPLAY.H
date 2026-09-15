//---------------------------------------------------------------------------
// Name    - DISPLAY.H
// Version - 1.4a
// Year    - 2000
// Author  - Rohin A Gosling
//
// Description
//
//   UI display function declarations for screen layout, dialog box,
//   title bar, statistics, beam data, cursor, and selection bar.
//
//---------------------------------------------------------------------------

#ifndef DISPLAY_H
#define DISPLAY_H

#include "BTYPES.H"

void DisplayScreen      ( WindowGeometry *Geo, LayoutConfig *Layout, ColorTheme *Colors, BridgeDataStruct *Bridge, FileState *FileInfo );
void DisplayTitleBar    ( LayoutConfig *Layout, int WindowColor, FileState *FileInfo );
void DisplayDialogBox   ( BridgeDataStruct *Bridge, LayoutConfig *Layout, ColorTheme *Colors );
void DisplayModelWindow ( WindowGeometry *Geo, LayoutConfig *Layout, int WindowColor );
void ShowStatData       ( int Color, BridgeDataStruct *Bridge, LayoutConfig *Layout );
void ShowBeamData       ( int Beam, ModelData *Model, WindowGeometry *Geo, BridgeDataStruct *Bridge, int DataTextColor );
void ShowDataItem       ( int DialogItem, BridgeDataStruct *Bridge, LayoutConfig *Layout, int DataTextColor );
void ShowSelectionBar   ( int Item, int Color, LayoutConfig *Layout );
void ShowCursor         ( int DialogItem, BridgeDataStruct *Bridge, LayoutConfig *Layout, int Cursor );

#endif
