//***************************************************************************
// Name    - GEOMETRY.CPP
// Version - 1.4a
// Year    - 2000
// Author  - Rohin A Gosling
//
// Description
//
//   Bridge generation functions for parametric bridge model construction.
//   Includes the main bridge geometry generator, coordinate normalization,
//   and material length calculation.
//
//***************************************************************************

#include "GEOMETRY.H"
#include "DISPLAY.H"

//---------------------------------------------------------------------------
// Function: SetBeam
//
// Description:
//
//   Sets all fields of a BeamData struct in a single call. Assigns the
//   two vertex positions, beam color, and beam tag.
//
// Parameters:
//
//   Beam    (BeamData *):   Beam to initialize.
//   x1      (float):       Vertex 1 x coordinate.
//   y1      (float):       Vertex 1 y coordinate.
//   z1      (float):       Vertex 1 z coordinate.
//   x2      (float):       Vertex 2 x coordinate.
//   y2      (float):       Vertex 2 y coordinate.
//   z2      (float):       Vertex 2 z coordinate.
//   Color   (int):         Beam color.
//   Tag     (int):         Beam type tag.
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void SetBeam
(
	BeamData *Beam,
	float     x1,
	float     y1,
	float     z1,
	float     x2,
	float     y2,
	float     z2,
	int       Color,
	int       Tag
)
{
	Beam->v1.x = x1;
	Beam->v1.y = y1;
	Beam->v1.z = z1;

	Beam->v2.x = x2;
	Beam->v2.y = y2;
	Beam->v2.z = z2;

	Beam->Color = Color;
	Beam->Tag   = Tag;
}

//---------------------------------------------------------------------------
// Function: NormalizeCoords
//
// Description:
//
//   Normalizes bridge model coordinates by dividing all beam vertex
//   components by 10,000.
//
//   The bridge generator works in a scaled coordinate space (multiplied
//   by 10,000 for integer precision). This function converts the model
//   back to normalized coordinates suitable for rendering.
//
// Parameters:
//
//   Model   (ModelData *):   3D model data (beam vertex coordinates to normalize).
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void NormalizeCoords ( ModelData *Model )
{
	int N = Model->NumBeams;

	for (int n = 0; n < N; n++ )
	{
		Model->Beam [ n ].v1.x /= CoordScale;
		Model->Beam [ n ].v1.y /= CoordScale;
		Model->Beam [ n ].v1.z /= CoordScale;

		Model->Beam [ n ].v2.x /= CoordScale;
		Model->Beam [ n ].v2.y /= CoordScale;
		Model->Beam [ n ].v2.z /= CoordScale;
	}
}


//---------------------------------------------------------------------------
// Function: GenerateBridge
//
// Description:
//
//   Generates the complete parametric bridge model from the current
//   design parameters.
//
//   Constructs vertical beams, parabolic arch beams, cross support beams,
//   road bed beams, and diagonal beams (hanging, support, or zig-zag
//   configuration) for both sides of the bridge. Resets and recalculates
//   statistical data, then normalizes model coordinates for rendering.
//
//   Beam tag values:
//     1  = Side 1 diagonal beams
//     2  = Side 2 diagonal beams
//     3  = Side 1 arch beams
//     4  = Side 2 arch beams
//     5  = Side 1 vertical beams
//     6  = Side 2 vertical beams
//     9  = Cross support beams
//     10 = Road bed beams
//
// Parameters:
//
//   Model    (ModelData *):          3D model data (beam array, beam count).
//   Bridge   (BridgeDataStruct *):   Bridge design parameters (dimensions, segments, configuration).
//   Colors   (ColorTheme *):         UI color palette (beam colors by structural role).
//   Layout   (LayoutConfig *):       Screen layout dimensions (for stat display update).
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void GenerateBridge ( ModelData *Model, BridgeDataStruct *Bridge, ColorTheme *Colors, LayoutConfig *Layout )
{
	int   BeamCount = 0;   // Beam counter variable
	int   zz        = 0;   // Zig zag direction
	float S         = 0;   // segment counter

	// Get design and construction parameters

	float l = Bridge->Length;                         // Bridge length
	float h = Bridge->Height;                         // Bridge height
	float w = Bridge->Width;                          // Bridge width

	float j   = Bridge->JointOverlap;                 // Joint overlap distance
	float s   = Bridge->NumSegments;                  // Number of arch segments
	float s2  = s / 2.0;                               // Half the number of segments
	float cs  = Bridge->NumCrossSuportedSegments;     // Number of cross supported segments
	float cs2 = cs / 2.0;                              // Half the number of cross supported segments

	int     Config    = Bridge->Configuration;        // Bridge configuration type
	Boolean SplitVert = Bridge->SplitVertBeams;       // Split vertical beams flag
	Boolean SplitDiag = Bridge->SplitDiagBeams;       // Split diagonal beams flag

	int Col  = 0;                                      // Beam color
	int Col1 = 0;                                      // Beam color
	int Col2 = 0;                                      // Beam color
	int Tag  = 0;                                      // Beam type flag
	int Tag1 = 0;                                      // Beam type flag
	int Tag2 = 0;                                      // Beam type flag

	float xd  = l / s;                                 // Segment distance
	float cxd = l / cs;                                // Cross suported segment distance
	float y   = 0;                                     // Hight of arch at a particular value of x
	float y0  = 0;                                     // Hight of arch at a particular value of x
	float y1  = 0;                                     // Hight of arch at a particular value of x
	float x   = 0;                                     // Segment sample point
	float x0  = 0;                                     // Segment sample point
	float x1  = 0;                                     // Segment sample point
	float a   = 0;                                     // Coefficient of quadratic function describing the arch
	float l2  = l / 2;                                 // Half the length of the bridge

	// Reset statistical data

	ShowStatData ( 0, Bridge, Layout );

	Bridge->NumVertBeams   = 0;
	Bridge->NumDiagBeams   = 0;
	Bridge->NumArchBeams   = 0;
	Bridge->NumCrossBeams  = 0;
	Bridge->MaterialLength = 0;


	// --------------------------
	// Generate bridge model data
	// --------------------------

	BeamCount = 0;


	// ---------------
	// Generate side 1
	// ---------------

	Col = Colors->Side1VertBeamColor;
	Tag = 5;                      // Side 1

	for ( S = s2 - 1; S > 0; S-- )
	{
		// Calculate parabolic arch point

		a = -h / ( l2*l2 );
		x =  S*xd;
		y =  a*x*x + h;

		y += j;

		// Set left and right beam vertices

		SetBeam ( &Model->Beam [ BeamCount     ],  x, -j, w / 2,  x, y, w / 2, Col, Tag );
		SetBeam ( &Model->Beam [ BeamCount + 1 ], -x, -j, w / 2, -x, y, w / 2, Col, Tag );

		// Next beam

		BeamCount += 2;
		Bridge->NumVertBeams += 2;

	} // for

	// Set side 1 center beam

	SetBeam ( &Model->Beam [ BeamCount ], 0, -j, w / 2, 0, h + j, w / 2, Col, Tag );

	BeamCount++;
	Bridge->NumVertBeams++;


	// ---------------
	// Generate side 2
	// ---------------

	Col = Colors->Side1VertBeamColor;
	Tag = 6;                   // Side 2

	for ( S = s2 - 1; S > 0; S-- )
	{
		// Calculate parabolic arch point

		a = -h / ( l2*l2 );
		x = S*xd;
		y = a*x*x + h;

		y += j;

		// Set left and right beam vertices

		SetBeam ( &Model->Beam [ BeamCount     ],  x, -j, -w / 2,  x, y, -w / 2, Col, Tag );
		SetBeam ( &Model->Beam [ BeamCount + 1 ], -x, -j, -w / 2, -x, y, -w / 2, Col, Tag );

		// Next beam

		BeamCount += 2;
		Bridge->NumVertBeams += 2;

	} // for

	// Set side 2 center beam

	SetBeam ( &Model->Beam [ BeamCount ], 0, -j, -w / 2, 0, h + j, -w / 2, Col, Tag );

	BeamCount++;
	Bridge->NumVertBeams++;


	// ---------------
	// Generate Arches
	// ---------------

	Col1 = Colors->Side1VertBeamColor;
	Col2 = Colors->Side2VertBeamColor;
	Tag1 = 3;                    // Side 1
	Tag2 = 4;                    // Side 2

	// Generate arch beams

	for ( S = s2; S > 0; S-- )
	{
		// Calculate parabolic arch points

		a  = -h / ( l2*l2 );
		x0 =  S*xd;
		y0 =  a*x0*x0 + h;

		a = -h / ( l2*l2 );
		x1 = ( S - 1 )*xd;
		y1 = a*x1*x1 + h;

		// Set arch beams

		SetBeam ( &Model->Beam [ BeamCount     ],  x0, y0,  w / 2,  x1, y1,  w / 2, Col1, Tag1 );
		SetBeam ( &Model->Beam [ BeamCount + 1 ], -x0, y0,  w / 2, -x1, y1,  w / 2, Col1, Tag1 );
		SetBeam ( &Model->Beam [ BeamCount + 2 ],  x0, y0, -w / 2,  x1, y1, -w / 2, Col2, Tag2 );
		SetBeam ( &Model->Beam [ BeamCount + 3 ], -x0, y0, -w / 2, -x1, y1, -w / 2, Col2, Tag2 );

		BeamCount += 4;
		Bridge->NumArchBeams += 4;

	} // for


	// ----------------------------
	// Generate cross support beams
	// ----------------------------

	Col1 = Colors->BottomCrossBeamColor;
	Col2 = Colors->TopCrossBeamColor;
	Tag  = 9;                     // Cross support beam

	for ( S = cs2 - 1; S > 0; S-- )
	{
		// Calculate parabolic arch point

		a = -h / ( l2*l2 );
		x =  S*cxd;
		y =  a*x*x + h;

		// Set top and bottom cross support beams

		SetBeam ( &Model->Beam [ BeamCount     ],  x, 0, w / 2,  x, 0, -w / 2, Col, Tag );
		SetBeam ( &Model->Beam [ BeamCount + 1 ],  x, y, w / 2,  x, y, -w / 2, Col, Tag );
		SetBeam ( &Model->Beam [ BeamCount + 2 ], -x, 0, w / 2, -x, 0, -w / 2, Col, Tag );
		SetBeam ( &Model->Beam [ BeamCount + 3 ], -x, y, w / 2, -x, y, -w / 2, Col, Tag );

		// Next beam

		BeamCount += 4;
		Bridge->NumCrossBeams += 4;

	} // for

	// Set center cross support beams

	SetBeam ( &Model->Beam [ BeamCount     ], 0, 0, w / 2, 0, 0, -w / 2, Col, Tag );
	SetBeam ( &Model->Beam [ BeamCount + 1 ], 0, h, w / 2, 0, h, -w / 2, Col, Tag );

	BeamCount += 2;
	Bridge->NumCrossBeams += 2;

	// Set end cross support beams

	SetBeam ( &Model->Beam [ BeamCount     ],  l / 2, 0, w / 2,  l / 2, 0, -w / 2, Col, Tag );
	SetBeam ( &Model->Beam [ BeamCount + 1 ], -l / 2, 0, w / 2, -l / 2, 0, -w / 2, Col, Tag );

	BeamCount += 2;
	Bridge->NumCrossBeams += 2;


	// -----------------------
	// Generate Road bed Beams
	// -----------------------

	Col1 = Colors->Side1VertBeamColor;
	Col2 = Colors->Side2VertBeamColor;
	Tag  = 3;   // Cross support beam

	SetBeam ( &Model->Beam [ BeamCount     ], -l / 2, 0,  w / 2, l / 2, 0,  w / 2, Col1, Tag );
	SetBeam ( &Model->Beam [ BeamCount + 1 ], -l / 2, 0, -w / 2, l / 2, 0, -w / 2, Col2, Tag );

	BeamCount += 2;


	// -----------------------
	// Generate diagonal beams
	// -----------------------

	Col1 = Colors->Side1DiagBeamColor;
	Col2 = Colors->Side2DiagBeamColor;
	Tag1 = 1;                     // Side 1 diagonal support beam
	Tag2 = 2;                     // Side 2 diagonal support beam

	if ( Bridge->Configuration == 0 )
	{
		// ------------------------------
		// Generate hanging configuration
		// ------------------------------

		for ( S = s2 - 1; S > 0; S-- )
		{

			// Calculate parabolic arch points

			a  = -h / ( l2*l2 );
			x0 =  S*xd;
			y0 =  a*x0*x0 + h;

			a  = -h / ( l2*l2 );
			x1 = ( S - 1 )*xd;
			y1 =  a*x1*x1 + h;

			// Set diagonal beams

			SetBeam ( &Model->Beam [ BeamCount     ],  x1, 0,  w / 2,  x0, y0,  w / 2, Col1, Tag1 );
			SetBeam ( &Model->Beam [ BeamCount + 1 ], -x1, 0,  w / 2, -x0, y0,  w / 2, Col1, Tag1 );
			SetBeam ( &Model->Beam [ BeamCount + 2 ],  x1, 0, -w / 2,  x0, y0, -w / 2, Col2, Tag2 );
			SetBeam ( &Model->Beam [ BeamCount + 3 ], -x1, 0, -w / 2, -x0, y0, -w / 2, Col2, Tag2 );

			BeamCount += 4;
			Bridge->NumDiagBeams += 4;

		} // for


	}
	else if ( Bridge->Configuration == 1 )
	{
		// ------------------------------
		// Generate support configuration
		// ------------------------------

		for ( S = s2 - 1; S > 0; S-- )
		{

			// Calculate parabolic arch points

			a  = -h / ( l2*l2 );
			x0 =  S*xd;
			y0 =  a*x0*x0 + h;

			a  = -h / ( l2*l2 );
			x1 = ( S - 1 )*xd;
			y1 =  a*x1*x1 + h;

			// Set diagonal beams

			SetBeam ( &Model->Beam [ BeamCount     ],  x0, 0,  w / 2,  x1, y1,  w / 2, Col1, Tag1 );
			SetBeam ( &Model->Beam [ BeamCount + 1 ], -x0, 0,  w / 2, -x1, y1,  w / 2, Col1, Tag1 );
			SetBeam ( &Model->Beam [ BeamCount + 2 ],  x0, 0, -w / 2,  x1, y1, -w / 2, Col2, Tag2 );
			SetBeam ( &Model->Beam [ BeamCount + 3 ], -x0, 0, -w / 2, -x1, y1, -w / 2, Col2, Tag2 );

			BeamCount += 4;
			Bridge->NumDiagBeams += 4;

		} // for
	}
	else
	{
		// ------------------------------
		// Generate zig-zag configuration
		// ------------------------------

		zz = 0;

		for ( S = s2 - 1; S > 0; S-- )
		{
			// Calculate parabolic arch points

			a  = -h / ( l2*l2 );
			x0 =  S*xd;
			y0 =  a*x0*x0 + h;

			a  = -h / ( l2*l2 );
			x1 = ( S - 1 )*xd;
			y1 =  a*x1*x1 + h;

			// Set diagonal beams

			if ( zz == 0 )
			{
				SetBeam ( &Model->Beam [ BeamCount     ],  x1, 0,  w / 2,  x0, y0,  w / 2, Col1, Tag1 );
				SetBeam ( &Model->Beam [ BeamCount + 1 ], -x1, 0,  w / 2, -x0, y0,  w / 2, Col1, Tag1 );
				SetBeam ( &Model->Beam [ BeamCount + 2 ],  x1, 0, -w / 2,  x0, y0, -w / 2, Col2, Tag2 );
				SetBeam ( &Model->Beam [ BeamCount + 3 ], -x1, 0, -w / 2, -x0, y0, -w / 2, Col2, Tag2 );
			}
			else
			{
				SetBeam ( &Model->Beam [ BeamCount     ],  x0, 0,  w / 2,  x1, y1,  w / 2, Col1, Tag1 );
				SetBeam ( &Model->Beam [ BeamCount + 1 ], -x0, 0,  w / 2, -x1, y1,  w / 2, Col1, Tag1 );
				SetBeam ( &Model->Beam [ BeamCount + 2 ],  x0, 0, -w / 2,  x1, y1, -w / 2, Col2, Tag2 );
				SetBeam ( &Model->Beam [ BeamCount + 3 ], -x0, 0, -w / 2, -x1, y1, -w / 2, Col2, Tag2 );

			} // if else

			BeamCount += 4;
			Bridge->NumDiagBeams += 4;
			if ( zz == 0 ) zz = 1; else zz = 0;

		} // for

	} // if else if else


	// Set the total number of beams used

	Model->NumBeams = BeamCount;

	// Update statistical data and normalize model coordinates

	Bridge->MaterialLength = GetMaterialLength ( Model, Bridge );

	ShowStatData    ( Colors->DataTextColor, Bridge, Layout );
	NormalizeCoords ( Model );
}

//---------------------------------------------------------------------------
// Function: GetMaterialLength
//
// Description:
//
//   Calculates the total length of construction material needed to
//   build the bridge.
//
//   Iterates over all beams, computes each beam's 3D length using the
//   Pythagorean formula, and adjusts for double-strength beams when
//   the split vertical beams option is enabled. Returns the total
//   material length as an integer.
//
// Parameters:
//
//   Model    (ModelData *):          3D model data (beam vertex coordinates, beam count).
//   Bridge   (BridgeDataStruct *):   Bridge design parameters (split beam flags).
//
// Return Value:
//
//   The total construction material length in millimeters.
//
//---------------------------------------------------------------------------

int GetMaterialLength ( ModelData *Model, BridgeDataStruct *Bridge )
{
	float L = 0;                // Total construction material length
	float l = 0;                // Length of an individual beam
	float x1, y1, z1;           // Vertex 1 coordinates
	float x2, y2, z2;           // Vertex 2 coordinates
	float dx, dy, dz;           // Differentials
	int   N = Model->NumBeams;	// Number of beams
	int   n = 0;                // Beam counter

	// Sum up total length of construction materials.

	for ( n = 0, L = 0; n < N; n++ )
	{
		// Get vertex coordinates.

		x1 = Model->Beam [ n ].v1.x;
		y1 = Model->Beam [ n ].v1.y;
		z1 = Model->Beam [ n ].v1.z;

		x2 = Model->Beam [ n ].v2.x;
		y2 = Model->Beam [ n ].v2.y;
		z2 = Model->Beam [ n ].v2.z;

		// Calculate beam length, (Pythagoras).

		dx = x1 - x2;
		dy = y1 - y2;
		dz = z1 - z2;
		l  = sqrt ( dx*dx + dy*dy + dz*dz );

		// Adjust for double strength beams if necessary

		if ( Bridge->SplitVertBeams == True )
		{
			if ( ( Model->Beam [ n ].Tag == 3 ) || ( Model->Beam [ n ].Tag == 4 ) )
			{
				l *= 2;
				l += ( (int)( 1 + ( l / DuelSupportD ) ) ) * DuelSupportL;
			}
		}

		// Add to total length

		L += l;

	} // for

	return (int) L;
}
