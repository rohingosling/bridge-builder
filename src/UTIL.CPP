//***************************************************************************
// Name    - UTIL.CPP
// Version - 1.4a
// Year    - 2000
// Author  - Rohin A Gosling
//
// Description
//
//   Utility functions for sound effects and UI animation feedback.
//
//***************************************************************************

#include "UTIL.H"

//---------------------------------------------------------------------------
// Function: Sound
//
// Description:
//
//   Plays a sound effect using the PC speaker.
//
//   Selects from five predefined sound patterns based on the effect
//   index: descending sweep (0), alternating high-low (1), rising
//   chirp (2), short click (3), and high click (4).
//
// Parameters:
//
//   Sound   (int):   Sound effect index (0-4).
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void Sound ( int Sound )
{
	// Select and play a sound effect by index

	switch ( Sound )
	{
		// Descending sweep: 12 kHz -> 10 kHz -> 8 kHz -> 6 kHz

		case 0:
		{
			sound   ( 12000 ); delay ( 2 ); sound ( 10000 ); delay ( 2 );
			sound   ( 8000 );  delay ( 2 ); sound ( 6000 );  delay ( 2 );
			nosound ();
		} break;

		// Alternating high-low: two cycles of 15 kHz / 6 kHz

		case 1:
		{
			sound   ( 15000 ); delay ( 5 ); sound ( 6000 ); delay ( 5 );
			sound   ( 15000 ); delay ( 5 ); sound ( 6000 ); delay ( 5 );
			nosound ();
		} break;

		// Rising chirp: 500 Hz -> 10 kHz -> 10 kHz -> 500 Hz

		case 2:
		{
			sound   ( 500 );   delay ( 2 ); sound ( 10000 ); delay ( 2 );
			sound   ( 10000 ); delay ( 2 ); sound ( 500 );   delay ( 2 );
			nosound ();
		} break;

		// Short click: brief 4 kHz pulse

		case 3:
		{
			sound ( 4000 ); delay ( 1 ); nosound ();
		} break;

		// High click: brief 6 kHz pulse

		case 4:
		{
			sound ( 6000 ); delay ( 1 ); nosound ();
		} break;

	} // switch
}

//---------------------------------------------------------------------------
// Function: FlashTitleBar
//
// Description:
//
//   Animates a flash effect on the title bar border.
//
//   Draws an inset rectangle inside the title bar, cycling the border
//   color from bright cyan through blue to black over four steps,
//   creating a brief highlight flash for visual feedback.
//
// Parameters:
//
//   Speed    (int):              Total animation duration in milliseconds.
//   Layout   (LayoutConfig *):   Screen layout dimensions (title bar height).
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void FlashTitleBar ( int Speed, LayoutConfig *Layout )
{
	int x1 = 2;                      		// Left edge of title bar
	int y1 = 2;                      		// Top edge of title bar
	int x2 = XRes - 3;               		// Right edge of title bar
	int y2 = Layout->TitleBarHeight - 2;	// Bottom edge of title bar
	int t  = Speed / 4;              		// Animation step delay

	// Animate flash effect by cycling border color from bright to dark

	setcolor  ( 11 );
	rectangle ( x1, y1, x2, y2 );
	delay     ( t );

	setcolor  ( 9 );
	rectangle ( x1, y1, x2, y2 );
	delay     ( t );

	setcolor  ( 1 );
	rectangle ( x1, y1, x2, y2 );
	delay     ( t );

	setcolor  ( 0 );
	rectangle ( x1, y1, x2, y2 );
}

//---------------------------------------------------------------------------
// Function: FlashSelectionBar
//
// Description:
//
//   Animates a flash effect on the selection bar border.
//
//   Computes the selection bar position from the dialog item index,
//   then cycles the border color from bright white through cyan and
//   blue to dark blue over four steps, creating a brief highlight
//   flash for visual feedback.
//
// Parameters:
//
//   Speed        (int):              Total animation duration in milliseconds.
//   Layout       (LayoutConfig *):   Screen layout dimensions (title bar height, dialog box width).
//   DialogItem   (int):              Active dialog item index (determines bar position).
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void FlashSelectionBar ( int Speed, LayoutConfig *Layout, int DialogItem )
{
	int x1 = 5;                                                 				// Left edge of selection bar
	int x2 = Layout->DialogBoxWidth - 4;                       					// Right edge of selection bar
	int y  = SelectionBarOffsetY + Layout->TitleBarHeight + Layout->WindowGap;	// Base vertical offset for menu items
	int y1 = 0;                                                 				// Top edge of selection bar
	int y2 = 0;                                                 				// Bottom edge of selection bar
	int dy = DialogRowHeight;                                   				// Vertical spacing between menu items
	int t  = Speed / 4;                                         				// Animation step delay

	// Compute selection bar position from current dialog item index

	y1 = y + dy*DialogItem;
	y2 = y1 + DialogRowHeight;

	// Animate flash effect by cycling border color from bright to dark

	setcolor  ( 15 );
	rectangle ( x1, y1, x2, y2 );
	delay     ( t );

	setcolor  ( 11 );
	rectangle ( x1, y1, x2, y2 );
	delay     ( t );

	setcolor  ( 9 );
	rectangle ( x1, y1, x2, y2 );
	delay     ( t );

	setcolor  ( 1 );
	rectangle ( x1, y1, x2, y2 );
}

//---------------------------------------------------------------------------
// Function: FlashWindow
//
// Description:
//
//   Animates a flash effect on the border of the active window.
//
//   Selects the window rectangle (dialog box or model window) based
//   on ActiveWindow, then cycles the border color from bright cyan
//   through blue to black over four steps, creating a brief highlight
//   flash for visual feedback.
//
// Parameters:
//
//   Speed          (int):                Total animation duration in milliseconds.
//   Layout         (LayoutConfig *):     Screen layout dimensions (dialog box width, title bar height).
//   Geo            (WindowGeometry *):   Model window screen bounds.
//   ActiveWindow   (int):                Active window index (0 = dialog box, 1 = model window).
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void FlashWindow ( int Speed, LayoutConfig *Layout, WindowGeometry *Geo, int ActiveWindow )
{
	int x1, y1, x2, y2;     // Window rectangle coordinates
	int t = Speed / 3;      // Animation step delay

	// Set active display page for drawing

	setactivepage ( 0 );
	setactivepage ( 0 );

	// Set window dimensions

	switch ( ActiveWindow )
	{
		case 0:   // Dialog box
		{
			x1 = 2;
			y1 = Layout->TitleBarHeight + Layout->WindowGap + 2;
			x2 = Layout->DialogBoxWidth - 2;
			y2 = YRes - 180;
		} break;

		case 1:   // Model window
		{
			x1 = Geo->Gx1 + 2;
			y1 = Geo->Gy1 + 2;
			x2 = Geo->Gx2 - 2;
			y2 = Geo->Gy2 - 2;
		} break;

	} // switch

	// Animate flash effect by cycling border color from bright to dark

	setcolor  ( 11 );
	rectangle ( x1, y1, x2, y2 );
	delay     ( t );

	setcolor  ( 9 );
	rectangle ( x1, y1, x2, y2 );
	delay     ( t );

	setcolor  ( 1 );
	rectangle ( x1, y1, x2, y2 );
	delay     ( t );

	setcolor  ( 0 );
	rectangle ( x1, y1, x2, y2 );
	nosound   ();
}
