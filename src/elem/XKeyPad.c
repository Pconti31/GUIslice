// =======================================================================
// GUIslice library extension: XKeyPad control
// - Paul Conti, Calvin Hass
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
/// \file XKeyPad.c



// GUIslice library
#include "GUIslice.h"
#include "GUIslice_drv.h"

#include "elem/XKeyPad.h"

#include <stdio.h>

#if (GSLC_USE_PROGMEM)
    #include <avr/pgmspace.h>
#endif

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
// Extended Element: KeyPad
// - Keypad element with numeric input
// - Optionally supports floating point values
// - Optionally supports negative values
// ============================================================================

// Define the button labels
// - TODO: Create more efficient storage for the labels
//   so that it doesn't consume 4 bytes even for labels
//   that can be generated (eg. 0..9, a--z, etc.)
// - NOTE: Not using "const char" in order to support
//   modification of labels by user.


const char KEYPAD_DISP_NEGATIVE = '-';
const char KEYPAD_DISP_DECIMAL_PT = '.';

// NOTE: You must keep this color list in the 
// same order as the enum list of special keys
#define KEYPAD_BTN_IDX    7
#define KEYPAD_TXT_IDX    8
static gslc_tsColor SPECIAL_KEY_COLORS[] = {
  // --- Special Buttons
  GSLC_COL_GRAY_DK1,  //  KEYPAD_ID_BACKSPACE,
  GSLC_COL_BLUE_LT1,  //  KEYPAD_ID_SPACE,
  GSLC_COL_GRAY_DK1,  //  KEYPAD_ID_DECIMAL,
  GSLC_COL_GRAY_DK1,  //  KEYPAD_ID_MINUS,
  GSLC_COL_RED,       //  KEYPAD_ID_ESC,
  GSLC_COL_GREEN_DK2, //  KEYPAD_ID_ENTER,
  GSLC_COL_GRAY_DK1,  //  KEYPAD_ID_SWAP_PAD,
  // --- Basic Buttons
  GSLC_COL_BLUE_LT1,  //  KEYPAD_ID_BASIC_START=100,
  // --- Text field no need for text field since we element's colors
};

// --------------------------------------------------------------------------
// Create the keypad definition
// --------------------------------------------------------------------------
// Convert between keypad ID and the index into the keypad label array
char* gslc_XKeyPadMapLabel(gslc_tsGui* pGui, gslc_tsXKeyPadCfg* pConfig, int16_t nKeyId)
{
  int16_t nKeyIdx=-1; // Not found 

  // Basic button
  if (nKeyId >= KEYPAD_ID_BASIC_START) {
    nKeyIdx = (nKeyId - KEYPAD_ID_BASIC_START) + pConfig->nSpecialKeys;
  } else {
    // Special button - always skip text field
    for (int i=1; i<pConfig->nSpecialKeys+1; i++) {
      if (pConfig->pKeyPos[i].nId == nKeyId) {
        nKeyIdx = i-1;
        break;
      }
    }
  }
  if (nKeyIdx == -1) return NULL;
  return pConfig->pActiveLabels[nKeyIdx];
}

// Convert between keypad ID and the index into the keypad label array
int16_t gslc_XKeyPadLookup(gslc_tsGui* pGui, gslc_tsXKeyPadCfg* pConfig, int16_t nKeyId)
{
  int16_t nKeyIdx=-1; // Not found

  // Basic button
  if (nKeyId >= KEYPAD_ID_BASIC_START) {
    nKeyIdx = (nKeyId - KEYPAD_ID_BASIC_START) + pConfig->nSpecialKeys + 1;
  } else {
    // Special button - always skip text field
    for (int i=1; i<pConfig->nSpecialKeys+1; i++) {
      if (pConfig->pKeyPos[i].nId == nKeyId) {
        nKeyIdx = i;
        break;
      }
    }
  }
  return nKeyIdx;
}

// Generate the keypad layout
void gslc_XKeyPadDrawLayout(gslc_tsGui* pGui, void* pXData, gslc_tsColor cColFrame,
  gslc_tsColor cColFill, gslc_tsColor cColText)
{
  // - Draw the fields
  gslc_tsXKeyPad* pKeypadData = (gslc_tsXKeyPad*)pXData;
  gslc_tsXKeyPadCfg* pConfig = &pKeypadData->sConfig;
  
  uint8_t nId;
  gslc_tsColor cColButton;
  bool bVisible, bTxtField;
  
  // - Draw the fields
  for (int8_t i=0; i < pConfig->nMaxKeys+1; i++) {
    bVisible = true;
    bTxtField = false;
    nId = pConfig->pKeyPos[i].nId;
    if (nId < KEYPAD_ID_BASIC_START) { // check for special keys
      cColButton = SPECIAL_KEY_COLORS[nId];
      // Almost done - now we check for visibility 
      if (nId == KEYPAD_ID_MINUS) {
        bVisible = pConfig->bSignEn;
      }
      if (nId == KEYPAD_ID_DECIMAL) {
        bVisible = pConfig->bFloatEn;
      }
    } else if (nId == KEYPAD_ID_TXT){ // check for text field
      bTxtField = true;
      cColButton = cColFill;
    } else {  // must be normal button
      cColButton = SPECIAL_KEY_COLORS[KEYPAD_BTN_IDX];
    }
    gslc_XKeyPadDrawKey(pGui, pKeypadData, nId, bTxtField, 
      pConfig->pKeyPos[i].nRow, 
      pConfig->pKeyPos[i].nCol, 
      1, 
      pConfig->pKeyPos[i].nColSpan, 
      cColFrame,
      cColButton, 
      cColText,
      bVisible);
  }
}

void gslc_XKeyPadDrawKey(gslc_tsGui* pGui, gslc_tsXKeyPad* pXData, int16_t nKeyId, bool bTxtField, 
  int16_t nRow, int16_t nCol, int8_t nRowSpan, int8_t nColSpan,
  gslc_tsColor cColFrame, gslc_tsColor cColFill, gslc_tsColor cColText, bool bVisible)
{
  gslc_tsXKeyPadCfg* pConfig;
  char* pKeyStr = NULL;

  pConfig = &(pXData->sConfig);

  int16_t nButtonSzW = pConfig->nButtonSzW;
  int16_t nButtonSzH = pConfig->nButtonSzH;
  int16_t nOffsetX = pConfig->nOffsetX;
  int16_t nOffsetY = pConfig->nOffsetY;

  int16_t nButtonW = (nColSpan * nButtonSzW);
  int16_t nButtonH = (nRowSpan * nButtonSzH);

  
  if (!bVisible) {
    // skip addition of invisible buttons
	  return;
  }
  // Fetch the keypad key string
  if (bTxtField) {
    pKeyStr = (char*)pXData->acValStr;
  } else {
    pKeyStr= gslc_XKeyPadMapLabel(pGui, pConfig, nKeyId);
  }

  gslc_tsRect rElem;
  rElem.x = nOffsetX + (nCol*nButtonSzW);
  rElem.y = nOffsetY + (nRow*nButtonSzH);
  rElem.w = nButtonW-1;
  rElem.h = nButtonH - 1;
  
  if (bTxtField) {
    // Text field - save our parameters for redraws
    pXData->nTxtX = rElem.x;
    pXData->nTxtY = rElem.y;
    pXData->nTxtW = rElem.w;
    pXData->nTxtH = rElem.h;
    pXData->cTxtFrame = cColFrame;
    pXData->cTxtFill  = cColFill;
    pXData->cTxtText  = cColText;
    gslc_XKeypadDrawVirtualTxt(pGui,rElem,pKeyStr,XKEYPAD_VAL_LEN,pConfig->nFontId,cColFrame,
      cColFill, cColText);
  } else {
    // Text button
//    GSLC_DEBUG_PRINT("DrawBtn: %d [%s] @ r=%d,c=%d,width=%d\n", nKeyId, gslc_XKeyPadMapLabel(pGui, pConfig, nKeyId), nRow, nCol, nButtonW);
//    GSLC_DEBUG_PRINT("rElem.x=%d rElem.y=%d rElem.w=%d rElem.h=%d\n",rElem.x,rElem.y,rElem.w,rElem.h);
    gslc_XKeyPadDrawVirtualBtn(pGui,rElem,pKeyStr,sizeof(pKeyStr),pConfig->nFontId,cColFrame, 
      cColFill,cColText,pConfig->bRoundEn);
  }

}

gslc_tsElemRef* gslc_XKeyPadCreateBase(gslc_tsGui* pGui, int16_t nElemId, int16_t nPage,
  gslc_tsXKeyPad* pXData, int16_t nX0, int16_t nY0, int8_t nFontId, gslc_tsXKeyPadCfg* pConfig)
{
  if ((pGui == NULL) || (pXData == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXKeyPadCreate";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL, FUNCSTR);
    return NULL;
  }

  // Fetch config options
  int8_t nButtonSzW = pConfig->nButtonSzW;
  int8_t nButtonSzH = pConfig->nButtonSzH;

  gslc_tsRect rElem;
  rElem.x = nX0;
  rElem.y = nY0;
  rElem.w = (nButtonSzW * pConfig->nMaxCols) + (2 * pConfig->nFrameMargin);
  rElem.h = (nButtonSzH * pConfig->nMaxRows) + (2 * pConfig->nFrameMargin);

  gslc_tsElem sElem;
  gslc_tsElemRef* pElemRef = NULL;

  // Initialize element
  sElem = gslc_ElemCreate(pGui, nElemId, nPage, GSLC_TYPEX_KEYPAD, rElem, NULL, 0, GSLC_FONT_NONE);
  sElem.nFeatures |= GSLC_ELEM_FEA_FRAME_EN;
  sElem.nFeatures |= GSLC_ELEM_FEA_FILL_EN;
  sElem.nFeatures |= GSLC_ELEM_FEA_CLICK_EN;
  sElem.nFeatures &= ~GSLC_ELEM_FEA_GLOW_EN;  // Don't need to glow outer element
  sElem.nGroup = GSLC_GROUP_ID_NONE;

  sElem.pXData = (void*)(pXData);
  // Specify the custom drawing callback
  sElem.pfuncXDraw = &gslc_XKeyPadDraw;
  // Specify the custom touch tracking callback
  sElem.pfuncXTouch = &gslc_XKeyPadTouch;
 
  // shouldn't be used
  sElem.colElemFill       = GSLC_COL_BLACK;
  sElem.colElemFillGlow   = GSLC_COL_BLACK;
  sElem.colElemFrame      = GSLC_COL_WHITE;
  sElem.colElemFrameGlow  = GSLC_COL_WHITE;
  sElem.colElemText       = GSLC_COL_WHITE;

  // Determine offset coordinate of compound element so that we can
  // specify relative positioning during the sub-element Create() operations.
  int16_t nOffsetX = nX0 + pConfig->nFrameMargin;
  int16_t nOffsetY = nY0 + pConfig->nFrameMargin;

  // Reset value string
  memset(pXData->acValStr, 0, XKEYPAD_VAL_LEN);
  pXData->nValStrPos = 0;

  pXData->pfuncCb       = NULL;
  pXData->nTargetId     = GSLC_ID_NONE;  // Default to no target defined
  pXData->bValPositive  = true;
  pXData->bValDecimalPt = false;

  // Update config with constructor settings
  // Ensure the Font has been defined
  if (gslc_FontGet(pGui,nFontId) == NULL) {
    GSLC_DEBUG_PRINT("ERROR: gslc_XKeyPadCreateBase(ID=%d): Font(ID=%d) not loaded\n",nElemId,nFontId);
    return NULL;
  }
  pConfig->nFontId = nFontId;
  pConfig->nOffsetX = nOffsetX;
  pConfig->nOffsetY = nOffsetY;


  // Copy content into local structure
  pXData->sConfig = *pConfig;

  // Now proceed to add the element to the page
  if (nPage != GSLC_PAGE_NONE) {
    pElemRef = gslc_ElemAdd(pGui, nPage, &sElem, GSLC_ELEMREF_DEFAULT);
    return pElemRef;
  }
  return NULL;
}

void gslc_ElemXKeyPadValSet(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, const char* pStrBuf)
{
  gslc_tsXKeyPad* pKeyPad = (gslc_tsXKeyPad*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_KEYPAD, __LINE__);
  if (!pKeyPad) return;

  // Copy over the new value string
  strncpy(pKeyPad->acValStr, pStrBuf, XKEYPAD_VAL_LEN);
  pKeyPad->acValStr[XKEYPAD_VAL_LEN - 1] = 0; // Force null terminate

  // Handle negation and floating point detection
  pKeyPad->bValPositive = true;
  if (pKeyPad->acValStr[0] == KEYPAD_DISP_NEGATIVE) {
    pKeyPad->bValPositive = false;
  }
  pKeyPad->bValDecimalPt = false;
  if (strchr(pKeyPad->acValStr, KEYPAD_DISP_DECIMAL_PT)) {
    pKeyPad->bValDecimalPt = true;
  }

  // Update the current buffer position
  pKeyPad->nValStrPos = strlen(pKeyPad->acValStr);

  // Find element associated with text field
//  gslc_tsElemRef* pTxtElemRef = gslc_CollectFindElemById(pGui, &pKeyPad->sCollect, KEYPAD_ID_TXT);
  // Mark as needing redraw
//  gslc_ElemSetRedraw(pGui,pTxtElemRef,GSLC_REDRAW_INC);
}

void gslc_ElemXKeyPadTargetIdSet(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int16_t nTargetId)
{
  gslc_tsXKeyPad* pKeyPad = (gslc_tsXKeyPad*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_KEYPAD, __LINE__);
  if (!pKeyPad) return;
  pKeyPad->nTargetId = nTargetId;
}

int16_t gslc_ElemXKeyPadDataTargetIdGet(gslc_tsGui* pGui, void* pvData)
{
  if (pvData == NULL) {
    #if defined(DEBUG_LOG)
    GSLC_DEBUG2_PRINT("ERROR: gslc_ElemXKeyPadDataTargetIdGet() NULL data\n", "");
    #endif
    return GSLC_ID_NONE;
  }
  gslc_tsXKeyPadData* pData = (gslc_tsXKeyPadData*)pvData;
  return pData->nTargetId;
}

char* gslc_ElemXKeyPadDataValGet(gslc_tsGui* pGui, void* pvData)
{
  if (pvData == NULL) {
    #if defined(DEBUG_LOG)
    GSLC_DEBUG2_PRINT("ERROR: gslc_ElemXKeyPadDataValGet() NULL data\n", "");
    #endif
    return NULL;
  }
  gslc_tsXKeyPadData* pData = (gslc_tsXKeyPadData*)pvData;
  return pData->pStr;
}



// NOTE: API changed to pass nStrBufLen (total buffer size including terminator)
//       instead of nStrBufMax (max index value)
bool gslc_ElemXKeyPadValGet(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, char* pStrBuf, uint8_t nStrBufLen)
{
  gslc_tsXKeyPad* pKeyPad = (gslc_tsXKeyPad*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_KEYPAD, __LINE__);
  if (!pKeyPad) return false;

  // - Check for negation flag
  // - If negative, copy minus sign
  // - Copy the remainder of the string
  // FIXME: check for shortest of XKEYPAD_VAL_LEN & nStrBufLen
  char* pBufPtr = pStrBuf;
  uint8_t nMaxCopyLen = nStrBufLen - 1;
  // FIXME: Do we still need to do this step?
  if (!pKeyPad->bValPositive) {
    *pBufPtr = KEYPAD_DISP_NEGATIVE;
    pBufPtr++;
    nMaxCopyLen--;
  }
  strncpy(pBufPtr, pKeyPad->acValStr, nMaxCopyLen);
  pStrBuf[nStrBufLen-1] = '\0';  // Force termination

  // TODO: Do we need to reset the contents?

  return true;
}


// Redraw the KeyPad element
// - When drawing a KeyPad we do not clear the background.
bool gslc_XKeyPadDraw(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw)
{
  gslc_tsGui*       pGui  = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef*   pElemRef = (gslc_tsElemRef*)(pvElemRef);

  gslc_tsXKeyPad* pKeyPad = (gslc_tsXKeyPad*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_KEYPAD, __LINE__);
  if (!pKeyPad) return false;
  gslc_tsXKeyPadCfg* pConfig;
  pConfig = &(pKeyPad->sConfig);

  gslc_tsElem*    pElem = gslc_GetElemFromRef(pGui, pElemRef);

  GSLC_DEBUG_PRINT("DRAW: %d\n", eRedraw);

  // Draw any parts of the compound element itself
  if (eRedraw == GSLC_REDRAW_FULL) {
    // Force the fill to ensure the background doesn't bleed through gaps
    // between the keys
    gslc_DrawFillRect(pGui, pElem->rElem, pElem->colElemFill);
    if (pElem->nFeatures & GSLC_ELEM_FEA_FRAME_EN) {
      gslc_DrawFrameRect(pGui, pElem->rElem, pElem->colElemFrame);
    }
    // Draw the keys and text field
    gslc_XKeyPadDrawLayout(pGui, (void*)pKeyPad, pElem->colElemFrame, pElem->colElemFill, pElem->colElemText);
  } else if (eRedraw == GSLC_REDRAW_INC) {
    gslc_XKeypadDrawVirtualTxt(pGui,  
      (gslc_tsRect) { pKeyPad->nTxtX, pKeyPad->nTxtY, pKeyPad->nTxtW, pKeyPad->nTxtH },
      pKeyPad->acValStr,XKEYPAD_VAL_LEN,pConfig->nFontId,pKeyPad->cTxtFrame,pKeyPad->cTxtFill,pKeyPad->cTxtText);
  }
  // Clear the redraw flag
  gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_NONE);

  // Mark page as needing flip
  gslc_PageFlipSet(pGui, true);

  return true;
}

void gslc_ElemXKeyPadValSetCb(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, GSLC_CB_INPUT pfuncCb)
{
  gslc_tsXKeyPad* pKeyPad = (gslc_tsXKeyPad*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_KEYPAD, __LINE__);
  if (!pKeyPad) return;

  pKeyPad->pfuncCb = pfuncCb;
}

// Change the sign of the number string
// - This function also performs in-place shifting of the content
void gslc_XKeyPadValSetSign(gslc_tsGui* pGui, gslc_tsElemRef *pElemRef, bool bPositive)
{
  gslc_tsXKeyPad* pKeyPad = (gslc_tsXKeyPad*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_KEYPAD, __LINE__);
  if (!pKeyPad) return;

  char* pStrBuf = pKeyPad->acValStr;

  //GSLC_DEBUG2_PRINT("SetSign: old=%d new=%d\n", pKeyPad->bValPositive, bPositive);
  if (pKeyPad->bValPositive == bPositive) {
    // No change to sign
    return;
  }

  //GSLC_DEBUG2_PRINT("SetSign:   str_old=[%s] idx=%d\n", pStrBuf,pKeyPad->nValStrPos);

  if ((pKeyPad->bValPositive) && (!bPositive)) {
    // Change from positive to negative
    // - Shift string right one position, and replace first position with minus sign
    // - Increase current buffer position
    memmove(pStrBuf+1, pStrBuf, XKEYPAD_VAL_LEN-1);
    pStrBuf[0] = KEYPAD_DISP_NEGATIVE; // Overwrite with minus sign
    pStrBuf[XKEYPAD_VAL_LEN-1] = 0; // Force terminate
    pKeyPad->nValStrPos++; // Advance buffer position
  } else if ((!pKeyPad->bValPositive) && (bPositive)) {
    // Change from negative to positive
    // - Shift string left one position, overwriting the minus sign
    memmove(pStrBuf, pStrBuf+1, XKEYPAD_VAL_LEN-1);
    pStrBuf[XKEYPAD_VAL_LEN-1] = 0; // Force terminate
    if (pKeyPad->nValStrPos > 0) {
      // Expect that original position should be non-zero if previously minux
      pKeyPad->nValStrPos--; // Reduce buffer position
    }
  }

  //GSLC_DEBUG2_PRINT("SetSign:   str_new=[%s] idx=%d\n", pStrBuf,pKeyPad->nValStrPos);

  // Update sign state
  pKeyPad->bValPositive = bPositive;

  // The text string sub-element content has already been updated,
  // so now we can simply mark it for redraw
//  gslc_tsElemRef* pTxtElemRef = gslc_CollectFindElemById(pGui, &pKeyPad->sCollect, KEYPAD_ID_TXT);
//  gslc_ElemSetRedraw(pGui,pTxtElemRef,GSLC_REDRAW_INC);

}

bool gslc_ElemXKeyPadAddChar(gslc_tsGui* pGui, gslc_tsXKeyPad* pKeyPad, char ch)
{
  bool bRedraw = false;
  // Do we have space for this character?
  if (pKeyPad->nValStrPos < XKEYPAD_VAL_LEN - 1) {
    pKeyPad->acValStr[pKeyPad->nValStrPos] = ch;
    pKeyPad->nValStrPos++;
    pKeyPad->acValStr[pKeyPad->nValStrPos] = '\0'; // Zero terminate
    bRedraw = true;
  }
  return bRedraw;
}

// Map keypad ID to its bounding rectangle position
int16_t gslc_XKeyPadMapEvent(gslc_tsGui* pGui, void* pXData, int16_t nRelX, int16_t nRelY)
{
  gslc_tsXKeyPad* pKeypad = (gslc_tsXKeyPad*)pXData;
  gslc_tsXKeyPadCfg* pConfig;

  pConfig = &(pKeypad->sConfig);

  int16_t nIdx;
  uint8_t nRow, nCol, nRowSpan, nColSpan;
  

  int16_t nButtonSzW = pConfig->nButtonSzW;
  int16_t nButtonSzH = pConfig->nButtonSzH;
  int16_t nOffsetX   = pConfig->nOffsetX;
  int16_t nOffsetY   = pConfig->nOffsetY;
  
//  GSLC_DEBUG_PRINT("[XKeyPadMapEvent_Num] nRelX=%d nRelY=%d\n",nRelX,nRelY);

  gslc_tsRect       selRect;

  // Scan for button - always skip text field
  for (int i=1; i<pConfig->nMaxKeys+1; i++) 
  {
    nRow     = pConfig->pKeyPos[i].nRow; 
    nCol     = pConfig->pKeyPos[i].nCol; 
    nRowSpan = 1; 
    nColSpan = pConfig->pKeyPos[i].nColSpan;
  
    int16_t nButtonW = (nColSpan * nButtonSzW);
    int16_t nButtonH = (nRowSpan * nButtonSzH);

    selRect.x = nOffsetX + (nCol*nButtonSzW);
    selRect.y = nOffsetY + (nRow*nButtonSzH);
    selRect.w = nButtonW-1;
    selRect.h = nButtonH - 1;
    
//    GSLC_DEBUG_PRINT("selRect.x=%d selRect.y=%d selRect.w=%d selRect.h=%d\n",selRect.x,selRect.y,selRect.w,selRect.h);

    if (gslc_IsInRect(nRelX,nRelY,selRect)) {
//      GSLC_DEBUG_PRINT("match: i=%d ID=%d\n", i, pConfig->pKeyPos[i].nId);
      return (int16_t)pConfig->pKeyPos[i].nId;
    }

  }
  return -1;
}

// - This routine is called by gslc_ElemEvent() to handle
//   any click events that resulted from the touch tracking process.
bool gslc_XKeyPadTouch(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY)
{
  #if defined(DRV_TOUCH_NONE)
  return false;
  #else

  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  // NO point in this call since we don't support flash based keypad.
  // gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  gslc_tsElem* pElem = pElemRef->pElem;
  gslc_tsRect rElem = pElem->rElem;
  
  int16_t nRelX = nX + rElem.x;
  int16_t nRelY = nY + rElem.y;

  // this call is excessive given the data structures and the overhead it causes
  //gslc_tsXKeyPad* pKeyPad = (gslc_tsXKeyPad*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_KEYPAD, __LINE__);
  gslc_tsXKeyPad* pKeyPad = (gslc_tsXKeyPad*)pElem->pXData;
  if (pKeyPad == NULL) return false;

  gslc_tsXKeyPadCfg* pConfig;
  pConfig = &(pKeyPad->sConfig);
  
//  GSLC_DEBUG_PRINT("KeyPad Click   Touch=%d\n", eTouch);
  gslc_teRedrawType eRedraw = GSLC_REDRAW_NONE;
  
  // Handle the various button presses
  if (eTouch == GSLC_TOUCH_UP_IN) {
    bool bValRedraw = false;
    char* sKey;

    GSLC_CB_INPUT pfuncXInput = pKeyPad->pfuncCb;
    gslc_tsXKeyPadData sKeyPadData;

    // Prepare the return data
    sKeyPadData.pStr = pKeyPad->acValStr;
    sKeyPadData.nTargetId = pKeyPad->nTargetId;
    
    // Determine if touch matches any fields on the keypad
    int16_t nId = gslc_XKeyPadMapEvent(pGui, pKeyPad, nRelX, nRelY);

    GSLC_DEBUG_PRINT("KeyPad ID=%d\n", nId);
    if (nId == -1) return false;

    
    switch (nId) {

    case KEYPAD_ID_ENTER:
      //GSLC_DEBUG2_PRINT("KeyPad Key=ENT\n", "");
      //GSLC_DEBUG2_PRINT("KeyPad Done Str=[%s]\n", pKeyPad->acValStr);
   
      // Issue callback with Done status
      if (pfuncXInput != NULL) {
        (*pfuncXInput)(pvGui, (void*)(pElemRef), XKEYPAD_CB_STATE_DONE, (void*)(&sKeyPadData));
      }

      // Clear the contents
      memset(pKeyPad->acValStr, 0, XKEYPAD_VAL_LEN);
      pKeyPad->nValStrPos = 0;
      pKeyPad->nTargetId = GSLC_ID_NONE;
      break;

    case KEYPAD_ID_ESC:
      //GSLC_DEBUG2_PRINT("KeyPad Key=ESC\n", "");
      // Clear the contents
      memset(pKeyPad->acValStr, 0, XKEYPAD_VAL_LEN);
      pKeyPad->nValStrPos = 0;
      pKeyPad->nTargetId = GSLC_ID_NONE;    
      bValRedraw = true;
      //GSLC_DEBUG2_PRINT("KeyPad Done Str=[%s]\n", pKeyPad->acValStr);
      // Issue callback with Cancel status
      if (pfuncXInput != NULL) {
        (*pfuncXInput)(pvGui, (void*)(pElemRef), XKEYPAD_CB_STATE_CANCEL, (void*)(&sKeyPadData));
      }
      break;

    case KEYPAD_ID_DECIMAL:
      //GSLC_DEBUG2_PRINT("KeyPad Key=Decimal\n", "");
      if (!pKeyPad->sConfig.bFloatEn) break; // Ignore if floating point not enabled
      if (!pKeyPad->bValDecimalPt) {
        bValRedraw |= gslc_ElemXKeyPadAddChar(pGui, pKeyPad, KEYPAD_DISP_DECIMAL_PT);
      }
      break;

    case KEYPAD_ID_MINUS:
      //GSLC_DEBUG2_PRINT("KeyPad Key=Minus\n", "");
      if (!pKeyPad->sConfig.bSignEn) break; // Ignore if negative numbers not enabled
      // Toggle sign
      gslc_XKeyPadValSetSign(pGui, pElemRef, pKeyPad->bValPositive ? false : true);
      bValRedraw = true;
      break;

    case KEYPAD_ID_BACKSPACE:
      //GSLC_DEBUG2_PRINT("KeyPad Key=BS\n", "");
      if (pKeyPad->nValStrPos < 1) break;
      pKeyPad->nValStrPos--;
      // Handle the special case of decimal point if floating point enabled
      if (pKeyPad->sConfig.bFloatEn) {
        if (pKeyPad->acValStr[pKeyPad->nValStrPos] == KEYPAD_DISP_DECIMAL_PT) {
          //GSLC_DEBUG2_PRINT("KeyPad Key=BS across decimal\n", "");
          pKeyPad->bValDecimalPt = false;
        }
      }
      if (pKeyPad->nValStrPos == 0) {
        memset(pKeyPad->acValStr, 0, XKEYPAD_VAL_LEN);
        pKeyPad->bValPositive = true;
      }
      pKeyPad->acValStr[pKeyPad->nValStrPos] = '\0';
      bValRedraw = true;
      break;

    case KEYPAD_ID_SWAP_PAD:
       // first check if we have multiple keypads
      if (pConfig->pLabels1 == NULL || pConfig->pLabels2 == NULL)
         break;
      // swap key pads
      if (pConfig->pActiveLabels == pConfig->pLabels1) {
        pConfig->pActiveLabels = pConfig->pLabels2;
      } else {
        pConfig->pActiveLabels = pConfig->pLabels1;
      }
      // Draw the keys and text field
      gslc_XKeyPadDrawLayout(pGui, (void*)pKeyPad, pElem->colElemFrame, pElem->colElemFill, pElem->colElemText);
      break;
      
    case KEYPAD_ID_SPACE:
      sKey = " ";
      bValRedraw |= gslc_ElemXKeyPadAddChar(pGui, pKeyPad, sKey[0]);
      break;
      
    default:
      // Normal character
      //GSLC_DEBUG2_PRINT("KeyPad Key=Digit\n", "");
      // For basic buttons, we need to fetch the keypad string index
      sKey = gslc_XKeyPadMapLabel(pGui, pConfig, nId);
      bValRedraw |= gslc_ElemXKeyPadAddChar(pGui, pKeyPad, sKey[0]);
      break;
    } // switch

    // Do we need to redraw the text field?
    if (bValRedraw) {
        eRedraw = GSLC_REDRAW_INC;
      // Issue callback with Update status - WHY????
//      if (pfuncXInput != NULL) {
//        (*pfuncXInput)(pvGui, (void*)(pElemRef), XKEYPAD_CB_STATE_UPDATE, (void*)(&sKeyPadData));
//      }
    } // bValRedraw
  } // eTouch

  gslc_ElemSetRedraw(pGui,pElemRef,eRedraw);

  return true;
  #endif // !DRV_TOUCH_NONE
}

void gslc_ElemXKeyPadCfgSetButtonSz(gslc_tsXKeyPadCfg* pConfig, int8_t nButtonSzW, int8_t nButtonSzH)
{
  pConfig->nButtonSzW = nButtonSzW;
  pConfig->nButtonSzH = nButtonSzH;
}

void gslc_ElemXKeyPadCfgSetSpecialColors(gslc_tsXKeyPadCfg* pConfig,gslc_tsColor cColBasic,gslc_tsColor cColBacksp, 
  gslc_tsColor cColSpace,gslc_tsColor cColDecimal,gslc_tsColor cColMinus,gslc_tsColor cColEsc, 
  gslc_tsColor cColEnt, gslc_tsColor cColSwap)
{
  SPECIAL_KEY_COLORS[KEYPAD_BTN_IDX]      = cColBasic;
  SPECIAL_KEY_COLORS[KEYPAD_ID_BACKSPACE] = cColBacksp;
  SPECIAL_KEY_COLORS[KEYPAD_ID_SPACE]     = cColSpace;
  SPECIAL_KEY_COLORS[KEYPAD_ID_DECIMAL]   = cColDecimal;
  SPECIAL_KEY_COLORS[KEYPAD_ID_MINUS]     = cColMinus;
  SPECIAL_KEY_COLORS[KEYPAD_ID_ESC]       = cColEsc;
  SPECIAL_KEY_COLORS[KEYPAD_ID_ENTER]     = cColEnt;
  SPECIAL_KEY_COLORS[KEYPAD_ID_SWAP_PAD]  = cColSwap;
}

void gslc_ElemXKeyPadCfgSetFloatEn(gslc_tsXKeyPadCfg* pConfig, bool bEn)
{
  pConfig->bFloatEn = bEn;
}

void gslc_ElemXKeyPadCfgSetSignEn(gslc_tsXKeyPadCfg* pConfig, bool bEn)
{
  pConfig->bSignEn = bEn;
}

void gslc_ElemXKeyPadCfgSetRoundEn(gslc_tsXKeyPadCfg* pConfig, bool bEn)
{
  pConfig->bRoundEn = bEn;
}


void gslc_ElemXKeyPadSetFloatEn(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, bool bEn)
{
  gslc_tsXKeyPad* pKeyPad = (gslc_tsXKeyPad*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_KEYPAD, __LINE__);
  if (!pKeyPad) return;
  pKeyPad->sConfig.bFloatEn = bEn;
  // Mark as needing full redraw as button visibility may have changed
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}

// FIXME: Runtime API not fully functional yet - Do not use
void gslc_ElemXKeyPadSetSignEn(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, bool bEn)
{
  gslc_tsXKeyPad* pKeyPad = (gslc_tsXKeyPad*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_KEYPAD, __LINE__);
  if (!pKeyPad) return;
  pKeyPad->sConfig.bSignEn = bEn;
  // Mark as needing full redraw as button visibility may have changed
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}

void gslc_XKeypadDrawVirtualTxt(gslc_tsGui* pGui,gslc_tsRect rElem,
  char* pStrBuf,uint8_t nStrBufMax,int16_t nFontId, gslc_tsColor cColFrame,
  gslc_tsColor cColFill, gslc_tsColor cColText)
{
  gslc_tsElem  sElem;
  gslc_tsElem* pVirtualElem = &sElem;

  pVirtualElem->colElemFill       = cColFill;
  pVirtualElem->colElemFillGlow   = cColFill;
  pVirtualElem->colElemFrame      = cColFrame;
  pVirtualElem->colElemFrameGlow  = cColFrame;
  pVirtualElem->colElemText       = cColText;
  pVirtualElem->colElemTextGlow   = cColText;

  pVirtualElem->nFeatures         = GSLC_ELEM_FEA_NONE;
  pVirtualElem->nFeatures        |= GSLC_ELEM_FEA_FRAME_EN;
  pVirtualElem->nFeatures        |= GSLC_ELEM_FEA_FILL_EN;

  pVirtualElem->nId               = 0;
  pVirtualElem->rElem             = rElem;
  pVirtualElem->nType             = 0;
  pVirtualElem->pTxtFont          = gslc_FontGet(pGui,nFontId);

  pVirtualElem->pStrBuf           = pStrBuf;
  pVirtualElem->nStrBufMax        = nStrBufMax;
  pVirtualElem->eTxtFlags         = GSLC_TXT_DEFAULT;
  pVirtualElem->eTxtFlags         = (pVirtualElem->eTxtFlags & ~GSLC_TXT_ALLOC) | GSLC_TXT_ALLOC_EXT;
  pVirtualElem->eTxtAlign         = GSLC_ALIGN_MID_LEFT;
  pVirtualElem->nTxtMarginX       = 5;
  pVirtualElem->nTxtMarginY       = 5;

  pVirtualElem->nFeatures |= GSLC_ELEM_FEA_VALID;
  
  // display our virtual element to screen
  gslc_XKeyPadDrawVirtualElem(pGui, pVirtualElem);

}

void gslc_XKeyPadDrawVirtualBtn(gslc_tsGui* pGui, gslc_tsRect rElem,
  char* pStrBuf,uint8_t nStrBufMax,int16_t nFontId, gslc_tsColor cColFrame,
  gslc_tsColor cColFill, gslc_tsColor cColText, bool bRoundedEn)
{
  gslc_tsElem  sElem;
  gslc_tsElem* pVirtualElem = &sElem;

  pVirtualElem->colElemFill       = cColFill;
  pVirtualElem->colElemFillGlow   = cColFill;
  pVirtualElem->colElemFrame      = cColFrame;
  pVirtualElem->colElemFrameGlow  = cColFrame;
  pVirtualElem->colElemText       = cColText;
  pVirtualElem->colElemTextGlow   = cColText;
  pVirtualElem->nFeatures         = GSLC_ELEM_FEA_NONE;
  pVirtualElem->nFeatures        |= GSLC_ELEM_FEA_FRAME_EN;
  pVirtualElem->nFeatures        |= GSLC_ELEM_FEA_FILL_EN;

  pVirtualElem->nId               = 0;
  pVirtualElem->rElem             = rElem;
  pVirtualElem->nType             = 0;
  pVirtualElem->pTxtFont          = gslc_FontGet(pGui,nFontId);

  pVirtualElem->pStrBuf           = pStrBuf;
  pVirtualElem->nStrBufMax        = nStrBufMax;
  pVirtualElem->eTxtFlags         = GSLC_TXT_DEFAULT;
  pVirtualElem->eTxtFlags         = (pVirtualElem->eTxtFlags & ~GSLC_TXT_ALLOC) | GSLC_TXT_ALLOC_EXT;
  pVirtualElem->eTxtAlign         = GSLC_ALIGN_MID_MID;
  pVirtualElem->nTxtMarginX       = 0;
  pVirtualElem->nTxtMarginY       = 0;

  // For the text buttons, optionally use rounded profile if enabled
  if (bRoundedEn) {
    pVirtualElem->nFeatures |= GSLC_ELEM_FEA_ROUND_EN;
  } else {
    pVirtualElem->nFeatures &= ~GSLC_ELEM_FEA_ROUND_EN;
  }
  pVirtualElem->nFeatures |= GSLC_ELEM_FEA_VALID;

  // display our virtual element to screen
  gslc_XKeyPadDrawVirtualElem(pGui, pVirtualElem);

}

// Draw a virtual element to the active display
// - Element is referenced by an fake temporary element pointer
// By the way do you know the difference between virtual and invisible?
// invisible is something that's there that you can't see
// while virtual is something you can see that's not there. :)
bool gslc_XKeyPadDrawVirtualElem(gslc_tsGui* pGui,gslc_tsElem* pElem)
{

  // --------------------------------------------------------------------------
  // Init for default drawing
  // --------------------------------------------------------------------------
  
  // NOTE no point in supporting Glow since change would happen too quickly.
  
  int16_t   nElemX,nElemY;
  uint16_t  nElemW,nElemH;

  nElemX    = pElem->rElem.x;
  nElemY    = pElem->rElem.y;
  nElemW    = pElem->rElem.w;
  nElemH    = pElem->rElem.h;

  // --------------------------------------------------------------------------
  // Background
  // --------------------------------------------------------------------------

  // Fill in the background
  gslc_tsRect rElemInner = pElem->rElem;
  // - Assume both fill and frame are enabled so contract
  //   the fill region slightly so that we don't overdraw
  //   the frame (prevent unnecessary flicker).
  rElemInner = gslc_ExpandRect(rElemInner,-1,-1);
  gslc_tsColor colBg = pElem->colElemFill;

  if (pElem->nFeatures & GSLC_ELEM_FEA_ROUND_EN) {
      gslc_DrawFillRoundRect(pGui, rElemInner, pGui->nRoundRadius, colBg);
  } else {
    gslc_DrawFillRect(pGui, rElemInner, colBg);
  }

  // --------------------------------------------------------------------------
  // Frame
  // --------------------------------------------------------------------------

  // Frame the region
  if (pElem->nFeatures & GSLC_ELEM_FEA_ROUND_EN) {
    gslc_DrawFrameRoundRect(pGui, pElem->rElem, pGui->nRoundRadius, pElem->colElemFrame);
  } else {
    gslc_DrawFrameRect(pGui, pElem->rElem, pElem->colElemFrame);
  }

  // --------------------------------------------------------------------------
  // Text overlays
  // --------------------------------------------------------------------------

  // Draw text string if defined
  if (pElem->pStrBuf) {
    gslc_tsColor  colTxt    = pElem->colElemText;
    int8_t        nMarginX  = pElem->nTxtMarginX;
    int8_t        nMarginY  = pElem->nTxtMarginY;

    gslc_DrawTxtBase(pGui, pElem->pStrBuf, pElem->rElem, pElem->pTxtFont, pElem->eTxtFlags,
      pElem->eTxtAlign, colTxt, colBg, nMarginX, nMarginY);
  }

  return true;
}

void gslc_ElemXKeyPadInputAsk(gslc_tsGui* pGui, gslc_tsElemRef* pKeyPadRef, int16_t nPgPopup, gslc_tsElemRef* pTxtRef)
{
  // Fetch the Element ID from the text element
  gslc_tsElem* pTxtElem = gslc_GetElemFromRefD(pGui,pTxtRef,__LINE__);
  if (!pTxtElem) return;
  int16_t nTxtElemId = pTxtElem->nId;

  // Associate the keypad with the text field
  gslc_ElemXKeyPadTargetIdSet(pGui, pKeyPadRef, nTxtElemId);
  // Show a popup box for the keypad
  gslc_PopupShow(pGui, nPgPopup, true);
  // Preload text field with current value
  gslc_ElemXKeyPadValSet(pGui, pKeyPadRef, gslc_ElemGetTxtStr(pGui, pTxtRef));
}

char* gslc_ElemXKeyPadInputGet(gslc_tsGui* pGui, gslc_tsElemRef* pTxtRef, void* pvCbData)
{
  char* pStr = NULL;
  // Fetch the current value of the keypad popup
  pStr = gslc_ElemXKeyPadDataValGet(pGui, pvCbData);
  // Update the linked text item
  gslc_ElemSetTxtStr(pGui, pTxtRef, gslc_ElemXKeyPadDataValGet(pGui, pvCbData));
  // Hide the popup
  gslc_PopupHide(pGui);

  // Return the text string in case the user wants it
  return pStr;
}


// ============================================================================
