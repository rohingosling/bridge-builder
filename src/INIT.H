//---------------------------------------------------------------------------
// Name    - INIT.H
// Version - 1.4a
// Year    - 2000
// Author  - Rohin A Gosling
//
// Description
//
//   Initialization and startup function declarations.
//
//---------------------------------------------------------------------------

#ifndef INIT_H
#define INIT_H

#include "BTYPES.H"

void InitGraphicsMode ( void );
void InitBridgeData   ( BridgeDataStruct *Data, ModelData *Model );
void WaitRetrace      ( void );
void ShowTitleScreen  ( void );

#endif
