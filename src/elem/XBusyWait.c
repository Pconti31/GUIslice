// =======================================================================
// GUIslice library (extensions busywait)
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// =======================================================================
//
// The MIT License
//
// Copyright 2016-2019 Calvin Hass
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
/// \file XBusyWait.c



// GUIslice library
#include "GUIslice.h"
#include "GUIslice_drv.h"

//#include "elem/XBusyWait.h"
#include "XBusyWait.h"

#include <stdio.h>

#if (GSLC_USE_PROGMEM)
    #include <avr/pgmspace.h>
#endif

void delay(unsigned long);

// ----------------------------------------------------------------------------
// Error Messages
// ----------------------------------------------------------------------------

extern const char GSLC_PMEM ERRSTR_NULL[];
extern const char GSLC_PMEM ERRSTR_PXD_NULL[];

// ----------------------------------------------------------------------------
// Extended element definitions
// ----------------------------------------------------------------------------
//
// - This file extends the core GUIslice functionality with
//   additional widget types
//
// ----------------------------------------------------------------------------


// ============================================================================
// Extended Element: BusyWait 
// - XBusyWait is meant to depict a task that is currently executing, 
//   that is of indeterminate length. 
// ============================================================================


// Create a XBusyWait element and add it to the GUI element list
// - Defines callback for redraw
gslc_tsElemRef* gslc_ElemXBusyWaitCreate(gslc_tsGui* pGui, int16_t nElemId, int16_t nPage, 
  gslc_tsXBusyWait* pXData,gslc_tsRect rElem,int16_t nBalls,int16_t nRadius,gslc_tsColor nCol) {
  
  if ((pGui == NULL) || (pXData == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXBusyWaitCreate";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return NULL;
  }
  
  gslc_tsElem     sElem;
  gslc_tsElemRef* pElemRef = NULL;
  
  rElem.h = rElem.w;  // force a square

  sElem = gslc_ElemCreate(pGui,nElemId,nPage,GSLC_TYPEX_BUSYWAIT,rElem,NULL,0,GSLC_FONT_NONE);
  sElem.colElemFill       = GSLC_COL_BLACK;
  sElem.colElemFillGlow   = GSLC_COL_BLACK;
  sElem.colElemFrame      = GSLC_COL_GRAY_DK2;
  sElem.colElemFrameGlow  = GSLC_COL_BLACK;
  sElem.nFeatures        |= GSLC_ELEM_FEA_FILL_EN;
  sElem.nFeatures        &= GSLC_ELEM_FEA_FRAME_EN;

  sElem.nGroup            = GSLC_GROUP_ID_NONE;
  
  // center spinning wheel but first allow room for frame
  gslc_tsRect rInner = gslc_ExpandRect(rElem, -1, -1);
  
  pXData->centerX      = rInner.x + (rInner.w/2);
  pXData->centerY      = rInner.y + (rInner.h/2);
  pXData->nBalls       = nBalls;
  pXData->ballRadius   = nRadius;
  pXData->circleRadius = (rInner.w/2)-pXData->ballRadius;
  pXData->inc          = 360 / nBalls; 
  pXData->color        = nCol;

  sElem.pXData            = (void*)(pXData);
  
  // Specify the custom drawing callback
  sElem.pfuncXDraw        = &gslc_ElemXBusyWaitDraw;
  
  // Specify the custom touch tracking callback
  sElem.pfuncXTouch       = NULL;

  if (nPage != GSLC_PAGE_NONE) {
    pElemRef = gslc_ElemAdd(pGui,nPage,&sElem,GSLC_ELEMREF_DEFAULT);
    return pElemRef;
#if (GSLC_FEATURE_COMPOUND)
  } else {
    // Save as temporary element
    pGui->sElemTmp = sElem;
    pGui->sElemRefTmp.pElem = &(pGui->sElemTmp);
    pGui->sElemRefTmp.eElemFlags = GSLC_ELEMREF_DEFAULT | GSLC_ELEMREF_REDRAW_FULL;
    return &(pGui->sElemRefTmp);
#endif
  }
  return NULL;
}


// Redraw the element
bool gslc_ElemXBusyWaitDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw)
{
  if ((pvGui == NULL) || (pvElemRef == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXBusyWaitDraw";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }
  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*       pGui  = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef*   pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*      pElem = gslc_GetElemFromRef(pGui,pElemRef);

  // Fetch the element's extended data structure
  gslc_tsXBusyWait* pXData = (gslc_tsXBusyWait*)(pElem->pXData);
  if (pXData == NULL) {
    GSLC_DEBUG2_PRINT("ERROR: ElemXBusyWaitDraw(%s) pXData is NULL\n","");
    return false;
  }

  
  int32_t s64, c64;
  int16_t nAngle = 0;
  
  int16_t nX,nY;
  gslc_tsColor col = pXData->color;
  
  // draw all spokes one at a time
  for (int i=0; i<pXData->nBalls*2; i++) {

    if (i==pXData->nBalls) {
      col = pElem->colElemFill;
      nAngle = 0;
    }

    // rotate our point through a tiny rotation matrix
    s64 = gslc_sinFX(nAngle*64);
    s64 = ((int32_t)s64*10000) / 32767;           
    c64 = gslc_cosFX(nAngle*64);
    c64 = ((int32_t)c64*10000) / 32767;

    nX = -(pXData->circleRadius * s64) / 10000 + pXData->centerX;
    nY =  (pXData->circleRadius * c64) / 10000 + pXData->centerY;
    
    gslc_DrvDrawFillCircle(pGui,nX,nY,(int) pXData->ballRadius,col);

    nAngle += pXData->inc;
    delay(100);
  }
  
  return true;

}

// ============================================================================
