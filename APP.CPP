//***************************************************************************
// Name    - APP.CPP
// Version - 1.4a
// Year    - 2000
// Author  - Rohin A Gosling
//
// Description
//
//   Main application loop for the Bridge Builder program.
//   - Keyboard input
//   - 3D rotation updates
//   - View toggling
//   - Parameter editing
//   - File operations
//
//***************************************************************************

#include "APP.H"
#include "GEOMETRY.H"
#include "RENDER.H"
#include "DISPLAY.H"
#include "UTIL.H"
#include "FILEIO.H"
#include "INIT.H"

#include <bios.h>
#include <conio.h>
#include <time.h>

//---------------------------------------------------------------------------
// Forward declarations
//---------------------------------------------------------------------------

// Application Commands

void CommandResetBridge        ( ProgramState *State, ModelData *Model, BridgeDataStruct *Bridge, WindowGeometry *Geo, LayoutConfig *Layout, ColorTheme *Colors, FileState *FileInfo );
void CommandSelectNextBeam     ( ProgramState *State, ModelData *Model, BridgeDataStruct *Bridge, WindowGeometry *Geo, LayoutConfig *Layout, ColorTheme *Colors, FileState *FileInfo );
void CommandSelectPreviousBeam ( ProgramState *State, ModelData *Model, BridgeDataStruct *Bridge, WindowGeometry *Geo, LayoutConfig *Layout, ColorTheme *Colors, FileState *FileInfo );
void CommandToggleView         ( ProgramState *State, ModelData *Model, BridgeDataStruct *Bridge, WindowGeometry *Geo, LayoutConfig *Layout, ColorTheme *Colors, FileState *FileInfo );
void CommandDialogInteract     ( ProgramState *State, ModelData *Model, BridgeDataStruct *Bridge, WindowGeometry *Geo, LayoutConfig *Layout, ColorTheme *Colors, FileState *FileInfo );
void CommandSwitchWindow       ( ProgramState *State, ModelData *Model, BridgeDataStruct *Bridge, WindowGeometry *Geo, LayoutConfig *Layout, ColorTheme *Colors, FileState *FileInfo );
void CommandNavigateUp         ( ProgramState *State, ModelData *Model, BridgeDataStruct *Bridge, WindowGeometry *Geo, LayoutConfig *Layout, ColorTheme *Colors, FileState *FileInfo );
void CommandNavigateDown       ( ProgramState *State, ModelData *Model, BridgeDataStruct *Bridge, WindowGeometry *Geo, LayoutConfig *Layout, ColorTheme *Colors, FileState *FileInfo );
void CommandEditRight          ( ProgramState *State, ModelData *Model, BridgeDataStruct *Bridge, WindowGeometry *Geo, LayoutConfig *Layout, ColorTheme *Colors, FileState *FileInfo );
void CommandEditLeft           ( ProgramState *State, ModelData *Model, BridgeDataStruct *Bridge, WindowGeometry *Geo, LayoutConfig *Layout, ColorTheme *Colors, FileState *FileInfo );
void CommandFastIncrement      ( ProgramState *State, ModelData *Model, BridgeDataStruct *Bridge, WindowGeometry *Geo, LayoutConfig *Layout, ColorTheme *Colors, FileState *FileInfo );
void CommandFastDecrement      ( ProgramState *State, ModelData *Model, BridgeDataStruct *Bridge, WindowGeometry *Geo, LayoutConfig *Layout, ColorTheme *Colors, FileState *FileInfo );

// Main loop functions

void Update3DModel             ( ProgramState *State, ModelData *Model, BridgeDataStruct *Bridge, WindowGeometry *Geo, LayoutConfig *Layout, ColorTheme *Colors, FileState *FileInfo );
int  HandleKeyboard            ( ProgramState *State, ModelData *Model, BridgeDataStruct *Bridge, WindowGeometry *Geo, LayoutConfig *Layout, ColorTheme *Colors, FileState *FileInfo );

//---------------------------------------------------------------------------
// Function: MainLoop
//
// Description:
//
//   Main application event loop.
//
//   - Polls for keyboard input and dispatches actions including bridge
//     parameter editing, dialog item navigation, 2D/3D view toggling,
//     3D model rotation, beam selection, file save/load, and bridge reset.
//
//   - In 3D perspective mode, performs continuous rotation updates with
//     angular deceleration and double-buffered page flipping.
//
//   - Runs until the user presses Escape.
//
// Parameters:
//
//   State      (ProgramState *):       Runtime state (active window, edit mode, perspective, cursor).
//   Model      (ModelData *):          3D model data (beam array, camera angles, rotation velocities).
//   Bridge     (BridgeDataStruct *):   Bridge design parameters (dimensions, segments, configuration).
//   Geo        (WindowGeometry *):     Model window screen bounds.
//   Layout     (LayoutConfig *):       Screen layout dimensions.
//   Colors     (ColorTheme *):         UI color palette.
//   FileInfo   (FileState *):          Current file name and save slot ID.
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void MainLoop
(
	 ProgramState        *State,
	 ModelData           *Model,
	 BridgeDataStruct    *Bridge,
	 WindowGeometry      *Geo,
	 LayoutConfig        *Layout,
	 ColorTheme          *Colors,
	 FileState           *FileInfo
)
{
	// Local variables

	int  ch          = 0;            // Keyboard character code
	int  CursorSpeed = 1;            // Cursor blink speed
	int  Ct1         = 0, Ct2 = 0;   // Timer

	// Initialize visual and display pages

	State->ActivePage = 1;
	State->VisualPage = 0;

	// Execute main loop

	State->Cursor      = 0;
	State->CursorCount = 0;

	while ( ch != 27 )
	{
		// Update 3D model

		Update3DModel ( State, Model, Bridge, Geo, Layout, Colors, FileInfo );

		// Handle keyboard input

		ch = HandleKeyboard ( State, Model, Bridge, Geo, Layout, Colors, FileInfo );

		// Update cursor

		Ct2 = Ct1;
		Ct1 = clock ();

		if ( Ct1 - Ct2 >= CursorSpeed ) State->Cursor++;
		if ( State->Cursor > 3 ) State->Cursor = 0;

		// Show blinking cursor on active parameter when in edit mode

		if ( State->EditMode == 1 )
		{
			ShowCursor ( State->DialogItem, Bridge, Layout, State->Cursor );
		}
	}

	// Purge keyboard

	while ( bioskey ( 1 ) != 0 ) { if ( getch () == 0 ) getch (); }
}

//---------------------------------------------------------------------------
// Function: Update3DModel
//
// Description:
//
//   Updates the 3D model view when in perspective mode with the model
//   window active.
//
//   Applies angular velocity to pitch and yaw, wraps angles to the
//   0..2*PI range, decelerates rotation, and renders the updated 3D view
//   using double-buffered page flipping.
//
// Parameters:
//
//   State      (ProgramState *):       Runtime state (active page, visual page, edit mode).
//   Model      (ModelData *):          3D model data (rotation angles and velocities).
//   Bridge     (BridgeDataStruct *):   Bridge design parameters.
//   Geo        (WindowGeometry *):     Model window screen bounds.
//   Layout     (LayoutConfig *):       Screen layout dimensions.
//   Colors     (ColorTheme *):         UI color palette.
//   FileInfo   (FileState *):          Current file name and save slot ID.
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void Update3DModel
(
	ProgramState        *State,
	ModelData           *Model,
	BridgeDataStruct    *Bridge,
	WindowGeometry      *Geo,
	LayoutConfig        *Layout,
	ColorTheme          *Colors,
	FileState           *FileInfo
)
{
	// Render 3D model when perspective mode is active

	if ( ( State->Perspective == 1 ) && ( State->ActiveWindow == 1 ) )
	{
		// Update model rotation angles

		Model->Pitch += Model->Pv;
		Model->Yaw   += Model->Yv;

		// Wrap angles to 0..2*PI range and apply angular deceleration

		if ( Model->Pitch > 2*M_PI ) Model->Pitch = 0;
		if ( Model->Pitch < 0 )      Model->Pitch = 2*M_PI;

		if ( Model->Yaw > 2*M_PI )   Model->Yaw = 0;
		if ( Model->Yaw < 0 )        Model->Yaw = 2*M_PI;

		if ( Model->Pv > 0 )         Model->Pv -= RotationDamping;
		if ( Model->Pv < 0 )         Model->Pv += RotationDamping;

		if ( Model->Yv > 0 )         Model->Yv -= RotationDamping;
		if ( Model->Yv < 0 )         Model->Yv += RotationDamping;

		// Clamp velocity to zero when below threshold

		if ( ( Model->Pv >= -Model->Pa / 2 ) && ( Model->Pv <= Model->Pa / 2 ) ) Model->Pv = 0;
		if ( ( Model->Yv >= -Model->Ya / 2 ) && ( Model->Yv <= Model->Ya / 2 ) ) Model->Yv = 0;

		// Flip display pages

		if ( State->ActivePage == 0 )
		{
			State->ActivePage = 1;
			State->VisualPage = 0;
		}
		else
		{
			State->ActivePage = 0;
			State->VisualPage = 1;
		}

		// Set active and visual display pages

		setactivepage ( State->ActivePage );
		setvisualpage ( State->VisualPage );

		// Update 3D image

		WaitRetrace   ();
		cleardevice   ();
		Show3DModel   ( Model, Geo, Bridge, Colors->HeadingTextColor );
		DisplayScreen ( Geo, Layout, Colors, Bridge, FileInfo );

		// Update 3D image

		if ( ( State->EditMode == 0 ) && ( State->ActiveWindow == 0 ) )
		{
			ShowSelectionBar ( State->DialogItem, Colors->SelectionBarColor, Layout );
		}

	} // if
}

//---------------------------------------------------------------------------
// Function: HandleKeyboard
//
// Description:
//
//   Checks for and processes a single keyboard event.
//
//   - Handles standard keystrokes (bridge reset, beam selection, view
//     toggle.
//
//   - Enter for parameter editing.
//
//   - Tab for window switching, digit keys for file load) and extended
//     keystrokes (arrow keys for navigation and parameter adjustment,
//     Alt+digit for file save).
//
//   - Purges any remaining keys from the keyboard buffer after processing.
//
// Parameters:
//
//   State      (ProgramState *):       Runtime state (active window, edit mode, perspective, cursor).
//   Model      (ModelData *):          3D model data (beam array, camera angles, rotation velocities).
//   Bridge     (BridgeDataStruct *):   Bridge design parameters (dimensions, segments, configuration).
//   Geo        (WindowGeometry *):     Model window screen bounds.
//   Layout     (LayoutConfig *):       Screen layout dimensions.
//   Colors     (ColorTheme *):         UI color palette.
//   FileInfo   (FileState *):          Current file name and save slot ID.
//
// Return Value:
//
//   The keyboard character code, or 0 if no key was pressed.
//   Returns 27 (Escape) to signal program exit.
//
//---------------------------------------------------------------------------

int HandleKeyboard
(
	ProgramState        *State,
	ModelData           *Model,
	BridgeDataStruct    *Bridge,
	WindowGeometry      *Geo,
	LayoutConfig        *Layout,
	ColorTheme          *Colors,
	FileState           *FileInfo
)
{
	// Local variables

	union { unsigned int a; unsigned char b [ 2 ]; } ScanCode;

	int ch = 0;   // Keyboard character code

	// Check for key stroke

	if ( bioskey ( 1 ) != 0 )
	{
		ScanCode.a = bioskey ( 0 );

		if ( ch = ScanCode.b [ 0 ] )
		{
			// Standard keystrokes

			switch ( ch )
			{
				// *

				case 42: CommandResetBridge ( State, Model, Bridge, Geo, Layout, Colors, FileInfo ); break;

				// +

				case 61: 
				case 43: CommandSelectNextBeam ( State, Model, Bridge, Geo, Layout, Colors, FileInfo ); break;

				// -

				case 45: CommandSelectPreviousBeam ( State, Model, Bridge, Geo, Layout, Colors, FileInfo ); break;

				// V and v

				case 118: 
				case 86: CommandToggleView ( State, Model, Bridge, Geo, Layout, Colors, FileInfo ); break;

				// Enter

				case 13: CommandDialogInteract ( State, Model, Bridge, Geo, Layout, Colors, FileInfo ); break;

				// Tab

				case 9: CommandSwitchWindow ( State, Model, Bridge, Geo, Layout, Colors, FileInfo ); break;

				// Default

				default:
				{
					// Load file (digit keys 0-9)

					if ( ( ch >= 48 ) && ( ch <= 57 ) )
					{
						FileInfo->FileID = ch - 48;
						LoadFile ( FileInfo->FileID, FileInfo, Bridge, State, Geo, Layout, Colors, Model );
					}
				} break;

			} // switch
		}
		else
		{
			// Extended keystrokes

			ch = ScanCode.b [ 1 ];

			switch ( ch )
			{
				// Up arrow

				case 72: CommandNavigateUp ( State, Model, Bridge, Geo, Layout, Colors, FileInfo ); break;

				// Down arrow

				case 80: CommandNavigateDown ( State, Model, Bridge, Geo, Layout, Colors, FileInfo ); break;

				// Right arrow

				case 77: CommandEditRight ( State, Model, Bridge, Geo, Layout, Colors, FileInfo ); break;

				// Left arrow

				case 75: CommandEditLeft ( State, Model, Bridge, Geo, Layout, Colors, FileInfo ); break;

				// Ctrl right arrow

				case 116: CommandFastIncrement ( State, Model, Bridge, Geo, Layout, Colors, FileInfo ); break;

				// Ctrl left arrow

				case 115: CommandFastDecrement ( State, Model, Bridge, Geo, Layout, Colors, FileInfo ); break;

				// Default

				default:
				{
					// Save file (Alt+digit keys)

					if ( ( ch >= 120 ) && ( ch <= 128 ) )
					{
						FileInfo->FileID = ch - 119;
						SaveFile ( FileInfo->FileID, FileInfo, Bridge, State, Geo, Layout, Colors, Model );
					}
					else if ( ch == 129 )
					{
						FileInfo->FileID = 0;
						SaveFile ( FileInfo->FileID, FileInfo, Bridge, State, Geo, Layout, Colors, Model );
					}
				} break;

			} // switch

		} // if else

		// Purge keyboard

		while ( bioskey ( 1 ) != 0 ) { if ( getch () == 0 ) getch (); }

	} // if

	return ch;
}

//---------------------------------------------------------------------------
// Function: CommandResetBridge
//
// Description:
//
//   Resets the bridge to default parameters and refreshes the display.
//   Triggered by the asterisk (*) key.
//
//   Reinitializes bridge data, regenerates the model, animates UI
//   feedback (title bar, window, and selection bar flashes), resets
//   the file state to unsaved, and returns to 2D dialog mode.
//
// Parameters:
//
//   State      (ProgramState *):       Runtime state (active window, edit mode, perspective).
//   Model      (ModelData *):          3D model data (reset via InitBridgeData).
//   Bridge     (BridgeDataStruct *):   Bridge design parameters (reset to defaults).
//   Geo        (WindowGeometry *):     Model window screen bounds.
//   Layout     (LayoutConfig *):       Screen layout dimensions.
//   Colors     (ColorTheme *):         UI color palette (heading, model, selection bar colors).
//   FileInfo   (FileState *):          Current file name and save slot ID (reset to unsaved).
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void CommandResetBridge
(
	ProgramState        *State,
	ModelData           *Model,
	BridgeDataStruct    *Bridge,
	WindowGeometry      *Geo,
	LayoutConfig        *Layout,
	ColorTheme          *Colors,
	FileState           *FileInfo
)
{
	// Play reset sound effect

	Sound ( 1 ); Sound ( 3 ); Sound ( 1 );

	// Reset bridge data and regenerate model

	InitBridgeData ( Bridge, Model );
	GenerateBridge ( Model, Bridge, Colors, Layout );
	Show2DModel    ( Model, Geo, Bridge, Colors->HeadingTextColor, Colors->ModelColor1 );

	// Animate UI elements during reset

	FlashTitleBar     ( 150, Layout ); Sound ( 0 );
	FlashWindow       ( 150, Layout, Geo, State->ActiveWindow ); Sound ( 1 );
	FlashSelectionBar ( 150, Layout, State->DialogItem ); Sound ( 2 );
	GenerateBridge    ( Model, Bridge, Colors, Layout );
	strcpy            ( FileInfo->FileName, "BRIDGE_X.BRG" );

	// Reset file ID to indicate unsaved state

	FileInfo->FileID = -1;

	cleardevice      ();
	DisplayScreen    ( Geo, Layout, Colors, Bridge, FileInfo );
	ShowSelectionBar ( State->DialogItem, Colors->SelectionBarColor, Layout );

	// Reset window state to dialog mode

	State->ActiveWindow = 0;
	State->EditMode     = 0;
	State->Perspective  = 0;

	// Reset display to single page and show 2D model

	setactivepage ( 0 );
	setvisualpage ( 0 );
	Show2DModel   ( Model, Geo, Bridge, Colors->HeadingTextColor, Colors->ModelColor1 );

	// Play reset completion sound effect

	Sound ( 0 ); Sound ( 3 ); Sound ( 1 );
	Sound ( 0 ); Sound ( 3 ); Sound ( 4 );
}

//---------------------------------------------------------------------------
// Function: CommandSelectNextBeam
//
// Description:
//
//   Advances to the next visible beam in 2D view and highlights it.
//   Triggered by the plus (+) or equals (=) key.
//
//   Skips beams whose vertices have negative z-coordinates (not visible
//   in the current side view). Wraps around to beam 0 when the end of
//   the beam array is reached.
//
// Parameters:
//
//   State      (ProgramState *):       Runtime state (perspective, active window, selected beam).
//   Model      (ModelData *):          3D model data (beam array, beam count).
//   Bridge     (BridgeDataStruct *):   Bridge design parameters.
//   Geo        (WindowGeometry *):     Model window screen bounds.
//   Layout     (LayoutConfig *):       Screen layout dimensions.
//   Colors     (ColorTheme *):         UI color palette (heading, model, data text colors).
//   FileInfo   (FileState *):          Current file name and save slot ID.
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void CommandSelectNextBeam
(
	ProgramState        *State,
	ModelData           *Model,
	BridgeDataStruct    *Bridge,
	WindowGeometry      *Geo,
	LayoutConfig        *Layout,
	ColorTheme          *Colors,
	FileState           *FileInfo
)
{
	// Handle next beam selection in 2D view

	if ( ( State->Perspective == 0 ) && ( State->ActiveWindow == 1 ) )
	{
		// Advance to next beam index

		State->SelectedBeam++;

		// Skip beams not visible in current side view

		while ( !( ( Model->Beam [ State->SelectedBeam ].v1.z >= 0 ) && ( Model->Beam [ State->SelectedBeam ].v2.z >= 0 ) ) )
		{
			State->SelectedBeam++;
		}

		// Wrap around to first beam if past end

		if ( State->SelectedBeam >= Model->NumBeams )
		{
			State->SelectedBeam = 0;
		}

		// Refresh 2D view and highlight selected beam

		Clear2DModelWindow ( Geo );
		Show2DModel        ( Model, Geo, Bridge, Colors->HeadingTextColor, Colors->ModelColor1 );
		ShowBeam           ( State->SelectedBeam, 14, Model, Geo, Bridge );
		ShowBeamData       ( State->SelectedBeam, Model, Geo, Bridge, Colors->DataTextColor );
		Sound              ( 4 );
	}
}

//---------------------------------------------------------------------------
// Function: CommandSelectPreviousBeam
//
// Description:
//
//   Moves to the previous visible beam in 2D view and highlights it.
//   Triggered by the minus (-) key.
//
//   Skips beams whose vertices have negative z-coordinates (not visible
//   in the current side view). Wraps around to the last beam when the
//   beginning of the beam array is passed.
//
// Parameters:
//
//   State      (ProgramState *):       Runtime state (perspective, active window, selected beam).
//   Model      (ModelData *):          3D model data (beam array, beam count).
//   Bridge     (BridgeDataStruct *):   Bridge design parameters.
//   Geo        (WindowGeometry *):     Model window screen bounds.
//   Layout     (LayoutConfig *):       Screen layout dimensions.
//   Colors     (ColorTheme *):         UI color palette (heading, model, data text colors).
//   FileInfo   (FileState *):          Current file name and save slot ID.
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void CommandSelectPreviousBeam
(
	ProgramState        *State,
	ModelData           *Model,
	BridgeDataStruct    *Bridge,
	WindowGeometry      *Geo,
	LayoutConfig        *Layout,
	ColorTheme          *Colors,
	FileState           *FileInfo
)
{
	// Handle previous beam selection in 2D view

	if ( ( State->Perspective == 0 ) && ( State->ActiveWindow == 1 ) )
	{
		// Move to previous beam index

		State->SelectedBeam--;

		// Skip beams not visible in current side view

		while ( !( ( Model->Beam [ State->SelectedBeam ].v1.z >= 0 ) && ( Model->Beam [ State->SelectedBeam ].v2.z >= 0 ) ) )
		{
			State->SelectedBeam--;
		}

		// Wrap around to last beam if past beginning

		if ( State->SelectedBeam < 0 )
		{
			State->SelectedBeam = Model->NumBeams - 1;
		}

		// Refresh 2D view and highlight selected beam

		Clear2DModelWindow ( Geo );
		Show2DModel        ( Model, Geo, Bridge, Colors->HeadingTextColor, Colors->ModelColor1 );
		ShowBeam           ( State->SelectedBeam, 14, Model, Geo, Bridge );
		ShowBeamData       ( State->SelectedBeam, Model, Geo, Bridge, Colors->DataTextColor );
		Sound              ( 4 );
	}
}

//---------------------------------------------------------------------------
// Function: CommandToggleView
//
// Description:
//
//   Toggles between 2D orthographic and 3D perspective views.
//   Triggered by the V or v key.
//
//   Resets to single-page display, redraws the screen, and switches
//   the perspective mode. When switching to 3D, performs a full
//   redraw with double-buffered page setup. When switching to 2D,
//   shows the selected beam highlight if the model window is active.
//
// Parameters:
//
//   State      (ProgramState *):       Runtime state (perspective, active window, edit mode).
//   Model      (ModelData *):          3D model data (passed to 2D and 3D renderers).
//   Bridge     (BridgeDataStruct *):   Bridge design parameters.
//   Geo        (WindowGeometry *):     Model window screen bounds.
//   Layout     (LayoutConfig *):       Screen layout dimensions.
//   Colors     (ColorTheme *):         UI color palette (heading, model, data text, selection bar colors).
//   FileInfo   (FileState *):          Current file name and save slot ID.
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void CommandToggleView
(
	ProgramState        *State,
	ModelData           *Model,
	BridgeDataStruct    *Bridge,
	WindowGeometry      *Geo,
	LayoutConfig        *Layout,
	ColorTheme          *Colors,
	FileState           *FileInfo
)
{
	// Reset display to single page

	setactivepage ( 0 );
	cleardevice   ();
	DisplayScreen ( Geo, Layout, Colors, Bridge, FileInfo );
	setvisualpage ( 0 );

	// Togle 2D/3D view

	if ( State->Perspective == 0 )
	{
		Clear2DModelWindow ( Geo );
		State->Perspective = 1;
		Show3DModel ( Model, Geo, Bridge, Colors->HeadingTextColor );
	}
	else
	{
		Clear2DModelWindow ( Geo );
		State->Perspective = 0;
		Show2DModel ( Model, Geo, Bridge, Colors->HeadingTextColor, Colors->ModelColor1 );

		if ( State->ActiveWindow == 1 )
		{
			ShowBeam     ( State->SelectedBeam, 14, Model, Geo, Bridge );
			ShowBeamData ( State->SelectedBeam, Model, Geo, Bridge, Colors->DataTextColor );
		}
	}

	// Play view toggle sound effect

	Sound ( 3 ); Sound ( 0 );

	// Update 3D display

	if ( State->Perspective == 1 )
	{
		cleardevice   ();
		Show3DModel   ( Model, Geo, Bridge, Colors->HeadingTextColor );
		DisplayScreen ( Geo, Layout, Colors, Bridge, FileInfo );

		if ( ( State->EditMode == 0 ) && ( State->ActiveWindow == 0 ) )
		{
			ShowSelectionBar ( State->DialogItem, Colors->SelectionBarColor, Layout );
		}
	}
}

//---------------------------------------------------------------------------
// Function: CommandDialogInteract
//
// Description:
//
//   Handles Enter key interaction in the dialog box. Toggles edit mode
//   for numeric parameters (items 1-7) and cycles configuration settings
//   for toggle items (items 8-10).
//
//   - Items 1-7 (numeric): toggles edit mode on/off. On exit, regenerates
//     the bridge model and refreshes statistics and the model view.
//
//   - Item 8 (Configuration): cycles through Hanging, Support, Zig-zag.
//
//   - Item 9 (Split Vertical Beams): toggles Yes/No.
//
//   - Item 10 (Split Diagonal Beams): toggles Yes/No.
//
// Parameters:
//
//   State      (ProgramState *):       Runtime state (active window, dialog item, edit mode, cursor, perspective).
//   Model      (ModelData *):          3D model data (regenerated on edit mode exit).
//   Bridge     (BridgeDataStruct *):   Bridge design parameters (configuration, split beam toggles).
//   Geo        (WindowGeometry *):     Model window screen bounds.
//   Layout     (LayoutConfig *):       Screen layout dimensions (title bar height, window gap).
//   Colors     (ColorTheme *):         UI color palette (data text, heading, model, selection bar colors).
//   FileInfo   (FileState *):          Current file name and save slot ID.
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void CommandDialogInteract
(
	ProgramState        *State,
	ModelData           *Model,
	BridgeDataStruct    *Bridge,
	WindowGeometry      *Geo,
	LayoutConfig        *Layout,
	ColorTheme          *Colors,
	FileState           *FileInfo
)
{
	// Local variables

	int  x3  = DialogDataColumnX;                                					// Dialog box data column
	int  y   = DialogContentOffsetY + Layout->TitleBarHeight + Layout->WindowGap;  	// Dialog box top position
	int  dy  = DialogRowHeight;                                 					// Dialog item row height
	char Str [ StringSize10 ] = "";                             					// Temporary string buffer

	// Handle Enter key for dialog interaction

	if ( State->ActiveWindow == 0 )
	{
		Sound ( 1 );

		// Flash cursor and selection bar for editable items

		if ( ( State->DialogItem >= 1 ) && ( State->DialogItem <= 7 ) )
		{
			State->Cursor = 0;
			ShowCursor ( State->DialogItem, Bridge, Layout, State->Cursor );
		}

		FlashSelectionBar ( 300, Layout, State->DialogItem );
		Sound             ( 2 );

		// Toggle edit mode for numeric data items

		if ( ( State->DialogItem >= 1 ) && ( State->DialogItem <= 7 ) )
		{
			if ( State->EditMode == 0 )
			{
				State->EditMode = 1;
			}
			else
			{
				State->EditMode = 0;
				ShowDataItem ( State->DialogItem, Bridge, Layout, Colors->DataTextColor );

			} // if else

		} // if

		// Handle toggle actions for configuration items

		if ( ( State->DialogItem >= 8 ) && ( State->DialogItem <= 10 ) )
		{
			switch ( State->DialogItem )
			{
				// Configuration

				case 8: 
				{
					// Clear old text

					setcolor  ( 0 );
					strcpy    ( Str, "Support" );
					outtextxy ( x3, y + 8*dy, Str );
					strcpy    ( Str, "Hanging" );
					outtextxy ( x3, y + 8*dy, Str );
					strcpy    ( Str, "Zig-zag" );
					outtextxy ( x3, y + 8*dy, Str );

					// Increment configuration

					Bridge->Configuration++;

					if ( Bridge->Configuration > 2 )
					{
						Bridge->Configuration = 0;
					}

					// Display updated configuration name

					switch ( Bridge->Configuration )
					{
						case 0:
						{
							strcpy ( Str, "Hanging" );
						} break;

						case 1:
						{
							strcpy ( Str, "Support" );
						} break;

						case 2:
						{
							strcpy ( Str, "Zig-zag" );
						} break;

					} // switch

					setcolor  ( Colors->DataTextColor );
					outtextxy ( x3, y + 8*dy, Str );

				} break;

				// Split vertical beams

				case 9:  
				{
					// Clear old split vertical beams text

					setcolor  ( 0 );
					strcpy    ( Str, "Yes" );
					outtextxy ( x3, y + 9*dy, Str );
					strcpy    ( Str, "No" );
					outtextxy ( x3, y + 9*dy, Str );

					// Toggle split vertical beams setting

					if ( Bridge->SplitVertBeams == 0 )
					{
						Bridge->SplitVertBeams = 1;
						strcpy ( Str, "Yes" );
					}
					else
					{
						Bridge->SplitVertBeams = 0;
						strcpy ( Str, "No" );

					} // if else

					setcolor  ( Colors->DataTextColor );
					outtextxy ( x3, y + 9*dy, Str );

				} break;

				// Split diagonal beams

				case 10: 
				{
					// Clear old split diagonal beams text

					setcolor  ( 0 );
					strcpy    ( Str, "Yes" );
					outtextxy ( x3, y + 10*dy, Str );
					strcpy    ( Str, "No" );
					outtextxy ( x3, y + 10*dy, Str );

					if ( Bridge->SplitDiagBeams == 0 )
					{
						Bridge->SplitDiagBeams = 1;
						strcpy ( Str, "Yes" );
					}
					else
					{
						Bridge->SplitDiagBeams = 0;
						strcpy ( Str, "No" );
					}

					setcolor  ( Colors->DataTextColor );
					outtextxy ( x3, y + 10*dy, Str );

				} break;

			} // switch

		} // if

		// Update display

		if ( State->EditMode == 0 )
		{
			GenerateBridge ( Model, Bridge, Colors, Layout );
			ShowStatData   ( Colors->DataTextColor, Bridge, Layout );

			if ( State->Perspective == 0 )
			{
				Clear2DModelWindow ( Geo );
				Show2DModel        ( Model, Geo, Bridge, Colors->HeadingTextColor, Colors->ModelColor1 );
			}
			else
			{
			} // if

		} // if

		// Update 3D display

		if ( State->Perspective == 1 )
		{
			cleardevice   ();
			Show3DModel   ( Model, Geo, Bridge, Colors->HeadingTextColor );
			DisplayScreen ( Geo, Layout, Colors, Bridge, FileInfo );

			if ( State->ActiveWindow == 0 )
			{
				ShowSelectionBar ( State->DialogItem, Colors->SelectionBarColor, Layout );
			}

		} // if

	} // if
}

//---------------------------------------------------------------------------
// Function: CommandSwitchWindow
//
// Description:
//
//   Switches focus between the dialog box and model window.
//   Triggered by the Tab key.
//
//   Resets to single-page display, updates the selection bar visibility,
//   refreshes the model view for the new active window, exits edit mode
//   if active, and plays a window flash animation.
//
// Parameters:
//
//   State      (ProgramState *):       Runtime state (active window, perspective, dialog item, edit mode).
//   Model      (ModelData *):          3D model data (passed to 2D and 3D renderers).
//   Bridge     (BridgeDataStruct *):   Bridge design parameters.
//   Geo        (WindowGeometry *):     Model window screen bounds.
//   Layout     (LayoutConfig *):       Screen layout dimensions.
//   Colors     (ColorTheme *):         UI color palette (selection bar, heading, model, data text colors).
//   FileInfo   (FileState *):          Current file name and save slot ID.
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void CommandSwitchWindow
(
	ProgramState        *State,
	ModelData           *Model,
	BridgeDataStruct    *Bridge,
	WindowGeometry      *Geo,
	LayoutConfig        *Layout,
	ColorTheme          *Colors,
	FileState           *FileInfo
)
{
	// Reset display to single page

	setactivepage ( 0 );
	setvisualpage ( 0 );

	// Set active window

	if ( State->ActiveWindow == 0 )
	{
		State->ActiveWindow = 1;
		ShowSelectionBar ( State->DialogItem, 0, Layout );

		if ( State->Perspective == 0 )
		{
			Clear2DModelWindow ( Geo );
			Show2DModel        ( Model, Geo, Bridge, Colors->HeadingTextColor, Colors->ModelColor1 );
			ShowBeam           ( State->SelectedBeam, 14, Model, Geo, Bridge );
			ShowBeamData       ( State->SelectedBeam, Model, Geo, Bridge, Colors->DataTextColor );
		}
		else
		{
			Clear2DModelWindow ( Geo );
			Show3DModel        ( Model, Geo, Bridge, Colors->HeadingTextColor );			
		}
	}
	else
	{
		State->ActiveWindow = 0;
		//Perspective = 0;

		ShowSelectionBar ( State->DialogItem, Colors->SelectionBarColor, Layout );

		if ( State->Perspective == 0 )
		{
			Clear2DModelWindow ( Geo );
			Show2DModel        ( Model, Geo, Bridge, Colors->HeadingTextColor, Colors->ModelColor1 );
		}
		else
		{
		} // if

	} // if else

	if ( ( State->DialogItem >= 1 ) && ( State->DialogItem <= 7 ) )
	{
		ShowDataItem ( State->DialogItem, Bridge, Layout, Colors->DataTextColor );
		State->EditMode = 0;
	} // if

	// Play window switch sound effect and animate

	Sound       ( 1 );
	FlashWindow ( 300, Layout, Geo, State->ActiveWindow );
	Sound       ( 2 );
}

//---------------------------------------------------------------------------
// Function: CommandNavigateUp
//
// Description:
//
//   Navigates up in the dialog box or applies upward pitch rotation
//   in 3D view. Triggered by the Up arrow key.
//
//   In dialog mode, moves the selection bar to the previous item with
//   wrap-around (item 1 wraps to 9, item 6 wraps to 3 to skip the
//   statistics section). In 3D model mode, increases pitch velocity
//   and applies cross-axis yaw damping.
//
// Parameters:
//
//   State      (ProgramState *):       Runtime state (edit mode, active window, dialog item).
//   Model      (ModelData *):          3D model data (pitch/yaw velocities and acceleration).
//   Bridge     (BridgeDataStruct *):   Bridge design parameters.
//   Geo        (WindowGeometry *):     Model window screen bounds.
//   Layout     (LayoutConfig *):       Screen layout dimensions.
//   Colors     (ColorTheme *):         UI color palette (data text, selection bar colors).
//   FileInfo   (FileState *):          Current file name and save slot ID.
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void CommandNavigateUp
(
	ProgramState        *State,
	ModelData           *Model,
	BridgeDataStruct    *Bridge,
	WindowGeometry      *Geo,
	LayoutConfig        *Layout,
	ColorTheme          *Colors,
	FileState           *FileInfo
)
{
	// Handle dialog navigation when in dialog mode

	if ( ( State->EditMode == 0 ) && ( State->ActiveWindow == 0 ) )
	{
		// Commit current edit and exit edit mode if on a data item

		if ( ( State->DialogItem >= 1 ) && ( State->DialogItem <= 7 ) )
		{
			ShowDataItem ( State->DialogItem, Bridge, Layout, Colors->DataTextColor );
			State->EditMode = 0;
		}

		// Hide selection bar at current position

		ShowSelectionBar ( State->DialogItem, 0, Layout );

		// Move to previous dialog item with wrap-around

		if ( State->DialogItem == 1 )
		{
			State->DialogItem = 9;
		}
		else if ( State->DialogItem == 6 )
		{
			State->DialogItem = 3;
		}
		else
		{
			State->DialogItem--;
		}

		// Show selection bar at new position

		ShowSelectionBar ( State->DialogItem, Colors->SelectionBarColor, Layout );
		Sound            ( 0 );
	}

	// Handle 3D model rotation when in perspective mode

	if ( ( State->ActiveWindow == 1 ) && ( State->Perspective == 1 ) )
	{
		// Increase pitch velocity

		Model->Pv += Model->Pa;

		// Clamp pitch velocity to maximum

		if ( Model->Pv > Model->Pm )
		{
			Model->Pv -= Model->Pa;
		}

		// Apply cross-axis yaw damping

		if ( Model->Yv < 0 )
		{
			Model->Yv += RotationDamping;
		}
		else
		{
			Model->Yv -= RotationDamping;
		}
	}
}

//---------------------------------------------------------------------------
// Function: CommandNavigateDown
//
// Description:
//
//   Navigates down in the dialog box or applies downward pitch rotation
//   in 3D view. Triggered by the Down arrow key.
//
//   In dialog mode, moves the selection bar to the next item with
//   wrap-around (item 9 wraps to 1, item 3 wraps to 6 to skip the
//   statistics section).
//
//   In 3D model mode, decreases pitch velocity and applies cross-axis yaw damping.
//
// Parameters:
//
//   State      (ProgramState *):       Runtime state (edit mode, active window, dialog item).
//   Model      (ModelData *):          3D model data (pitch/yaw velocities and acceleration).
//   Bridge     (BridgeDataStruct *):   Bridge design parameters.
//   Geo        (WindowGeometry *):     Model window screen bounds.
//   Layout     (LayoutConfig *):       Screen layout dimensions.
//   Colors     (ColorTheme *):         UI color palette (data text, selection bar colors).
//   FileInfo   (FileState *):          Current file name and save slot ID.
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void CommandNavigateDown
(
	ProgramState        *State,
	ModelData           *Model,
	BridgeDataStruct    *Bridge,
	WindowGeometry      *Geo,
	LayoutConfig        *Layout,
	ColorTheme          *Colors,
	FileState           *FileInfo
)
{
	// Handle dialog navigation when in dialog mode

	if ( ( State->EditMode == 0 ) && ( State->ActiveWindow == 0 ) )
	{
		// Commit current edit and exit edit mode if on a data item

		if ( ( State->DialogItem >= 1 ) && ( State->DialogItem <= 7 ) )
		{
			ShowDataItem ( State->DialogItem, Bridge, Layout, Colors->DataTextColor );
			State->EditMode = 0;
		} // if

		// Hide selection bar at current position and move to next item

		ShowSelectionBar (State->DialogItem, 0, Layout);

		if ( State->DialogItem == 9 )
		{
		State->DialogItem = 1;
		}
		else if ( State->DialogItem == 3 )
		{
		State->DialogItem = 6;
		}
		else
		{
		State->DialogItem++;
		}

		// Show selection bar at new position

		ShowSelectionBar ( State->DialogItem, Colors->SelectionBarColor, Layout );
		Sound ( 0 );

	} // if

	// Handle 3D model rotation when in perspective mode

	if ( ( State->ActiveWindow == 1 ) && ( State->Perspective == 1 ) )
	{
		// Decrease pitch velocity

		Model->Pv -= Model->Pa;

		// Clamp pitch velocity to minimum

		if ( Model->Pv < -Model->Pm )
		{
		Model->Pv += Model->Pa;
		}

		// Apply cross-axis yaw damping

		if ( Model->Yv < 0 )
		{
			Model->Yv += RotationDamping;
		}
		else
		{
			Model->Yv -= RotationDamping;
		} // if else

	} // if
}

//---------------------------------------------------------------------------
// Function: CommandEditRight
//
// Description:
//
//   Increments the selected bridge parameter or applies rightward yaw
//   rotation in 3D view. Triggered by the Right arrow key.
//
//   In edit mode, increments the parameter for the current dialog item
//   by 1 (Length, Height, Width by 1; NumSegments by 2;
//   NumCrossSuportedSegments by 1) with clamping to valid ranges.
//   In 3D model mode, increases yaw velocity and applies cross-axis
//   pitch damping.
//
// Parameters:
//
//   State      (ProgramState *):       Runtime state (edit mode, cursor, dialog item, active window, perspective).
//   Model      (ModelData *):          3D model data (yaw/pitch velocities and acceleration).
//   Bridge     (BridgeDataStruct *):   Bridge design parameters (dimensions, segment counts).
//   Geo        (WindowGeometry *):     Model window screen bounds.
//   Layout     (LayoutConfig *):       Screen layout dimensions.
//   Colors     (ColorTheme *):         UI color palette.
//   FileInfo   (FileState *):          Current file name and save slot ID.
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void CommandEditRight
(
	ProgramState        *State,
	ModelData           *Model,
	BridgeDataStruct    *Bridge,
	WindowGeometry      *Geo,
	LayoutConfig        *Layout,
	ColorTheme          *Colors,
	FileState           *FileInfo
)
{
	// Increment selected parameter when in edit mode

	if ( State->EditMode == 1 )
	{
		// Flash cursor and play feedback sound

		State->Cursor = 4;
		ShowCursor ( State->DialogItem, Bridge, Layout, State->Cursor );
		Sound      ( 3 );

		// Increment parameter based on active dialog item

		switch ( State->DialogItem )
		{
			// Increment Length (clamp to MaxDimension)

			case 1:
			{
				Bridge->Length++;

				if ( Bridge->Length > MaxDimension )
				{
					Bridge->Length = MaxDimension;
				}
			} break;

			// Increment Height (clamp to MaxDimension)

			case 2:
			{
				Bridge->Height++;

				if ( Bridge->Height > MaxDimension )
				{
					Bridge->Height = MaxDimension;
				}
			} break;

			// Increment Width (clamp to MaxDimension)

			case 3:
			{
				Bridge->Width++;

				if ( Bridge->Width > MaxDimension )
				{
					Bridge->Width = MaxDimension;
				}
			} break;

			// Increment NumSegments by 2 (clamp to MaxSegments)

			case 6:
			{
				Bridge->NumSegments += 2;

				if ( Bridge->NumSegments > MaxSegments )
				{
					Bridge->NumSegments = MaxSegments;
				}
			} break;

			// Increment NumCrossSuportedSegments (clamp to NumSegments)

			case 7:
			{
				Bridge->NumCrossSuportedSegments++;

				if ( Bridge->NumCrossSuportedSegments > MaxSegments )
				{
					Bridge->NumCrossSuportedSegments = MaxSegments;
				}

				if ( Bridge->NumCrossSuportedSegments > Bridge->NumSegments )
				{
					Bridge->NumCrossSuportedSegments = Bridge->NumSegments;
				}
			} break;

		} // switch
	}
	else
	{
	} // if else

	// Handle 3D model rotation when in perspective mode

	if ( ( State->ActiveWindow == 1 ) && ( State->Perspective == 1 ) )
	{
		// Increase yaw velocity

		Model->Yv += Model->Ya;

		// Clamp yaw velocity to maximum

		if ( Model->Yv > Model->Ym )
		{
			Model->Yv -= Model->Ya;
		}

		// Apply cross-axis pitch damping

		if ( Model->Pv < 0 )
		{
			Model->Pv += RotationDamping;
		}
		else
		{
			Model->Pv -= RotationDamping;
		} // if else
	} // if
}

//---------------------------------------------------------------------------
// Function: CommandEditLeft
//
// Description:
//
//   Decrements the selected bridge parameter or applies leftward yaw
//   rotation in 3D view. Triggered by the Left arrow key.
//
//   - In edit mode; (All with clamping to valid ranges) 
//     - Decrements the parameter for the current dialog item, by 1.
//     - Length, Height, Width by 1.
//     - NumSegments by 2.
//     - NumCrossSuportedSegments by 1.
//     - When reducing NumSegments, also clamps NumCrossSuportedSegments 
//       if it exceeds the new value.
//
//   - In 3D model mode, decreases yaw velocity and applies cross-axis pitch 
//     damping.
//
// Parameters:
//
//   State      (ProgramState *):       Runtime state (edit mode, cursor, dialog item, active window, perspective).
//   Model      (ModelData *):          3D model data (yaw/pitch velocities and acceleration).
//   Bridge     (BridgeDataStruct *):   Bridge design parameters (dimensions, segment counts).
//   Geo        (WindowGeometry *):     Model window screen bounds.
//   Layout     (LayoutConfig *):       Screen layout dimensions.
//   Colors     (ColorTheme *):         UI color palette (data text color for cross-support update).
//   FileInfo   (FileState *):          Current file name and save slot ID.
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void CommandEditLeft
(
	ProgramState        *State,
	ModelData           *Model,
	BridgeDataStruct    *Bridge,
	WindowGeometry      *Geo,
	LayoutConfig        *Layout,
	ColorTheme          *Colors,
	FileState           *FileInfo
)
{
	// Decrement selected parameter when in edit mode

	if ( State->EditMode == 1 )
	{
		// Set cursor to visible state

		State->Cursor = 4;

		// Flash cursor and play feedback sound

		ShowCursor ( State->DialogItem, Bridge, Layout, State->Cursor );
		Sound      ( 3 );

		// Decrement parameter based on active dialog item

		switch ( State->DialogItem )
		{
			// Decrement Length (clamp to 0)

			case 1:
			{
				Bridge->Length--;

				if ( Bridge->Length < 0 )
				{
					Bridge->Length = 0;
				}
			} break;

			// Decrement Height (clamp to 0)

			case 2:
			{
				Bridge->Height--;

				if ( Bridge->Height < 0 )
				{
					Bridge->Height = 0;
				}
			} break;

			// Decrement Width (clamp to 0)

			case 3:
			{
				Bridge->Width--;

				if ( Bridge->Width < 0 )
				{
					Bridge->Width = 0;
				}
			} break;

			// Decrement NumSegments by 2 (clamp to MinSegments, adjust cross-supports)

			case 6:
			{
				Bridge->NumSegments -= 2;

				if ( Bridge->NumSegments < MinSegments )
				{
					Bridge->NumSegments = MinSegments;
				}

				if ( Bridge->NumCrossSuportedSegments > Bridge->NumSegments )
				{
					State->Cursor = 4;
					ShowCursor ( State->DialogItem + 1, Bridge, Layout, State->Cursor );
					Bridge->NumCrossSuportedSegments = Bridge->NumSegments;
					ShowDataItem ( State->DialogItem + 1, Bridge, Layout, Colors->DataTextColor );
				}
			} break;

			// Decrement NumCrossSuportedSegments (clamp to MinSegments)

			case 7:
			{
				Bridge->NumCrossSuportedSegments--;

				if ( Bridge->NumCrossSuportedSegments < MinSegments )
				{
					Bridge->NumCrossSuportedSegments = MinSegments;
				}
			} break;

		} // switch

	}
	else
	{
	} // if else

	// Handle 3D model rotation when in perspective mode

	if ( ( State->ActiveWindow == 1 ) && ( State->Perspective == 1 ) )
	{
		// Decrease yaw velocity

		Model->Yv -= Model->Ya;

		// Clamp yaw velocity to minimum

		if ( Model->Yv < -Model->Ym )
		{
			Model->Yv += Model->Ya;
		}

		// Apply cross-axis pitch damping

		if ( Model->Pv < 0 )
		{
			Model->Pv += RotationDamping;
		}
		else
		{
			Model->Pv -= RotationDamping;
		} // if else
	} // if
}

//---------------------------------------------------------------------------
// Function: CommandFastIncrement
//
// Description:
//
//   Increments the selected bridge dimension parameter by 10.
//   Triggered by the Ctrl+Right arrow key.
//
//   Only applies to dimension parameters (Length, Height, Width).
//   Values are clamped to a maximum of MaxDimension.
//
// Parameters:
//
//   State      (ProgramState *):       Runtime state (edit mode, cursor, dialog item).
//   Model      (ModelData *):          3D model data.
//   Bridge     (BridgeDataStruct *):   Bridge design parameters (Length, Height, Width).
//   Geo        (WindowGeometry *):     Model window screen bounds.
//   Layout     (LayoutConfig *):       Screen layout dimensions.
//   Colors     (ColorTheme *):         UI color palette.
//   FileInfo   (FileState *):          Current file name and save slot ID.
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void CommandFastIncrement
(
	ProgramState        *State,
	ModelData           *Model,
	BridgeDataStruct    *Bridge,
	WindowGeometry      *Geo,
	LayoutConfig        *Layout,
	ColorTheme          *Colors,
	FileState           *FileInfo
)
{
	// Fast increment selected dimension when in edit mode

	if ( State->EditMode == 1 )
	{
		// Flash cursor and play feedback sound

		State->Cursor = 4;
		ShowCursor ( State->DialogItem, Bridge, Layout, State->Cursor );
		Sound      ( 4 );

		// Increment dimension by 10 based on active dialog item

		switch ( State->DialogItem )
		{
			// Increment Length by 10 (clamp to MaxDimension)

			case 1:
			{
				Bridge->Length += 10;

				if ( Bridge->Length > MaxDimension )
				{
					Bridge->Length = MaxDimension;
				}
			} break;

			// Increment Height by 10 (clamp to MaxDimension)

			case 2:
			{
				Bridge->Height += 10;

				if ( Bridge->Height > MaxDimension )
				{
					Bridge->Height = MaxDimension;
				}
			} break;

			// Increment Width by 10 (clamp to MaxDimension)

			case 3:
			{
				Bridge->Width += 10;

				if ( Bridge->Width > MaxDimension )
				{
					Bridge->Width = MaxDimension;
				}
			} break;

		} // switch

	}
	else
	{
	} // if else
}

//---------------------------------------------------------------------------
// Function: CommandFastDecrement
//
// Description:
//
//   Decrements the selected bridge dimension parameter by 10.
//   Triggered by the Ctrl+Left arrow key.
//
//   Only applies to dimension parameters (Length, Height, Width).
//   Values are clamped to a minimum of 0.
//
// Parameters:
//
//   State      (ProgramState *):       Runtime state (edit mode, cursor, dialog item).
//   Model      (ModelData *):          3D model data.
//   Bridge     (BridgeDataStruct *):   Bridge design parameters (Length, Height, Width).
//   Geo        (WindowGeometry *):     Model window screen bounds.
//   Layout     (LayoutConfig *):       Screen layout dimensions.
//   Colors     (ColorTheme *):         UI color palette.
//   FileInfo   (FileState *):          Current file name and save slot ID.
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void CommandFastDecrement
(
	ProgramState        *State,
	ModelData           *Model,
	BridgeDataStruct    *Bridge,
	WindowGeometry      *Geo,
	LayoutConfig        *Layout,
	ColorTheme          *Colors,
	FileState           *FileInfo
)
{
	// Fast decrement selected dimension when in edit mode

	if ( State->EditMode == 1 )
	{
		// Set cursor to visible state

		State->Cursor = 4;

		// Flash cursor and play feedback sound

		ShowCursor ( State->DialogItem, Bridge, Layout, State->Cursor );
		Sound      ( 4 );

		// Decrement dimension by 10 based on active dialog item

		switch ( State->DialogItem )
		{
			// Decrement Length by 10 (clamp to 0)

			case 1:
			{
				Bridge->Length -= 10;

				if ( Bridge->Length < 0 )
				{
					Bridge->Length = 0;
				}
			} break;

			// Decrement Height by 10 (clamp to 0)

			case 2:
			{
				Bridge->Height -= 10;

				if ( Bridge->Height < 0 )
				{
					Bridge->Height = 0;
				}
			} break;

			// Decrement Width by 10 (clamp to 0)

			case 3:
			{
				Bridge->Width -= 10;

				if ( Bridge->Width < 0 )
				{
					Bridge->Width = 0;
				}
			} break;

		} //switch
	}
	else
	{
	} // if else
}


