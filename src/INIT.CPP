//***************************************************************************
// Name    - INIT.CPP
// Version - 1.4a
// Year    - 2000
// Author  - Rohin A Gosling
//
// Description
//
//   Initialization and startup functions for the Bridge Builder application.
//   Includes graphics mode setup, bridge data defaults, vertical retrace
//   synchronization, and the title screen display.
//
//***************************************************************************

#include "INIT.H"

#include <iostream.h>
#include <conio.h>
#include <bios.h>

//---------------------------------------------------------------------------
// Function: InitGraphicsMode
//
// Description:
//
//   Initializes the Borland Graphics Interface (BGI) in VGA medium
//   resolution mode (640x350).
//
//   Requests the VGA driver with VGAMED mode, calls initgraph, and
//   checks the result. 
//
//   On failure, displays the graphics error message
//   and terminates with exit code 1.
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

void InitGraphicsMode ( void )
{
	// Request VGA graphics drivers

	int gdriver = VGA;      // VGA graphics driver.
	int gmode   = VGAMED;   // VGA medium resolution mode.
	int errorcode;          // Graphics initialization result.

	// Initialize graphics mode

	initgraph ( &gdriver, &gmode, "" );


	// Read result of initialization

	errorcode = graphresult ();

	if ( errorcode != grOk )  // an error occurred
	{
		cout << "Graphics error: %s\n" << grapherrormsg ( errorcode );
		cout << "Press any key to halt:\n";
		getch ();
		exit  ( 1 );             // return with error code
	}
}

//---------------------------------------------------------------------------
// Function: InitBridgeData
//
// Description:
//
//   Initializes bridge design parameters and 3D model viewing state
//   to their default values.
//
//   Sets bridge dimensions (840x150x110 mm), segment counts, zig-zag
//   configuration, and zeroes all statistical counters. Initializes
//   the 3D camera focal length, rotation angles, velocities,
//   acceleration, and maximum velocity parameters.
//
// Parameters:
//
//   Data    (BridgeDataStruct *):   Bridge design parameters (initialized to defaults).
//   Model   (ModelData *):          3D model data (camera and rotation state initialized).
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void InitBridgeData ( BridgeDataStruct *Data, ModelData *Model )
{

	Data->Height                   = 150;     // Default bridge height (mm)
	Data->Length                   = 840;     // Default bridge length (mm)
	Data->Width                    = 110;     // Default bridge width (mm)
	Data->JointOverlap             = 5;       // Default joint overlap distance (mm)
	Data->MaterialLength           = 0;       // Total material length, computed later
	Data->NumSegments              = 6;       // Default number of arch segments
	Data->NumCrossSuportedSegments = 6;       // Default number of cross-supported segments
	Data->Configuration            = 2;       // 0 = Hanging, 1 = Support, 2 = Zig-zag
	Data->SplitVertBeams           = 0;       // Vertical beams not split by default
	Data->SplitDiagBeams           = 0;       // Diagonal beams not split by default
	Data->NumVertBeams             = 0;       // Vertical beam count, computed later
	Data->NumDiagBeams             = 0;       // Diagonal beam count, computed later
	Data->NumCrossBeams            = 0;       // Cross beam count, computed later
	Data->NumArchBeams             = 0;       // Arch beam count, computed later
	Data->NumJoints                = 0;       // Joint count, computed later

	// Initialize 3D model rotation and viewing parameters

	Model->f     = 800.0 / CoordScale;
	Model->Yaw   = 0;
	Model->Pitch = 0;
	Model->Yv    = 0;
	Model->Pv    = 0;
	Model->Ya    = 0.005;
	Model->Pa    = 0.005;
	Model->Ym    = 0.05;
	Model->Pm    = 0.05;
}

//---------------------------------------------------------------------------
// Function: WaitRetrace
//
// Description:
//
//   Waits for the monitor vertical retrace signal to synchronize
//   display page flipping.
//
//   Uses inline x86 assembly to poll VGA status register 0x3DA,
//   waiting for the vertical retrace bit (bit 3) to transition
//   from active to inactive and back, ensuring the page flip
//   occurs during the blanking interval for flicker-free animation.
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

void WaitRetrace (void)
{
	asm      MOV         DX, 0x3DA

RETRACE:;

	asm      IN          AL, DX
	asm      TEST        AL, 0x08
	asm      JNZ         RETRACE

NO_RETRACE:;

	asm      IN          AL, DX
	asm      TEST        AL, 0x08
	asm      JZ          NO_RETRACE
}

//---------------------------------------------------------------------------
// Function: ShowTitleScreen
//
// Description:
//
//   Displays the application title screen and waits for a key press.
//
//   Renders a light gray background with 3D-style beveled borders,
//   the "Bridge Builder" title with a drop shadow, the version string,
//   author credits (Rohin A Gosling, Casey Chadwick), and a "Press
//   any key to continue" prompt. Clears the screen after the user
//   presses a key.
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

void ShowTitleScreen ( void )
{
	// Fill background with light gray

	setfillstyle ( 1, 7 );
	bar          ( 0, 0, XRes - 1, YRes - 1 );

	// Draw outer border highlight (top-left edges)

	setcolor ( 15 );
	line     ( 1, 0, 1, YRes - 2 );
	line     ( 0, 0, 0, YRes - 3 );
	line     ( 0, 0, XRes - 2, 0 );
	line     ( 0, 1, XRes - 3, 1 );

	// Draw outer border shadow (bottom-right edges)

	setcolor ( 8 );
	line     ( XRes - 1, YRes - 1, XRes - 1, 1 );
	line     ( XRes - 2, YRes - 1, XRes - 2, 2 );
	line     ( XRes - 1, YRes - 1, 1, YRes - 1 );
	line     ( XRes - 1, YRes - 2, 2, YRes - 2 );

	// Draw inner border shadow (top-left edges)

	setcolor ( 8 );
	line     ( 10, 10, 10, YRes - 11 );
	line     ( 10, 10, XRes - 11, 10 );

	// Draw inner border highlight (bottom-right edges)

	setcolor ( 15 );
	line     ( XRes - 11, YRes - 11, XRes - 11, 10 );
	line     ( XRes - 11, YRes - 11, 10, YRes - 11 );

	// Draw inner border highlight (top-left edges, inset)

	setcolor ( 15 );
	line     ( 11, 11, 11, YRes - 12 );
	line     ( 11, 11, XRes - 12, 11 );

	// Draw inner border shadow (bottom-right edges, inset)

	setcolor ( 8 );
	line     ( XRes - 12, YRes - 12, XRes - 12, 11 );
	line     ( XRes - 12, YRes - 12, 11, YRes - 12 );

	// Title

	// Parse version number

	char  MajorStr   [ StringSize10 ];
	char  MinorStr   [ StringSize10 ];
	char  VersionStr [ StringSize20 ];

	itoa ( MajorVersion, MajorStr, 10 );
	itoa ( MinorVersion, MinorStr, 10 );

	strcpy ( VersionStr, "Version " );
	strcat ( VersionStr, MajorStr );
	strcat ( VersionStr, "." );
	strcat ( VersionStr, MinorStr );
	strcat ( VersionStr, "a" );

	settextjustify ( CENTER_TEXT, TOP_TEXT );
	settextstyle   ( 1, 0, 7 );

	setcolor ( 8 );

	outtextxy ( XRes / 2 + 5,       YRes / 8 + 3, "Bridge Builder" );
	outtextxy ( XRes / 2 - 1 + 5,   YRes / 8 + 3, "Bridge Builder" );
	outtextxy ( XRes / 2 + 1 + 5,   YRes / 8 + 3, "Bridge Builder" );
	outtextxy ( XRes / 2 + 5,       YRes / 8 + 4, "Bridge Builder" );
	outtextxy ( XRes / 2 - 1 + 5,   YRes / 8 + 4, "Bridge Builder" );
	outtextxy ( XRes / 2 + 1 + 5,   YRes / 8 + 4, "Bridge Builder" );

	setcolor ( 0 );

	outtextxy ( XRes / 2,       YRes / 8,       "Bridge Builder" );
	outtextxy ( XRes / 2 - 1,   YRes / 8,       "Bridge Builder" );
	outtextxy ( XRes / 2 + 1,   YRes / 8,       "Bridge Builder" );
	outtextxy ( XRes / 2,       YRes / 8 + 1,   "Bridge Builder" );
	outtextxy ( XRes / 2 - 1,   YRes / 8 + 1,   "Bridge Builder" );
	outtextxy ( XRes / 2 + 1,   YRes / 8 + 1,   "Bridge Builder" );

	// Version

	settextstyle ( 7, 0, 4 );
	setcolor     ( 8 );

	outtextxy ( XRes / 2 + 1,       9*YRes / 25 + 1, VersionStr );
	outtextxy ( XRes / 2 + 1 + 1,   9*YRes / 25 + 1, VersionStr );

	setcolor  ( 1 );
	outtextxy ( XRes / 2,       9*YRes / 25, VersionStr );
	outtextxy ( XRes / 2 + 1,   9*YRes / 25, VersionStr );

	// Titles

	settextstyle ( 2, 0, 5 );

	setcolor  ( 0 );
	outtextxy ( XRes / 2,   23*YRes / 40, "Program Design" );

	settextstyle ( 2, 0, 4 );

	setcolor  ( 4 );
	outtextxy ( XRes / 2,   25*YRes / 40, "Rohin A Gosling" );
	outtextxy ( XRes / 2,   26*YRes / 40, "Casey Chadwick" );

	settextstyle ( 2, 0, 5 );

	setcolor  ( 0 );
	outtextxy ( XRes / 2,   29*YRes / 40, "Programming and Graphics" );

	settextstyle ( 2, 0, 4 );

	setcolor  ( 4 );
	outtextxy ( XRes / 2,   31*YRes / 40, "Rohin A Gosling" );

	// Press any key massage

	settextstyle ( 2, 0, 4 );

	setcolor  ( 0 );
	outtextxy ( XRes / 2 + 1, 37*YRes / 40 + 1, "Press any key to continue" );
	setcolor  ( 15 );
	outtextxy ( XRes / 2,     37*YRes / 40,      "Press any key to continue" );

	bioskey     ( 0 );
	cleardevice ();
}
