//***************************************************************************
// Name    - RENDER.CPP
// Version - 1.4a
// Year    - 2000
// Author  - Rohin A Gosling
//
// Description
//
//   Rendering functions for 2D orthographic and 3D perspective display
//   of bridge models.
//
//***************************************************************************

#include "RENDER.H"

//---------------------------------------------------------------------------
// Function: Show3DModel
//
// Description:
//
//   Renders the bridge model as a 3D perspective projection.
//
//   Applies yaw and pitch rotations to each beam, projects 3D
//   coordinates to 2D screen space using perspective division,
//   scales and aspect-corrects the result, and draws all beams
//   within the model window. Displays a "3D View" heading.
//
// Parameters:
//
//   Model             (ModelData *):          3D model data (beam array, camera parameters).
//   Geo               (WindowGeometry *):     Model window screen bounds.
//   Bridge            (BridgeDataStruct *):   Bridge design parameters (dimensions for scale calculation).
//   HeadingTextColor  (int):                  Color for the window heading text.
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void Show3DModel ( ModelData *Model, WindowGeometry *Geo, BridgeDataStruct *Bridge, int HeadingTextColor )
{
	int N = Model->NumBeams;              	// Number of beams used
	int n = 0;                             	// Beam counter

	float f     = Model->f;               	// Camera focal length
	float Yaw   = Model->Yaw;             	// Yaw rotation
	float Pitch = Model->Pitch;           	// Pitch rotation

	float x1, y1, z1;                      	// 3D vertex 1 coordinates
	float x2, y2, z2;                      	// 3D vertex 2 coordinates

	float xt1, yt1, zt1;                   	// Temporary rotation variables
	float xt2, yt2, zt2;                   	// Temporary rotation variables

	float X1, Y1;                          	// Vertex 1 screen coordinates
	float X2, Y2;                          	// Vertex 2 screen coordinates

	float Gw = Geo->Gx2 - Geo->Gx1;      	// Width of model window
	float Gh = Geo->Gy2 - Geo->Gy1;      	// Height of model window
	float Xo = Geo->Gx1 + Gw / 2;         	// X origen of model window
	float Yo = Geo->Gy1 + Gh / 2;         	// Y origen of model window

	float l = Bridge->Length / CoordScale;	// Normalized length of bridge
	float h = Bridge->Height / CoordScale;  // Normalized height of bridge
	float w = Bridge->Width / CoordScale;   // Normalized width of bridge

	float Ax = 1.0;                        	// X graphics resolution aspect ratio
	float Ay = AspectRatioY;               	// Y graphics resolution aspect ratio

	float S  = Scale3D;                    	// Uniform scale
	float Sx = 0;                          	// X Scale
	float Sy = 0;                          	// Y Scale

	// Calculate 2D scale

	if ( h*2 <= l )
	{
		Sx = S*( Gw / l );
		Sy = Sx;
	}
	else
	{
		Sy = S*( Gh / ( h*HeightScaleFactor ) );
		Sx = Sy;
	} // if else

	// Adjust focal length for height of model

	if ( h > l / 2 )
	{
		f = h + ( 750.0 / CoordScale );
	}
	else
	{
		f = 750.0 / CoordScale;
	}

	// Show window title

	settextjustify ( LEFT_TEXT, TOP_TEXT );
	setcolor       ( HeadingTextColor );
	outtextxy      ( Geo->Gx1 + 10, Geo->Gy1 + 5, "3D View" );

	// Plot model

	setcolor ( 15 );

	for ( n = 0; n < N; n++ )
	{
		// Get beam 3D vertex coordinates

		x1 = Model->Beam [ n ].v1.x;
		y1 = Model->Beam [ n ].v1.y;
		z1 = Model->Beam [ n ].v1.z;

		x2 = Model->Beam [ n ].v2.x;
		y2 = Model->Beam [ n ].v2.y;
		z2 = Model->Beam [ n ].v2.z;

		// Yaw rotation ( �y )

		xt1 = x1*cos ( Yaw ) - z1*sin ( Yaw );
		zt1 = x1*sin ( Yaw ) + z1*cos ( Yaw );
		x1  = xt1;
		z1  = zt1;

		xt2 = x2*cos ( Yaw ) - z2*sin ( Yaw );
		zt2 = x2*sin ( Yaw ) + z2*cos ( Yaw );
		x2  = xt2;
		z2  = zt2;

		// Pitch rotation ( �x )

		yt1 = y1*cos ( Pitch ) - z1*sin ( Pitch );
		zt1 = y1*sin ( Pitch ) + z1*cos ( Pitch );
		y1  = yt1;
		z1  = zt1;

		yt2 = y2*cos ( Pitch ) - z2*sin ( Pitch );
		zt2 = y2*sin ( Pitch ) + z2*cos ( Pitch );
		y2  = yt2;
		z2  = zt2;

		// 3D to 2D projection (convert 3D coordinates to 2D coordinates

		X1 =  ( f*x1 ) / ( f + z1 );
		Y1 = -( f*y1 ) / ( f + z1 );

		X2 =  ( f*x2 ) / ( f + z2 );
		Y2 = -( f*y2 ) / ( f + z2 );

		// Set 2D screen scale

		X1 *= Sx;
		Y1 *= Sy;

		X2 *= Sx;
		Y2 *= Sy;

		// Set screen aspect ratio

		X1 *= Ax;
		Y1 *= Ay;

		X2 *= Ax;
		Y2 *= Ay;

		// Set screen displacement (We usually want to center the image hear)

		X1 += Xo;
		Y1 += Yo;

		X2 += Xo;
		Y2 += Yo;

		// Draw beam

		line ( X1, Y1, X2, Y2 );

	} // for
}

//---------------------------------------------------------------------------
// Function: Show2DModel
//
// Description:
//
//   Renders the bridge model as a 2D orthographic side view.
//
//   Draws only side-1 beams (vertical, arch, and diagonal) by
//   filtering on beam tag values. Scales, aspect-corrects, and
//   displaces coordinates to center the model in the window.
//   Displays a "2D View" heading.
//
// Parameters:
//
//   Model             (ModelData *):          3D model data (beam array, beam count).
//   Geo               (WindowGeometry *):     Model window screen bounds.
//   Bridge            (BridgeDataStruct *):   Bridge design parameters (dimensions for scale calculation).
//   HeadingTextColor  (int):                  Color for the window heading text.
//   ModelColor1       (int):                  Color for the model wireframe.
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void Show2DModel ( ModelData *Model, WindowGeometry *Geo, BridgeDataStruct *Bridge, int HeadingTextColor, int ModelColor1 )
{

	float Gw = Geo->Gx2 - Geo->Gx1;      	// Width of model window
	float Gh = Geo->Gy2 - Geo->Gy1;      	// Height of model window
	float Xo = Geo->Gx1 + Gw / 2;         	// X origen of model window
	float Yo = Geo->Gy1 + Gh / 2;         	// Y origen of model window

	float l = Bridge->Length / CoordScale;	// Normalized length of bridge
	float h = Bridge->Height / CoordScale;  // Normalized height of bridge
	float w = Bridge->Width / CoordScale;   // Normalized width of bridge

	float Ax = 1.0;                        	// X graphics resolution aspect ratio
	float Ay = AspectRatioY;               	// Y graphics resolution aspect ratio

	float S  = Scale2D;                    	// Uniform scale
	float Sx = 0;                          	// X Scale
	float Sy = 0;                          	// Y Scale

	float x1, y1;                          	// 2D vertex 1 coordinates
	float x2, y2;                          	// 2D vertex 2 coordinates

	int N = Model->NumBeams;              	// Number of beams used
	int n = 0;                             	// Beam counter


	// Calculate 2D scale

	if ( h*2 <= l )
	{
		Sx = S*( Gw / l );
		Sy = Sx;
	}
	else
	{
		Sy = S*( Gh / ( h*HeightScaleFactor ) );
		Sx = Sy;
	}

	// Show window title

	settextjustify ( LEFT_TEXT, TOP_TEXT );
	setcolor       ( HeadingTextColor );
	outtextxy      ( Geo->Gx1 + 10, Geo->Gy1 + 5, "2D View" );


	// Draw 2D model

	setcolor ( ModelColor1 );

	for ( n = 0; n < N; n++ )
	{
		if ( ( Model->Beam [ n ].Tag == 5 ) || ( Model->Beam [ n ].Tag == 3 ) || ( Model->Beam [ n ].Tag == 1 ) || ( Model->Beam [ n ].Tag == 2 ) )
		{

			// Get line segment coordinates

			x1 = Model->Beam [ n ].v1.x;
			y1 = Model->Beam [ n ].v1.y;
			x2 = Model->Beam [ n ].v2.x;
			y2 = Model->Beam [ n ].v2.y;

			// Scale coordinates

			x1 *= Sx;
			y1 *= -Sy;
			x2 *= Sx;
			y2 *= -Sy;

			// Adjust scale to graphics resolution aspect ratio

			x1 *= Ax;
			y1 *= Ay;
			x2 *= Ax;
			y2 *= Ay;

			// Displace coordinates relative to model window origen

			x1 += Xo;
			y1 += Yo;
			x2 += Xo;
			y2 += Yo;

			// Draw line segment

			line ( x1, y1, x2, y2 );

		} // if
	} // for
}

//---------------------------------------------------------------------------
// Function: ShowBeam
//
// Description:
//
//   Draws a single highlighted beam in the 2D model view.
//
//   Scales and projects the beam's 2D coordinates to screen space,
//   then draws the beam line in the specified color. 
//
//   When the color is non-zero, also draws small colored vertex markers at 
//   each endpoint (red for vertex 1, blue for vertex 2).
//
// Parameters:
//
//   Beam     (int):                  Index of the beam to highlight.
//   Color    (int):                  Highlight color (0 to erase the beam).
//   Model    (ModelData *):          3D model data (beam vertex coordinates).
//   Geo      (WindowGeometry *):     Model window screen bounds.
//   Bridge   (BridgeDataStruct *):   Bridge design parameters (dimensions for scale calculation).
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void ShowBeam ( int Beam, int Color, ModelData *Model, WindowGeometry *Geo, BridgeDataStruct *Bridge )
{

	float Gw = Geo->Gx2 - Geo->Gx1;      	// Width of model window
	float Gh = Geo->Gy2 - Geo->Gy1;      	// Height of model window
	float Xo = Geo->Gx1 + Gw / 2;         	// X origen of model window
	float Yo = Geo->Gy1 + Gh / 2;         	// Y origen of model window

	float l = Bridge->Length / CoordScale;	// Normalized length of bridge
	float h = Bridge->Height / CoordScale;  // Normalized height of bridge
	float w = Bridge->Width / CoordScale;   // Normalized width of bridge

	float Ax = 1.0;                        	// X graphics resolution aspect ratio
	float Ay = AspectRatioY;               	// Y graphics resolution aspect ratio

	float S  = Scale2D;                    	// Uniform scale
	float Sx = 0;                          	// X Scale
	float Sy = 0;                          	// Y Scale

	float x1, y1;                          	// 2D vertex 1 coordinates
	float x2, y2;                          	// 2D vertex 2 coordinates


	// Calculate 2D scale

	if ( h*2 <= l )
	{
		Sx = S * ( Gw / l );
		Sy = Sx;
	}
	else
	{
		Sy = S * ( Gh / ( h*HeightScaleFactor ) );
		Sx = Sy;
	}

	// Get line segment coordinates

	x1 = Model->Beam [ Beam ].v1.x;
	y1 = Model->Beam [ Beam ].v1.y;
	x2 = Model->Beam [ Beam ].v2.x;
	y2 = Model->Beam [ Beam ].v2.y;

	// Scale coordinates

	x1 *=  Sx;
	y1 *= -Sy;
	x2 *=  Sx;
	y2 *= -Sy;

	// Adjust scale to graphics resolution aspect ratio

	x1 *= Ax;
	y1 *= Ay;
	x2 *= Ax;
	y2 *= Ay;

	// Displace coordinates relative to model window origen

	x1 += Xo;
	y1 += Yo;
	x2 += Xo;
	y2 += Yo;

	// Draw beam

	if ( Color == 0 )
	{
		setcolor ( 0 );
		line     ( x1, y1, x2, y2 );
	}
	else
	{
		setcolor     ( Color );
		line         ( x1, y1, x2, y2 );

		setfillstyle ( 1, 12 );
		bar          ( x1 - 2, y1 - 2, x1 + 2, y1 + 2 );

		setfillstyle ( 1, 9 );
		bar          ( x2 - 2, y2 - 2, x2 + 2, y2 + 2 );

	} // if else
}

//---------------------------------------------------------------------------
// Function: Clear2DModelWindow
//
// Description:
//
//   Clears the interior of the 2D model window by filling it with
//   a solid black rectangle, leaving the window border intact.
//
// Parameters:
//
//   Geo   (WindowGeometry *):   Model window screen bounds.
//
// Return Value:
//
//   None.
//
//---------------------------------------------------------------------------

void Clear2DModelWindow ( WindowGeometry *Geo )
{

	setfillstyle ( 1, 0 );
	bar          ( Geo->Gx1 + 1, Geo->Gy1 + 1, Geo->Gx2 - 1, Geo->Gy2 - 1 );

}
