//***************************************************************************
// Name    - DISPLAY.CPP
// Version - 1.4a
// Year    - 2000
// Author  - Rohin A Gosling
//
// Description
//
//   UI display functions for screen layout, dialog box, title bar,
//   statistics, beam data, cursor, and selection bar.
//
//***************************************************************************

#include "DISPLAY.H"

//---------------------------------------------------------------------------
// Function: DisplayModelWindow
//
// Description:
//
//   Calculates and draws the model window on screen.
//
//   Computes the window bounds from the dialog box width and title bar
//   height, stores them in the WindowGeometry struct, and draws a
//   colored border rectangle.
//
// Parameters:
//
//   Geo           (WindowGeometry *):   Model window screen bounds (output).
//   Layout        (LayoutConfig *):     Screen layout dimensions.
//   WindowColor   (int):                Border color for the model window.
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void DisplayModelWindow ( WindowGeometry *Geo, LayoutConfig *Layout, int WindowColor )
{
	// Calculate model window coordinates

	Geo->Gx1 = Layout->DialogBoxWidth + Layout->WindowGap;
	Geo->Gx2 = XRes - 1;
	Geo->Gy1 = Layout->TitleBarHeight + Layout->WindowGap;
	Geo->Gy2 = YRes - 1;

	// Draw model window border

	setcolor  ( WindowColor );
	rectangle ( Geo->Gx1, Geo->Gy1, Geo->Gx2, Geo->Gy2 );
}

//---------------------------------------------------------------------------
// Function: DisplayTitleBar
//
// Description:
//
//   Draws the title bar at the top of the screen.
//
//   Renders the "Bridge Builder" title with a drop shadow, the current
//   file name, and the version string right-aligned. Draws a colored
//   border rectangle around the title bar area.
//
// Parameters:
//
//   Layout        (LayoutConfig *):   Screen layout dimensions (title bar height).
//   WindowColor   (int):              Border color for the title bar.
//   FileInfo      (FileState *):      Current file name and save slot ID.
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void DisplayTitleBar ( LayoutConfig *Layout, int WindowColor, FileState *FileInfo )
{
	// Declare title bar layout variables

	int   y = 2;
	char  Str [ StringSize20 ];

	// Parse version number

	char  MajorStr   [ StringSize10 ];
	char  MinorStr   [ StringSize10 ];
	char  VersionStr [ StringSize20 ];

	// Convert version numbers to strings

	itoa ( MajorVersion, MajorStr, 10 );
	itoa ( MinorVersion, MinorStr, 10 );

	// Build version string

	strcpy ( VersionStr, "(Version " );
	strcat ( VersionStr, MajorStr );
	strcat ( VersionStr, "." );
	strcat ( VersionStr, MinorStr );
	strcat ( VersionStr, "a)" );

	// Draw border

	setcolor  ( WindowColor );
	rectangle ( 0, 0, XRes - 1, Layout->TitleBarHeight );

	// Draw title text

	setcolor  ( 4 );
	outtextxy ( 10 + 2, y + 1, "Bridge Builder" );
	setcolor  ( 12 );
	outtextxy ( 10, y, "Bridge Builder" );

	// Draw file text

	strcpy ( Str, "- " );
	strcat ( Str, FileInfo->FileName );

	setcolor  ( 2 );
	outtextxy ( 100, y + 1, Str );

	// Draw version text

	settextjustify ( RIGHT_TEXT, TOP_TEXT );
	setcolor  ( 1 );
	outtextxy ( XRes - 6 + 2, y + 1, VersionStr );
	setcolor  ( 9 );
	outtextxy ( XRes - 6, y, VersionStr );
}

//---------------------------------------------------------------------------
// Function: DisplayDialogBox
//
// Description:
//
//   Draws the dialog box panel on the left side of the screen.
//
//   Renders the border, section headings (Design parameters,
//   Construction parameters, Statistical data), parameter labels,
//   and their current values. Displays design dimensions, segment
//   counts, configuration mode, split beam toggle, and statistical
//   summary data.
//
// Parameters:
//
//   Bridge   (BridgeDataStruct *):   Bridge design parameters (current values to display).
//   Layout   (LayoutConfig *):       Screen layout dimensions (dialog box width, title bar height).
//   Colors   (ColorTheme *):         UI color palette (window, heading, text, data text colors).
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void DisplayDialogBox ( BridgeDataStruct *Bridge, LayoutConfig *Layout, ColorTheme *Colors )
{
	int  x1                   = 10;                                                            		// Left margin for label text
	int  x2                   = 20;                                                            		// Left margin for parameter labels
	int  x3                   = DialogDataColumnX;                                             		// Left margin for parameter values
	int  y                    = DialogContentOffsetY + Layout->TitleBarHeight + Layout->WindowGap;	// Vertical start position for first row
	int  dy                   = DialogRowHeight;                                               		// Vertical spacing between rows
	int  y2                   = Layout->TitleBarHeight + Layout->WindowGap + y + dy*12 - 5;  		// Vertical position of section divider
	char Str [ StringSize10 ] = "";                                                  				// Temporary string buffer for numeric display

	// Draw border

	setcolor  ( Colors->WindowColor );
	rectangle ( 0, Layout->TitleBarHeight + Layout->WindowGap, Layout->DialogBoxWidth, YRes - 1 );
	line      ( 0, y2, Layout->DialogBoxWidth, y2 );

	// Set text parameters

	settextjustify ( LEFT_TEXT, TOP_TEXT );
	settextstyle   ( 2, 0, 4 );

	// Draw heading text

	setcolor  ( Colors->HeadingTextColor );
	outtextxy ( x1, y + 0*dy,  "Design parameters" );
	outtextxy ( x1, y + 5*dy,  "Construction parameters" );
	outtextxy ( x1, y + 14*dy, "Statistical data" );

	// Draw normal text

	setcolor  ( Colors->TextColor );
	outtextxy ( x2, y + 1*dy, "Bridge Length" );
	outtextxy ( x2, y + 2*dy, "Bridge height" );
	outtextxy ( x2, y + 3*dy, "Bridge width" );

	outtextxy ( x2, y + 6*dy,  "Segments" );
	outtextxy ( x2, y + 7*dy,  "Cross suported segments" );
	outtextxy ( x2, y + 8*dy,  "Configuration" );
	outtextxy ( x2, y + 9*dy,  "Double arch beams" );

	outtextxy ( x2, y + 15*dy, "Number of vertical beams" );
	outtextxy ( x2, y + 16*dy, "Number of diag beams" );
	outtextxy ( x2, y + 17*dy, "Number of arch beams" );
	outtextxy ( x2, y + 18*dy, "Number of cross beams" );
	outtextxy ( x2, y + 19*dy, "Total number of beams" );
	outtextxy ( x2, y + 20*dy, "Number of joints" );
	outtextxy ( x2, y + 21*dy, "Material length" );

	// --------------
	// Draw data text
	// --------------

	// Design data

	setcolor  ( Colors->DataTextColor );
	itoa      ( Bridge->Length, Str, 10 );
	strcat    ( Str, " mm" );
	outtextxy ( x3, y + 1*dy, Str );

	itoa      ( Bridge->Height, Str, 10 );
	strcat    ( Str, " mm" );
	outtextxy ( x3, y + 2*dy, Str );

	itoa      ( Bridge->Width, Str, 10 );
	strcat    ( Str, " mm" );
	outtextxy ( x3, y + 3*dy, Str );

	// Construction data

	itoa      ( Bridge->NumSegments, Str, 10 );
	outtextxy ( x3, y + 6*dy, Str );

	itoa      ( Bridge->NumCrossSuportedSegments, Str, 10 );
	outtextxy ( x3, y + 7*dy, Str );

	switch ( Bridge->Configuration )
	{
		case 0: strcpy ( Str, "Hanging" ); break;
		case 1: strcpy ( Str, "Support" ); break;
		case 2: strcpy ( Str, "Zig-zag" ); break;
	} // switch

	outtextxy ( x3, y + 8*dy, Str );

	if ( Bridge->SplitVertBeams == 0 )
	{
		strcpy ( Str, "No" );
	}
	else
	{
		strcpy ( Str, "Yes" );
	}

	outtextxy ( x3, y + 9*dy, Str );

	// Statistical data

	ShowStatData ( Colors->DataTextColor, Bridge, Layout );

}

//---------------------------------------------------------------------------
// Function: ShowBeamData
//
// Description:
//
//   Displays detailed data for a selected beam in the model window.
//
//   Shows the beam length, gradient, and both vertex coordinates with
//   their individual gradients. Denormalizes beam coordinates from
//   the model's internal scale, computes beam length via the
//   Pythagorean formula, and converts floating-point gradients to
//   display strings.
//
// Parameters:
//
//   Beam            (int):                  Index of the beam to display data for.
//   Model           (ModelData *):          3D model data (beam vertex coordinates).
//   Geo             (WindowGeometry *):     Model window screen bounds (for text positioning).
//   Bridge          (BridgeDataStruct *):   Bridge design parameters (bridge length for gradient calculation).
//   DataTextColor   (int):                  Color for the data text values.
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void ShowBeamData ( int Beam, ModelData *Model, WindowGeometry *Geo, BridgeDataStruct *Bridge, int DataTextColor )
{
	float l  = 0;                   			// Length of an individual beam
	float x1, y1, z1;               			// Vertex 1 coordinates
	float x2, y2, z2;               			// Vertex 2 coordinates
	float dx, dy, dz;               			// Differentials
	float X  = 15;                  			// Test displacement
	float t;                        			// Gradient

	float L = Bridge->Length;					// Length of bridge

	char *Str  = new char [ StringSize20 ];		// Temporary string variable
	char *Temp = new char [ StringSize20 ];		// Temporary string variable

	int  x       = 90;              			// Data text displacement
	char a [ 5 ] = "";              			// Temporary coordinate string
	char b [ 5 ] = "";              			// Temporary coordinate string

	int Dec;                        			// Floating point to string conversion variables
	int Sign;                       			// Floating point to string conversion variables

	// Get denormalized beam vertices

	x1 = Model->Beam [ Beam ].v1.x*CoordScale;
	y1 = Model->Beam [ Beam ].v1.y*CoordScale;
	z1 = Model->Beam [ Beam ].v1.z*CoordScale;

	x2 = Model->Beam [ Beam ].v2.x*CoordScale;
	y2 = Model->Beam [ Beam ].v2.y*CoordScale;
	z2 = Model->Beam [ Beam ].v2.z*CoordScale;

	// Calculate length of beam

	dx = x1 - x2;
	dy = y1 - y2;
	dz = z1 - z2;

	l = sqrt ( dx*dx + dy*dy + dz*dz );

	// ------------
	// Display data
	// ------------

	// Labels

	setcolor  ( 14 );
	outtextxy ( Geo->Gx1 + X, Geo->Gy2 - 90, "Beam length" );
	outtextxy ( Geo->Gx1 + X, Geo->Gy2 - 80, "Gradient" );

	setcolor  ( 12 );
	outtextxy ( Geo->Gx1 + X, Geo->Gy2 - 60, "Vertex 1" );
	outtextxy ( Geo->Gx1 + X, Geo->Gy2 - 50, "Gradient" );

	setcolor  ( 9 );
	outtextxy ( Geo->Gx1 + X, Geo->Gy2 - 30, "Vertex 2" );
	outtextxy ( Geo->Gx1 + X, Geo->Gy2 - 20, "Gradient" );

	// Length and gradient

	setcolor  ( DataTextColor );

	itoa      ( l, Str, 10 );
	strcat    ( Str, " mm" );
	outtextxy ( Geo->Gx1 + x, Geo->Gy2 - 90, Str );


	// calculate gradient of beam

	if ( x1 - x2 == 0 )
	{
		t = 0.0;
	}
	else
	{
		t = ( y1 - y2 ) / ( x1 - x2 );
		t *= CoordScale;
	} // if else

	// Convert float to string

	Str = fcvt ( t, 0, &Dec, &Sign );

	if ( Dec - 4 <= -3 )
	{
		strcpy ( Str, "0.0" );
	}
	else
	{
		if ( Dec - 4 > 0 )
		{

			strcpy ( Temp, Str );
			strcpy ( Temp + Dec - 4, "." );
			strcpy ( Temp + Dec - 4 + 1, Str + Dec - 4 );
			strcpy ( Str, Temp );

			Str [ Dec - 4 + 4 ] = 0;
		}
		else
		{
			Dec = 4 - Dec;

			strcpy ( Temp, "0.0000000000" );

			Temp [ 2 + Dec ] = 0;
			strcat ( Temp, Str );

			Temp [ 2 + Dec + 3 ] = 0;
			strcpy ( Str, Temp );

		} // if else

	} // if else

	if ( Sign != 0 )
	{
		strcpy ( Temp, "-" );
		strcat ( Temp, Str );
		strcpy ( Str, Temp );
	} // if else

	outtextxy ( Geo->Gx1 + x, Geo->Gy2 - 80, Str );

	// Vertex 1 and gradient

	itoa      ( x1, a, 10 );
	itoa      ( y1, b, 10 );
	strcpy    ( Str, "(" );
	strcat    ( Str, a );
	strcat    ( Str, "," );
	strcat    ( Str, b );
	strcat    ( Str, ")" );
	outtextxy ( Geo->Gx1 + x, Geo->Gy2 - 60, Str );

	// Calculate gradient of vertex 1

	if ( y1 > 0 )
	{
		t = -(2.0*x1*CoordScale) / L;
	}
	else
	{
		t = 0.0;
	}

	// Convert float to string

	Str = fcvt ( t, 0, &Dec, &Sign );

	if ( Dec - 4 <= -3 )
	{
		strcpy ( Str, "0.0" );
	}
	else
	{
		if ( Dec - 4 > 0 )
		{
			strcpy ( Temp, Str );
			strcpy ( Temp + Dec - 4, "." );
			strcpy ( Temp + Dec - 4 + 1, Str + Dec - 4 );
			strcpy ( Str, Temp );

			Str [ Dec - 4 + 4 ] = 0;
		}
		else
		{
			Dec = 4 - Dec;

			strcpy ( Temp, "0.0000000000" );

			Temp [ 2 + Dec ] = 0;
			strcat ( Temp, Str );

			Temp [ 2 + Dec + 3 ] = 0;
			strcpy ( Str, Temp );
		} // if else
	} // if else

	if ( Sign != 0 )
	{
		strcpy ( Temp, "-" );
		strcat ( Temp, Str );
		strcpy ( Str, Temp );
	} // if else

	outtextxy ( Geo->Gx1 + x, Geo->Gy2 - 50, Str );

	// Vertex 2 and gradient

	itoa      ( x2, a, 10 );
	itoa      ( y2, b, 10 );
	strcpy    ( Str, "(" );
	strcat    ( Str, a );
	strcat    ( Str, "," );
	strcat    ( Str, b );
	strcat    ( Str, ")" );
	outtextxy ( Geo->Gx1 + x, Geo->Gy2 - 30, Str );

	// Calculate gradient of vertex 2

	if ( y2 > 0 )
	{
		t = -(2.0*x2*CoordScale) / L;
	}
	else
	{
		t = 0.0;
	}

	// Convert float to string

	Str = fcvt ( t, 0, &Dec, &Sign );

	if ( Dec - 4 <= -3 )
	{
		strcpy ( Str, "0.0" );
	}
	else
	{
		if ( Dec - 4 > 0 )
		{
			strcpy ( Temp, Str );
			strcpy ( Temp + Dec - 4, "." );
			strcpy ( Temp + Dec - 4 + 1, Str + Dec - 4 );
			strcpy ( Str, Temp );

			Str [ Dec - 4 + 4 ] = 0;
		}
		else
		{
			Dec = 4 - Dec;

			strcpy ( Temp, "0.0000000000" );

			Temp [ 2 + Dec ] = 0;
			strcat ( Temp, Str );

			Temp [ 2 + Dec + 3 ] = 0;
			strcpy ( Str, Temp );
		} // if else

	} // if else

	if ( Sign != 0 )
	{
		strcpy ( Temp, "-" );
		strcat ( Temp, Str );
		strcpy ( Str, Temp );
	} // if else

	outtextxy ( Geo->Gx1 + x, Geo->Gy2 - 20, Str );

	// Deallocate used memory

	delete [] Str;
	delete [] Temp;
}

//---------------------------------------------------------------------------
// Function: ShowStatData
//
// Description:
//
//   Displays the statistical data section in the dialog box.
//
//   Shows the count of vertical, diagonal, arch, and cross beams, the total 
//   beam count, total joint count, and total material length. 
//
//   Values are drawn at the stat section position in the dialog box using 
//   the specified color.
//
// Parameters:
//
//   Color    (int):                  Text color for the statistical data (0 to erase).
//   Bridge   (BridgeDataStruct *):   Bridge design parameters (beam counts, material length).
//   Layout   (LayoutConfig *):       Screen layout dimensions (for vertical positioning).
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void ShowStatData ( int Color, BridgeDataStruct *Bridge, LayoutConfig *Layout )
{
	int  x                    = DialogDataColumnX;                                            		// Data text x position
	int  y                    = DialogContentOffsetY + Layout->TitleBarHeight + Layout->WindowGap;	// Dialog content top position
	int  dy                   = DialogRowHeight;                                              		// Dialog item row height
	int  y2                   = Layout->TitleBarHeight + Layout->WindowGap + y + dy*12 - 5;			// Dialog box bottom divider position
	int  Temp                 = 0;                                                            		// Temporary calculation variable
	char Str [ StringSize10 ] = "";                                                 				// Temporary string buffer
	int  v, d, a, c;                                                                				// Vertical, diagonal, arch, cross beam counts

	setcolor ( Color );

	// Display beam count statistics

	itoa      ( v = Bridge->NumVertBeams, Str, 10 );
	outtextxy ( x, y + 15*dy, Str );

	itoa      ( d = Bridge->NumDiagBeams, Str, 10 );
	outtextxy ( x, y + 16*dy, Str );

	itoa      ( a = Bridge->NumArchBeams, Str, 10 );
	outtextxy ( x, y + 17*dy, Str );

	itoa      ( c = Bridge->NumCrossBeams, Str, 10 );
	outtextxy ( x, y + 18*dy, Str );

	// Calculate and display total beam count

	Temp = v + d + a + c + 2;

	itoa      ( Temp, Str, 10 );
	outtextxy ( x, y + 19*dy, Str );

	// Calculate and display total joint count

	Temp = 2*(a + 2) - 4;

	itoa      ( Temp, Str, 10 );
	outtextxy ( x, y + 20*dy, Str );

	itoa      ( Bridge->MaterialLength, Str, 10 );
	strcat    ( Str, " mm" );
	outtextxy ( x, y + 21*dy, Str );
}

//---------------------------------------------------------------------------
// Function: DisplayScreen
//
// Description:
//
//   Draws the complete screen layout by composing all UI elements.
//
//   Renders the title bar, dialog box, and model window border in
//   sequence.
//
// Parameters:
//
//   Geo        (WindowGeometry *):     Model window screen bounds (output from DisplayModelWindow).
//   Layout     (LayoutConfig *):       Screen layout dimensions.
//   Colors     (ColorTheme *):         UI color palette.
//   Bridge     (BridgeDataStruct *):   Bridge design parameters (for dialog box display).
//   FileInfo   (FileState *):          Current file name and save slot ID (for title bar).
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void DisplayScreen ( WindowGeometry *Geo, LayoutConfig *Layout, ColorTheme *Colors, BridgeDataStruct *Bridge, FileState *FileInfo )
{

	// show the title bar

	DisplayTitleBar ( Layout, Colors->WindowColor, FileInfo );

	// show the dialog box

	DisplayDialogBox ( Bridge, Layout, Colors );

	// Show the model window

	DisplayModelWindow ( Geo, Layout, Colors->WindowColor );

}

//---------------------------------------------------------------------------
// Function: ShowSelectionBar
//
// Description:
//
//   Draws or erases the selection bar rectangle around a dialog item.
//
//   Computes the vertical position from the item index and row height,
//   then draws a colored rectangle to highlight the selected item.
//   Pass 0 for Color to erase the selection bar.
//
// Parameters:
//
//   Item     (int):              Dialog item index to highlight.
//   Color    (int):              Selection bar color (0 to erase).
//   Layout   (LayoutConfig *):   Screen layout dimensions (dialog box width, title bar height).
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void ShowSelectionBar ( int Item, int Color, LayoutConfig *Layout )
{

	int x  = 5;                                                 				// Selection bar left edge.
	int y  = SelectionBarOffsetY + Layout->TitleBarHeight + Layout->WindowGap;	// Selection bar top offset.
	int y1 = 0;                                                 				// Selection bar upper bound.
	int y2 = 0;                                                 				// Selection bar lower bound.
	int dy = DialogRowHeight;                                   				// Item row height.

	setcolor ( Color );

	y1 = y + dy*Item;
	y2 = y1 + DialogRowHeight;

	rectangle ( x, y1, Layout->DialogBoxWidth - 4, y2 );
}

//---------------------------------------------------------------------------
// Function: ShowCursor
//
// Description:
//
//   Displays the blinking edit cursor for the active dialog item.
//
//   Selects a color based on the cursor blink state (cycling from
//   bright white through blues to black), retrieves the current
//   parameter value for the active dialog item, and redraws the
//   value text in the cursor color to create an animated blink effect.
//
// Parameters:
//
//   DialogItem   (int):                  Active dialog item index.
//   Bridge       (BridgeDataStruct *):   Bridge design parameters (current values).
//   Layout       (LayoutConfig *):       Screen layout dimensions (for text positioning).
//   Cursor       (int):                  Cursor blink state (0-4, controls display color).
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void ShowCursor ( int DialogItem, BridgeDataStruct *Bridge, LayoutConfig *Layout, int Cursor )
{

	int  x                    = DialogDataColumnX;                                            		// Data text x position
	int  y                    = DialogContentOffsetY + Layout->TitleBarHeight + Layout->WindowGap;	// Dialog content top position
	int  dy                   = DialogRowHeight;                                              		// Dialog item row height
	int  y2                   = Layout->TitleBarHeight + Layout->WindowGap + y + dy*12 - 5;			// Dialog box bottom divider position
	int  Data                 = 0;                                                            		// Current parameter value
	char Str [ StringSize10 ] = "";                                                 				// Temporary string buffer
	int  Color;                                                                     				// Cursor display color

	// Select cursor color based on blink state

	switch ( Cursor )
	{
		case 0: Color = 15; break;
		case 1: Color = 11; break;
		case 2: Color = 9;  break;
		case 3: Color = 1;  break;
		case 4: Color = 0;  break;
	} // switch

	// Set cursor color based on blink state

	setcolor ( Color );

	switch ( DialogItem )
	{
		case 1:
		{
			Data = Bridge->Length;
		} break;

		case 2:
		{
			Data = Bridge->Height;
		} break;

		case 3:
		{
			Data = Bridge->Width;
		} break;

		case 6:
		{
			Data = Bridge->NumSegments;
		} break;

		case 7:
		{
			Data = Bridge->NumCrossSuportedSegments;
		} break;
	} // switch

	// Convert data value to string

	itoa ( Data, Str, 10 );

	// Append unit suffix for dimension parameters

	if ( DialogItem <= 3 ) strcat ( Str, " mm" );

	// Display data value at the current dialog item position

	outtextxy ( x, y + DialogItem*dy, Str );
}

//---------------------------------------------------------------------------
// Function: ShowDataItem
//
// Description:
//
//   Displays the current value of a dialog data item in the dialog box.
//
//   Retrieves the parameter value for the specified dialog item,
//   converts it to a string, appends the unit suffix for dimension
//   parameters, and draws it at the corresponding position in the
//   dialog box.
//
// Parameters:
//
//   DialogItem      (int):                  Dialog item index to display.
//   Bridge          (BridgeDataStruct *):   Bridge design parameters (current values).
//   Layout          (LayoutConfig *):       Screen layout dimensions (for text positioning).
//   DataTextColor   (int):                  Color for the data text value.
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void ShowDataItem ( int DialogItem, BridgeDataStruct *Bridge, LayoutConfig *Layout, int DataTextColor )
{
	int  x                    = DialogDataColumnX;                                            		// Data text x position
	int  y                    = DialogContentOffsetY + Layout->TitleBarHeight + Layout->WindowGap;	// Dialog content top position
	int  dy                   = DialogRowHeight;                                              		// Dialog item row height
	int  y2                   = Layout->TitleBarHeight + Layout->WindowGap + y + dy*12 - 5;			// Dialog box bottom divider position
	int  Data                 = 0;                                                            		// Current parameter value
	char Str [ StringSize10 ] = "";                                                 				// Temporary string buffer
	

	// Get data value for current dialog item

	switch ( DialogItem )
	{
		case 1:
		{
			Data = Bridge->Length;
		} break;

		case 2:
		{
			Data = Bridge->Height;
		} break;

		case 3:
		{
			Data = Bridge->Width;
		} break;

		case 6:
		{
			Data = Bridge->NumSegments;
		} break;

		case 7:
		{
			Data = Bridge->NumCrossSuportedSegments;
		} break;
	} // switch

	// Set text color and convert data value to string

	setcolor ( DataTextColor );
	itoa     ( Data, Str, 10 );

	// Append unit suffix for dimension parameters

	if ( DialogItem <= 3 ) strcat ( Str, " mm" );

	// Display data value at the current dialog item position

	outtextxy ( x, y + DialogItem*dy, Str );
}
