// =======================================================================
// GUIslice library extension: XKeyPad control (Numeric entry)
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
/// \file XKeyPad_Num.c



// GUIslice library
#include "GUIslice.h"
#include "GUIslice_drv.h"

#include "elem/XKeyPad.h"
#include "elem/XKeyPad_Num.h"

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
// - TODO: Support use of PROGMEM. Note that these labels
//   are not currently using "const char" since we may
//   want to support user-modification of the labels.

static char* KEYPAD_LABEL_STRINGS[] = {
  // Special buttons
  "<", ".", "-", "ESC", "ENT",
  // Basic buttons
  "0", "1", "2", "3", "4", "5", "6", "7" ,"8", "9",
};

static gslc_tsKey KEYPAD_KEY_POS[] = {
  // text field
  { KEYPAD_ID_TXT,          0, 0, 6 }, 
  // Special buttons
  { KEYPAD_ID_BACKSPACE,    1, 6, 2 },   
  { KEYPAD_ID_DECIMAL,      2, 5, 1 }, 
  { KEYPAD_ID_MINUS,        1, 5, 1 }, 
  { KEYPAD_ID_ESC,          2, 6, 2 }, 
  { KEYPAD_ID_ENTER,        0, 6, 2 }, 
  // Basic buttons
  { KEYPAD_ID_BASIC_START,  1, 0, 1 },  // "0"
  { KEYPAD_ID_BASIC_START+1,1, 1, 1 },  // "1", 
  { KEYPAD_ID_BASIC_START+2,1, 2, 1 },  // "2", 
  { KEYPAD_ID_BASIC_START+3,1, 3, 1 },  // "3", 
  { KEYPAD_ID_BASIC_START+4,1, 4, 1 },  // "4", 
  { KEYPAD_ID_BASIC_START+5,2, 0, 1 },  // "5", 
  { KEYPAD_ID_BASIC_START+6,2, 1, 1 },  // "6", 
  { KEYPAD_ID_BASIC_START+7,2, 2, 1 },  // "7" ,
  { KEYPAD_ID_BASIC_START+8,2, 3, 1 },  // "8", 
  { KEYPAD_ID_BASIC_START+9,2, 4, 1 },  // "9",
};

// Reset the XKeyPad config struct
// - This must be called before any XKeyPad config update APIs are called
gslc_tsXKeyPadCfg gslc_ElemXKeyPadCfgInit_Num()
{
  gslc_tsXKeyPadCfg sConfig;
  sConfig.nButtonSzW   = 25;
  sConfig.nButtonSzH   = 25;
  sConfig.bFloatEn     = true;
  sConfig.bSignEn      = true;
  sConfig.bRoundEn     = false;
  sConfig.nFontId      = GSLC_FONT_NONE; // Will be overwritten
  sConfig.pActiveLabels = KEYPAD_LABEL_STRINGS;
  sConfig.pLabels1      = NULL;
  sConfig.pLabels2      = NULL;
  sConfig.pKeyPos      = KEYPAD_KEY_POS;
  sConfig.nSpecialKeys = XKEYPADNUM_BTN_SPECIAL;
  sConfig.nBasicKeys   = XKEYPADNUM_BTN_BASIC;
  sConfig.nMaxKeys     = XKEYPADNUM_ELEM_MAX;
  sConfig.nMaxCols     = 8;
  sConfig.nMaxRows     = 3;
  sConfig.nFrameMargin = 2;
  return sConfig;
}


// Create the XKeyPad_Num element
gslc_tsElemRef* gslc_ElemXKeyPadCreate_Num(gslc_tsGui* pGui, int16_t nElemId, int16_t nPage,
  gslc_tsXKeyPad_Num* pXData, int16_t nX0, int16_t nY0, int8_t nFontId, gslc_tsXKeyPadCfg* pConfig)
{

  // Provide default config if none supplied
  gslc_tsXKeyPadCfg sConfigTmp;
  if (pConfig == NULL) {
    sConfigTmp = gslc_ElemXKeyPadCfgInit_Num();
    pConfig = &sConfigTmp;
  }
  
  return gslc_XKeyPadCreateBase(pGui, nElemId, nPage, (void*)pXData, nX0, nY0, nFontId, pConfig);
}


// ============================================================================
