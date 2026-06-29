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

#include "FILEIO.H"
#include "GEOMETRY.H"
#include "RENDER.H"
#include "DISPLAY.H"
#include "UTIL.H"

#include <conio.h>

//---------------------------------------------------------------------------
// Function: CheckFiles
//
// Description:
//
//   Verifies that all required BGI font files are present on disk.
//
//   Attempts to open LITT.CHR, TRIP.CHR, and TSCR.CHR. If any file
//   is missing, displays an error message and terminates the program
//   with exit code 1.
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

void CheckFiles ( void )
{
	FILE *File;          // File handle.
	char ErrorFlag = 0;  // Missing file indicator.

	// Check for small font file.

	if ( ( File = fopen ( "LITT.CHR", "rb" ) ) == NULL )
	{
		ErrorFlag = 1;
	}
	fclose ( File );

	// Check for triplex font file.

	if ( ( File = fopen ( "TRIP.CHR", "rb" ) ) == NULL )
	{
		ErrorFlag = 1;
	}
	fclose ( File );

	// Check for script font file.

	if ( ( File = fopen ( "TSCR.CHR", "rb" ) ) == NULL )
	{
		ErrorFlag = 1;
	}
	fclose ( File );

	// Display error message and exit program

	if ( ErrorFlag == 1 )
	{
		clrscr ();
		printf ( "\nLoad error:\n\n" );
		printf ( "   One or more of the files that this program needs\n" );
		printf ( "   to execute correctly is missing.\n\n" );
		exit   ( 1 );
	}
}

//---------------------------------------------------------------------------
// Function: SaveFile
//
// Description:
//
//   Saves the current bridge data to a .BRG file on disk.
//
//   Constructs the file name from the save slot ID, writes the file
//   ID string, version number, and bridge data struct in binary
//   format.
//
//   Resets the display to single-page mode, refreshes the screen with the 
//   updated file name, and plays save confirmation sound and animation effects.
//
// Parameters:
//
//   ID         (int):                  Save slot ID (0-9).
//   FileInfo   (FileState *):          Current file name and save slot ID (updated on save).
//   Bridge     (BridgeDataStruct *):   Bridge design parameters to save.
//   State      (ProgramState *):       Runtime state (reset to dialog mode after save).
//   Geo        (WindowGeometry *):     Model window screen bounds.
//   Layout     (LayoutConfig *):       Screen layout dimensions.
//   Colors     (ColorTheme *):         UI color palette.
//   Model      (ModelData *):          3D model data (for 2D view refresh after save).
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void SaveFile
(
	int               ID, 
	FileState        *FileInfo, 
	BridgeDataStruct *Bridge, 
	ProgramState     *State, 
	WindowGeometry   *Geo, 
	LayoutConfig     *Layout, 
	ColorTheme       *Colors, 
	ModelData        *Model
)
{
	FILE *File;              // File handle for writing.
	char Str [ StringSize10 ];  // File ID string buffer.

	// Parse file name

	itoa   ( ID, Str, 10 );
	strcpy ( FileInfo->FileName, "BRIDGE_" );
	strcat ( FileInfo->FileName, Str );
	strcat ( FileInfo->FileName, ".BRG" );

	// Initialize ID string

	char IDStr [ FileIdSize ] = "Bridge Builder";

	IDStr [ 14 ] = 13;
	IDStr [ 15 ] = 10;
	IDStr [ 16 ] = 0;

	// Open file for writing

	File = fopen ( FileInfo->FileName, "wb" );

	// Save ID string

	fwrite ( IDStr, FileIdSize, 1, File );

	// Save file version number

	fwrite ( &MajorVersion, 2, 1, File );
	fwrite ( &MinorVersion, 2, 1, File );

	// Save bridge data

	fwrite ( Bridge, sizeof ( BridgeDataStruct ), 1, File );

	// Close file

	fclose ( File );

	// Reset display state after save.

	setactivepage    ( 1 );
	setvisualpage    ( 1 );
	cleardevice      ();
	setactivepage    ( 0 );
	setvisualpage    ( 0 );
	cleardevice      ();
	DisplayScreen    ( Geo, Layout, Colors, Bridge, FileInfo );
	ShowSelectionBar ( State->DialogItem, Colors->SelectionBarColor, Layout );

	// Reset editor state.

	State->ActiveWindow = 0;
	State->EditMode     = 0;
	State->Perspective  = 0;

	// Show the 2D bridge model.

	setactivepage ( 0 );
	setvisualpage ( 0 );
	Show2DModel   ( Model, Geo, Bridge, Colors->HeadingTextColor, Colors->ModelColor1 );

	// Play save confirmation sounds.

	Sound ( 1 ); Sound ( 0 ); Sound ( 2 );

	// Flash the title bar to confirm save.

	FlashTitleBar ( 300, Layout );
}

//---------------------------------------------------------------------------
// Function: LoadFile
//
// Description:
//
//   Loads bridge data from a .BRG file on disk.
//
//   - Constructs the file name from the save slot ID and attempts to
//     open it.
//
//   - On success, reads the bridge data struct from the binary file, 
//     regenerates the bridge model, resets the display to single-page 
//     mode, and refreshes the screen.
//
//   - On failure, restores the previous file name and resets the file ID 
//     to unsaved.
//
// Parameters:
//
//   ID         (int):                  Save slot ID (0-9).
//   FileInfo   (FileState *):          Current file name and save slot ID (updated on load).
//   Bridge     (BridgeDataStruct *):   Bridge design parameters (overwritten with loaded data).
//   State      (ProgramState *):       Runtime state (reset to dialog mode after load).
//   Geo        (WindowGeometry *):     Model window screen bounds.
//   Layout     (LayoutConfig *):       Screen layout dimensions.
//   Colors     (ColorTheme *):         UI color palette.
//   Model      (ModelData *):          3D model data (regenerated from loaded bridge data).
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void LoadFile
(
	int               ID, 
	FileState        *FileInfo, 
	BridgeDataStruct *Bridge, 
	ProgramState     *State, 
	WindowGeometry   *Geo, 
	LayoutConfig     *Layout, 
	ColorTheme       *Colors, 
	ModelData        *Model
)
{
	FILE *File;                			// File handle for reading.
	char Str         [ StringSize10 ];  // File ID string buffer.
	char OldFileName [ StringSize20 ];  // Backup of current file name.

	// Save old file name

	strcpy ( OldFileName, FileInfo->FileName );

	// Parse file name

	itoa   ( ID, Str, 10 );
	strcpy ( FileInfo->FileName, "BRIDGE_" );
	strcat ( FileInfo->FileName, Str );
	strcat ( FileInfo->FileName, ".BRG" );

	// Open file for writing

	if ( ( File = fopen ( FileInfo->FileName, "rb" ) ) != NULL )
	{
		// Save bridge data

		fseek ( File, FileDataOffset, 0 );
		fread ( Bridge, sizeof ( BridgeDataStruct ), 1, File );

		// Close file

		fclose ( File );

		// Play load confirmation sounds.

		Sound ( 2 ); Sound ( 1 ); Sound ( 0 );

		// Reset display state after load.

		GenerateBridge   ( Model, Bridge, Colors, Layout );
		setactivepage    ( 1 );
		setvisualpage    ( 1 );
		cleardevice      ();
		setactivepage    ( 0 );
		setvisualpage    ( 0 );
		cleardevice      ();
		DisplayScreen    ( Geo, Layout, Colors, Bridge, FileInfo );
		ShowSelectionBar ( State->DialogItem, Colors->SelectionBarColor, Layout );

		// Reset editor state.

		State->ActiveWindow = 0;
		State->EditMode     = 0;
		State->Perspective  = 0;

		// Show the 2D bridge model.

		setactivepage ( 0 );
		setvisualpage ( 0 );
		Show2DModel   ( Model, Geo, Bridge, Colors->HeadingTextColor, Colors->ModelColor1 );
		FlashTitleBar ( 300, Layout );
	}
	else
	{
		// File not found, restore previous file name.

		FileInfo->FileID = -1;
		strcpy ( FileInfo->FileName, OldFileName );

	} // if else
}
