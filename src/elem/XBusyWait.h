#ifndef _GUISLICE_EX_XBUSYWAIT_H_
#define _GUISLICE_EX_XBUSYWAIT_H_

#include "GUIslice.h"


// =======================================================================
// GUIslice library extension: XBusyWait
// - XBusyWait is meant to depict a task that is currently executing, 
//   that is of indeterminate length. 
// - Paul Conti
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// =======================================================================
//
// The MIT License
//
// Copyright 2019 Paul Conti
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =======================================================================
/// \file XBusyWait.h

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


// ============================================================================
// Extended Element: Example busywait
// ============================================================================

// Define unique identifier for extended element type
// - Select any number above GSLC_TYPE_BASE_EXTEND
#define  GSLC_TYPEX_BUSYWAIT GSLC_TYPE_BASE_EXTEND + 52

// Extended element data structures
// - These data structures are maintained in the gslc_tsElem
//   structure via the pXData pointer

/// Extended data for Slider element
typedef struct {
  int16_t           centerX;      // center position of our circle display
  int16_t           centerY;
  int16_t           nBalls;       // number of balls to place around the circle
  int16_t           ballRadius;   // size of an individual ball
  int16_t           circleRadius; // size of our main circle
  int16_t           inc;          // amount to increment angles when placing them on the circle
  gslc_tsColor      color;        // Ball Color 
} gslc_tsXBusyWait;

///
/// Create an XBusyWait Element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..16383 or GSLC_ID_AUTO to autogen)
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  pXData:      Ptr to extended element data structure
/// \param[in]  rElem:       Rectangle coordinates defining busy label size
/// \param[in]  nBalls:      Number of balls to place around circle
/// \param[in]  nRadius:     Radius of balls
/// \param[in]  nCol:        Color for the balls
///
/// \return Pointer to Element reference or NULL if failure
///
gslc_tsElemRef* gslc_ElemXBusyWaitCreate(gslc_tsGui* pGui, int16_t nElemId, int16_t nPage, 
  gslc_tsXBusyWait* pXData,gslc_tsRect rElem,int16_t nBalls,int16_t nBallRadius,gslc_tsColor nCol);


///
/// Draw the busywait element on the screen
/// - Called from gslc_ElemDraw()
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
/// \param[in]  pvElemRef:   Void ptr to Element (typecast to gslc_tsElemRef*)
/// \param[in]  eRedraw:     Redraw mode
///
/// \return true if success, false otherwise
///
bool gslc_ElemXBusyWaitDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw);


// ============================================================================


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_EX_XBUSYWAIT_H_

