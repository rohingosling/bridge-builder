//***************************************************************************
// Name    - BTYPES.H
// Version - 1.4a
// Year    - 2000
// Author  - Rohin A Gosling
//
// Description
//
//   Shared type definitions, constants, and common system includes for the
//   Bridge Builder application.
//
//***************************************************************************

#ifndef BTYPES_H
#define BTYPES_H

#include <graphics.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <dos.h>

//---------------------------------------------------------------------------
// constants
//---------------------------------------------------------------------------

const float XRes         = 640;     		// Screen x resolution
const float YRes         = 350;     		// Screen y resolution
const int   MaxBeams     = 1000;			// Maximum number of beams used for construction
const int   MajorVersion = 1;				// Major version number
const int   MinorVersion = 4;				// Minor version number
const float DuelSupportD = 42;	   			// Duel support distance
const float DuelSupportL = 10;	   			// Duel support length

//---------------------------------------------------------------------------
// Coordinate scaling
//---------------------------------------------------------------------------

const float CoordScale = 10000;				// Internal coordinate precision scale factor

//---------------------------------------------------------------------------
// Rendering constants
//---------------------------------------------------------------------------

const float AspectRatioY      = 0.72;   	// VGA 640x350 pixel aspect ratio (Y)
const float Scale3D           = 0.8;    	// Uniform scale for 3D perspective view
const float Scale2D           = 0.9;    	// Uniform scale for 2D orthographic view
const float HeightScaleFactor = 1.5;    	// Height-dominant scale denominator

//---------------------------------------------------------------------------
// Bridge parameter limits
//---------------------------------------------------------------------------

const int   MaxDimension = 9999;   			// Maximum bridge dimension (mm)
const int   MaxSegments  = 20;     			// Maximum number of arch segments
const int   MinSegments  = 2;      			// Minimum number of arch segments

//---------------------------------------------------------------------------
// 3D rotation physics
//---------------------------------------------------------------------------

const float RotationDamping = 0.001;  		// Cross-axis angular deceleration

//---------------------------------------------------------------------------
// Dialog box layout
//---------------------------------------------------------------------------

const int   DialogRowHeight      = 11;     	// Vertical spacing between dialog items (px)
const int   DialogDataColumnX    = 170;		// X position of data values in dialog box
const int   DialogContentOffsetY = 5;     	// Top margin above first dialog row (px)
const int   SelectionBarOffsetY  = 6;      	// Top margin for selection bar (px)

//---------------------------------------------------------------------------
// File format
//---------------------------------------------------------------------------

const int   FileIdSize     = 20;     		// Size of file ID string in .BRG files
const int   FileDataOffset = 24;     		// Byte offset to bridge data in .BRG files

//---------------------------------------------------------------------------
// String buffer sizes
//---------------------------------------------------------------------------

const int   StringSize10   = 10;     		// Short string buffer (numeric conversions)
const int   StringSize20   = 20;     		// Long string buffer (composite display strings)


//---------------------------------------------------------------------------
// Data types
//---------------------------------------------------------------------------

enum Boolean { False, True };

// 3D Vector

struct Vector
{
	float x, y, z;
};

// Bridge data

struct BridgeDataStruct
{

	float   Height;                     // Bridge height
	float   Length;                     // Bridge length
	float   Width;                      // Bridge width
	int     MaterialLength;             // Total construction material length
	float   JointOverlap;               // Joint overlap distance
	int     NumSegments;                // Number of arch segments
	int     NumCrossSuportedSegments;   // Number of cross supported segments
	int     Configuration;              // 0=Hanging, 1=Supporting, 3=Alternating
	Boolean SplitVertBeams;             // Double vertical beams flag
	Boolean SplitDiagBeams;             // Double diagonal beams flag
	int     NumVertBeams;               // Total vertical beams
	int     NumDiagBeams;               // Total diagonal beams
	int     NumCrossBeams;              // Total cross support beams
	int     NumArchBeams;               // Total arch beams
	int     NumJoints;                  // Total joints

};

// Beam Data

struct BeamData
{
	Vector v1;           				// Vertex 1
	Vector v2;           				// vertex 2
	int    Color;        				// Beam color
	int    Tag;          				// General purpose variable
};

// 3D Model data

struct ModelData
{
	int   NumBeams;      				// Number of beams used in bridge
	float f;             				// Camera focal length
	float Yaw;           				// Yaw rotation
	float Pitch;         				// Pitch rotation
	float Yv;            				// Yaw angular velocity
	float Pv;            				// Pitch angular velocity
	float Ya;            				// Yaw acceleration
	float Pa;            				// Pitch acceleration
	float Ym;            				// Minimum absolute yaw velocity
	float Pm;            				// Minimum absolute pitch velocity

	BeamData Beam [ MaxBeams ];   		// Array of beams
};

// Window geometry

struct WindowGeometry
{
	float Gx1;                        	// Model window x1
	float Gy1;                          // Model window y1
	float Gx2;                          // Model window x2
	float Gy2;                          // Model window y2
};

// Screen layout configuration

struct LayoutConfig
{
	int TitleBarHeight;                 // Title bar height
	int DialogBoxWidth;                 // Width of user dialog box
	int WindowGap;                      // Distance between window borders
};

// UI color theme

struct ColorTheme
{
	int WindowColor;                    // The color of the window border frames
	int ActiveWindowColor;              // The color of the active window
	int HeadingTextColor;               // Heading text color
	int TextColor;                      // Normal text color
	int ActiveHeadingTextColor;         // Active heading text color
	int ActiveTextColor;                // Active normal text color
	int DataTextColor;                  // Data text color
	int SelectionBarColor;              // Selection bar color
	int ActiveDataTextColor;            // Active data text color
	int Side1VertBeamColor;             // Side 1 vertical beam color
	int Side1DiagBeamColor;             // Side 1 diagonal beam color
	int Side1ArchColor;                 // Side 1 arch beam color
	int Side2VertBeamColor;             // Side 2 vertical beam color
	int Side2DiagBeamColor;             // Side 2 diagonal beam color
	int Side2ArchColor;                 // Side 2 arch beam color
	int BottomCrossBeamColor;           // Bottom cross beam color
	int TopCrossBeamColor;              // Top cross beam color
	int ModelColor1;                    // Primary model display color
	int ModelColor2;                    // Secondary model display color
};

// Application state

struct ProgramState
{
	int ActiveWindow;                 	// 0=dialog box, 1=model window
	int ActivePage;                     // Active display page
	int VisualPage;                     // Active visual page
	int DialogItem;                     // Item ID for the dialog items
	int Perspective;                    // 0 = 2D,  1 = 3D
	int EditMode;                       // 0 = Off, 1 = On
	int Cursor;                         // Cursor flag
	int CursorCount;                    // Cursor timer
	int SelectedBeam;                   // User selected beam
};

// File state

struct FileState
{
	char FileName [ 14 ];				// Bridge data file name
	int  FileID;                        // File ID
};

#endif
