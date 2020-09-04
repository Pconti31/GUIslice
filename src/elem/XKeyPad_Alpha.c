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
/// \file XKeyPad_alpha.c



// GUIslice library
#include "GUIslice.h"
#include "GUIslice_drv.h"

#include "elem/XKeyPad.h"
#include "elem/XKeyPad_Alpha.h"

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

static char* KEYPAD_ALPHA_LABEL[] = {
  // Special buttons
  "<", " ", "ESC", "ENT","123",
  // Basic buttons
  "Q", "W", "E", "R", "T", "Y", "U", "I" ,"O" ,"P",
  "A", "S", "D", "F", "G", "H", "J", "K", "L",
  "Z", "X", "C", "V", "B", "N", "M",
};

static char* KEYPAD_NUM_LABEL[] = {
  // Special buttons
  "<", " ", "ESC", "ENT","ABC",
  // Basic buttons
  "0", "1", "2", "3", "4", "5", "6", "7" ,"8" ,"9",
  "+", "-", "=", "%", "'", "&", "(", ")", ".",
  "@", "#", "$", "/", "!", "?", ",",
};

static gslc_tsKey KEYPAD_KEY_POS[] = {
  { KEYPAD_ID_TXT,           0, 2, 6 }, 
  // Special buttons
  { KEYPAD_ID_BACKSPACE,     2, 9, 1 }, 
  { KEYPAD_ID_SPACE,         3, 9, 1 }, 
  { KEYPAD_ID_ESC,           0, 0, 2 }, 
  { KEYPAD_ID_ENTER,         0, 8, 2 }, 
  { KEYPAD_ID_SWAP_PAD,      3, 0, 2 }, 
  // Basic buttons
  { KEYPAD_ID_BASIC_START,   1, 0, 1 },  // "Q" - "0"
  { KEYPAD_ID_BASIC_START+1 ,1, 1, 1 },  // "W" - "1"
  { KEYPAD_ID_BASIC_START+2 ,1, 2, 1 },  // "E" - "2"
  { KEYPAD_ID_BASIC_START+3 ,1, 3, 1 },  // "R" - "3"
  { KEYPAD_ID_BASIC_START+4 ,1, 4, 1 },  // "T" - "4"
  { KEYPAD_ID_BASIC_START+5 ,1, 5, 1 },  // "Y" - "5"
  { KEYPAD_ID_BASIC_START+6 ,1, 6, 1 },  // "U" - "6"
  { KEYPAD_ID_BASIC_START+7 ,1, 7, 1 },  // "I" - "7"
  { KEYPAD_ID_BASIC_START+8 ,1, 8, 1 },  // "O" - "8"
  { KEYPAD_ID_BASIC_START+9 ,1, 9, 1 },  // "P" - "9"
  { KEYPAD_ID_BASIC_START+10,2, 0, 1 },  // "A" - "+"
  { KEYPAD_ID_BASIC_START+11,2, 1, 1 },  // "S" - "-"
  { KEYPAD_ID_BASIC_START+12,2, 2, 1 },  // "D" - "=" 
  { KEYPAD_ID_BASIC_START+13,2, 3, 1 },  // "F" - "%"
  { KEYPAD_ID_BASIC_START+14,2, 4, 1 },  // "G" - "'"
  { KEYPAD_ID_BASIC_START+15,2, 5, 1 },  // "H" - "&'
  { KEYPAD_ID_BASIC_START+16,2, 6, 1 },  // "J" - "("
  { KEYPAD_ID_BASIC_START+17,2, 7, 1 },  // "K" - ")"
  { KEYPAD_ID_BASIC_START+18,2, 8, 1 },  // "L" - "."
  { KEYPAD_ID_BASIC_START+19,3, 2, 1 },  // "Z" - "@"
  { KEYPAD_ID_BASIC_START+20,3, 3, 1 },  // "X" - "#"
  { KEYPAD_ID_BASIC_START+21,3, 4, 1 },  // "C" - "$"
  { KEYPAD_ID_BASIC_START+22,3, 5, 1 },  // "V" - "/"
  { KEYPAD_ID_BASIC_START+23,3, 6, 1 },  // "B" - "!"
  { KEYPAD_ID_BASIC_START+24,3, 7, 1 },  // "N" - "?"
  { KEYPAD_ID_BASIC_START+25,3, 8, 1 },  // "M" - ","
};

// Reset the XKeyPad config struct
// - This must be called before any XKeyPad config update APIs are called
gslc_tsXKeyPadCfg gslc_ElemXKeyPadCfgInit_Alpha()
{
  gslc_tsXKeyPadCfg sConfig;
  sConfig.nButtonSzW    = 25;
  sConfig.nButtonSzH    = 25;
  sConfig.bFloatEn      = false; // Unused
  sConfig.bSignEn       = false; // Unused
  sConfig.bRoundEn      = false;
  sConfig.nFontId       = GSLC_FONT_NONE; // Will be overwritten
  sConfig.pActiveLabels = KEYPAD_ALPHA_LABEL;
  sConfig.pLabels1      = KEYPAD_ALPHA_LABEL;
  sConfig.pLabels2      = KEYPAD_NUM_LABEL;
  sConfig.pKeyPos       = KEYPAD_KEY_POS;
  sConfig.nSpecialKeys  = XKEYPADALPHA_BTN_SPECIAL;
  sConfig.nBasicKeys    = XKEYPADALPHA_BTN_BASIC;
  sConfig.nMaxKeys      = XKEYPADALPHA_ELEM_MAX;
  sConfig.nMaxCols      = 10;
  sConfig.nMaxRows      = 4;
  sConfig.nFrameMargin  = 2;
  return sConfig;
}

// Create the XKeyPad_Num element
gslc_tsElemRef* gslc_ElemXKeyPadCreate_Alpha(gslc_tsGui* pGui, int16_t nElemId, int16_t nPage,
  gslc_tsXKeyPad_Alpha* pXData, int16_t nX0, int16_t nY0, int8_t nFontId, gslc_tsXKeyPadCfg* pConfig)
{

  // Provide default config if none supplied
  gslc_tsXKeyPadCfg sConfigTmp;
  if (pConfig == NULL) {
    sConfigTmp = gslc_ElemXKeyPadCfgInit_Alpha();
    pConfig = &sConfigTmp;
  }
  
  return gslc_XKeyPadCreateBase(pGui, nElemId, nPage, (void*)pXData, nX0, nY0, nFontId, pConfig);
}


// ============================================================================
