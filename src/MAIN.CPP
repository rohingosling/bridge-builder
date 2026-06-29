//***************************************************************************
// Program title - Bridge builder
// Version       - 1.4a
// Year          - 2000
// Author        - Rohin A Gosling
//
// Description
//
//   Bridge builder is an experimental bridge design utility program used to
//   manipulate the parameters of and model the structure of parabolic
//   tied-arch bridges.
//
// File structure (version 1.4)
//
//   File name structure: BRIDGEX.BRG, where X is a number from 0..9
//
//   ---------------------------------------------------------------------
//   Size		Data
//   ---------------------------------------------------------------------
//
//   20        "Bridge Builder", 13, 10, 0
//   2			Major version number = x for version x.y
//   2			Minor version number = y for version x.y
//   38        Bridge data structure
//
//   ---------------------------------------------------------------------
//
//***************************************************************************

#include "BTYPES.H"
#include "APP.H"
#include "GEOMETRY.H"
#include "RENDER.H"
#include "DISPLAY.H"
#include "UTIL.H"
#include "FILEIO.H"
#include "INIT.H"

#include <bios.h>
#include <conio.h>

//---------------------------------------------------------------------------
// Function: main
//
// Description:
//
//   Program entry point for the Bridge Builder application.
//
//   Verifies required runtime files, initializes configuration structs,
//   allocates bridge and model data on the heap, sets up the graphics
//   mode and title screen, generates the initial bridge model, and
//   enters the main event loop. On exit, deallocates memory, restores
//   text mode, and purges the keyboard buffer.
//
// Parameters:
//
//   None.
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void main ( void )
{
	// Check that all the necessary file are present

	CheckFiles ();

	// Initialize configuration structs

	WindowGeometry Geo      = { 0, 0, 0, 0 };
	LayoutConfig   Layout   = { 17, 220, 3 };
	ColorTheme     Colors   = { 1, 9, 9, 2, 11, 10, 7, 1, 15, 9, 1, 9, 12, 4, 12, 10, 2, 7, 15 };
	ProgramState   State    = { 0, 0, 1, 1, 0, 0, 0, 0, 0 };
	FileState      FileInfo = { "BRIDGE_X.BRG", -1 };

	// Allocate bridge and model data on the heap

	BridgeDataStruct *Bridge = new BridgeDataStruct;
	ModelData        *Model  = new ModelData;

	// Initialize the bridge model data

	InitBridgeData ( Bridge, Model );


	// initialize graphics mode and set up screen

	InitGraphicsMode  ();
	ShowTitleScreen   ();
	settextjustify    ( LEFT_TEXT, TOP_TEXT );
	settextstyle      ( 2, 0, 4 );
	DisplayScreen     ( &Geo, &Layout, &Colors, Bridge, &FileInfo );
	Sound             ( 1 );
	ShowSelectionBar  ( State.DialogItem, Colors.SelectionBarColor, &Layout );
	FlashTitleBar     ( 200, &Layout );
	Sound             ( 0 );
	FlashWindow       ( 200, &Layout, &Geo, State.ActiveWindow );
	Sound             ( 1 );
	FlashSelectionBar ( 200, &Layout, State.DialogItem );
	Sound             ( 2 );

	// Generate bridge data

	GenerateBridge ( Model, Bridge, &Colors, &Layout );
	Show2DModel    ( Model, &Geo, Bridge, Colors.HeadingTextColor, Colors.ModelColor1 );

	// Execute main program loop

	MainLoop ( &State, Model, Bridge, &Geo, &Layout, &Colors, &FileInfo );

	// Deallocate memory

	delete Bridge;
	delete Model;

	// Restore text mode

	closegraph ();

	// Purge keyboard buffer

	while ( bioskey ( 1 ) != 0 )
	{
		if ( getch () == 0 ) getch ();
	}
}
