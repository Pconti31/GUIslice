#ifndef _GUISLICE_EX_XCOMBOBOX_H_
#define _GUISLICE_EX_XCOMBOBOX_H_

#include "GUIslice.h"


// =======================================================================
// GUIslice library extension: Combobox control
// - Calvin Hass Paul Conti
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
/// \file XCombobox.h

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus



#include "elem/XListbox.h"

// ============================================================================
// Extended Element: Combobox
// - This is a complex but not compound element containing one text button
//   with storage that displays the current value and is used to expand
//   an overlay page with one element. A Listbox that represents the choices.
// - NOTE: The XCombobox element is in beta development.
//         Therefore, its API is subject to change.
// ============================================================================

// Define unique identifier for extended element type
// - Select any number above GSLC_TYPE_BASE_EXTEND
#define  GSLC_TYPEX_COMBOBOX GSLC_TYPE_BASE_EXTEND + 11

// Define constants specific to the control
#define XCOMBOBOX_SEL_NONE       -1  // Indicator for "no selection"
#define XCOMBOBOX_SIZE_AUTO      -1  // Indicator for "auto-size"
#define XCOMBOBOX_BUF_OH_R        2  // Combobox buffer overhead per row

// Define the max number of sub-elements in compound element
#define XCOMBOBOX_COMP_CNT 2

// Defines the maximum length of a combobox item
#define XCOMBOBOX_MAX_STR        20

// Define the status for GSLC_CB_INPUT callback
#define XCOMBOBOX_CB_STATE_UPDATE 3

/// Callback function for Combobox feedback
typedef bool (*GSLC_CB_XCOMBOBOX_SEL)(void* pvGui,void* pvElem,int16_t nSel);

// Extended element data structures
// - These data structures are maintained in the gslc_tsElem
//   structure via the pXData pointer

/// Extended data for Combobox element
typedef struct {

  // Style - helps us to work out listbox sizing
  int8_t                nButtonSz;       ///< Expand button size

  // Callbacks
  gslc_tsElemRef*       pListElemRef;    ///< Save our ElemRef for communication with out listbox
  GSLC_CB_TOUCH         pfunctUser;      ///< User's Callback event to say element has changed

  // Provide storage
  char                 acButton[2];      ///< button arrow character
  char                 acElemTxt[XCOMBOBOX_MAX_STR+1];  ///< Storage for expand button's text
  gslc_tsXListbox      sListbox;         ///< Storage for listbox XData
  gslc_tsPage          asOverlayPage;    ///< Storage for overlay 
  gslc_tsElem          asPageElem[1];    ///< Storage for overlay elems (listbox)
  gslc_tsElemRef       asPageElemRef[1]; ///< Storage for overlay elemrefs (listbox)
} gslc_tsXCombobox;


///
/// Create a Combobox Element
///
/// \param[in]  pGui:          Pointer to GUI
/// \param[in]  nElemId:       Element ID to assign (0..16383 or GSLC_ID_AUTO to autogen)
/// \param[in]  nPage:         Page ID to attach element to
/// \param[in]  pXData:        Ptr to extended element data structure
/// \param[in]  rElem:         Rectangle coordinates defining button size
/// \param[in]  nFontId:       Font ID for item display
/// \param[in]  nMaxItems:     Maximum number of items in combobox
/// \param[in]  pBufItems:     Pointer to buffer that will contain list of items
/// \param[in]  nBufItemsMax:  Max size of buffer for list of items (pBufItems)
/// \param[in]  nButtonSz:     Size of expand button
/// \param[in]  nSelDefault:   Default item to select
/// \param[in]  nListId:       Returned ID for list callback
/// \param[in]  funcCb:        Function pointer to selection routine (or NULL for none)
///
/// \return Pointer to Element reference or NULL if failure
///
gslc_tsElemRef* gslc_ElemXComboboxCreate(gslc_tsGui* pGui, int16_t nElemId, int16_t nPage,
  gslc_tsXCombobox* pXData, gslc_tsRect rElem, int16_t nFontId, int8_t nMaxItems,
  uint8_t* pBufItems, uint16_t nBufItemsMax, int8_t nButtonSz, int16_t nSelDefault,int16_t* nListId,
  GSLC_CB_TOUCH funcCb);


///
/// Configure the margin inside the combobox
/// - Defines the region bewteen the element rect and the inner combobox items
///
/// \param[in]  pGui:          Pointer to GUI
/// \param[in]  pElemRef:      Ptr to Element Reference to update
/// \param[in]  nMarginW:      Set the margin (horizontal) inside the combobox (0 for none)
/// \param[in]  nMarginH:      Set the margin (horizontal) inside the combobox (0 for none)
///
/// \return none
///
void gslc_ElemXComboboxSetMargin(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int8_t nMarginW, int8_t nMarginH);

///
/// Configure the gap between combobox items
///
/// \param[in]  pGui:          Pointer to GUI
/// \param[in]  pElemRef:      Ptr to Element Reference to update
/// \param[in]  nGap:          Set the gap between combobox items (0 for none)
/// \param[in]  colGap:        Set the color of the gap between combobox items
///
/// \return none
///
void gslc_ElemXComboboxItemsSetGap(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int8_t nGap, gslc_tsColor colGap);


///
/// Empty the combobox of all items
///
/// \param[in]  pGui:          Pointer to GUI
/// \param[in]  pElemRef:      Ptr to Element Reference to update
///
/// \return none
///
void gslc_ElemXComboboxReset(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef);


///
/// Add an item to the combobox
///
/// \param[in]  pGui:          Pointer to GUI
/// \param[in]  pElemRef:      Ptr to Element Reference to update
/// \param[in]  pStrItem:      String to use when creating the combobox item
///
/// \return true if OK, false if fail (eg. insufficient buffer storage)
///
bool gslc_ElemXComboboxAddItem(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, const char* pStrItem);

///
/// Get the indexed combobox item
///
/// \param[in]  pGui:          Pointer to GUI
/// \param[in]  pElemRef:      Ptr to Element Reference to update
/// \param[in]  nItemCurSel:      Item index to fetch
/// \param[out] pStrItem:      Ptr to the string buffer to receive the item
/// \param[in]  nStrItemLen:   Maximum buffer length of pStrItem
///
/// \return true if success, false if fail (eg. can't locate item)
///
bool gslc_ElemXComboboxGetItem(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int16_t nItemCurSel, char* pStrItem, uint8_t nStrItemLen);


///
/// Get the currently selected combobox item
///
/// \param[in]  pGui:          Pointer to GUI
/// \param[in]  pElemRef:      Ptr to Element Reference to update
/// \param[out] pStrItem:      Ptr to the string buffer to receive the item
/// \param[in]  nStrItemLen:   Maximum buffer length of pStrItem
///
/// \return true if success, false if fail (eg. can't locate item)
///
bool gslc_ElemXComboboxGetSelItem(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, char* pStrItem, uint8_t nStrItemLen);


///
/// Get the number of items in the combobox
///
/// \param[in]  pGui:          Pointer to GUI
/// \param[in]  pElemRef:      Ptr to Element Reference to update
///
/// \return Number of items
///
int16_t gslc_ElemXComboboxGetItemCnt(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef);

///
/// Draw a Combobox compound element on the screen
/// that includes a Text Field and an Expand Button
/// - Called from gslc_ElemDraw()
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
/// \param[in]  pvElemRef:   Void ptr to Element (typecast to gslc_tsElemRef*)
/// \param[in]  eRedraw:     Redraw mode
///
/// \return true if success, false otherwise
///
bool gslc_ElemXComboboxDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw);

///
/// Draw a Combobox list on the screen
/// - Called from gslc_ElemDraw()
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
/// \param[in]  pvElemRef:   Void ptr to Element (typecast to gslc_tsElemRef*)
/// \param[in]  eRedraw:     Redraw mode
///
/// \return true if success, false otherwise
///
bool gslc_ElemXComboboxDrawList(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw);

///
/// Handle Combobox Button callback
/// - This is called internally by the Combobox touch handler
/// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
/// \param[in]  pvElemRef    Void ptr to Element ref (typecast to gslc_tsElemRef*)
/// \param[in]  eTouch:      Touch event type
/// \param[in]  nX:          Touch X coord
/// \param[in]  nY:          Touch Y coord
///
/// \return bool 
///
bool gslc_ElemXComboboxButtonClick(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY);

///
/// Get a Combobox element's current selection
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
///
/// \return Current Combobox selection (or -1 if none)
///
int16_t gslc_ElemXComboboxGetSel(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef);

///
/// Handle Combobox Listbox callback
/// - This is called internally by the Listbox touch handler
/// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
/// \param[in]  pvElemRef    Void ptr to Element ref (typecast to gslc_tsElemRef*)
/// \param[in]  nSelId:      Index of seleted item or -1 for none
///
/// \return bool 
///
bool gslc_ElemXComboboxListClick(void* pvGui, void* pvElemRef, int16_t nSelId);


///
/// Set a Combobox element's current selection
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  nItemCurSel:    Combobox item to select (or -1 for none)
///
/// \return true if success, false if fail
///
bool gslc_ElemXComboboxSetSel(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int16_t nItemCurSel);

///
/// Set the Combobox scroll position
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  nScrollPos:  Scroll the combobox so that the nScrollPos item is at the top (0 default)
///
/// \return true if success, false if fail
///
//bool gslc_ElemXComboboxSetScrollPos(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, uint16_t nScrollPos);


// ============================================================================

// ------------------------------------------------------------------------
// Read-only element macros
// ------------------------------------------------------------------------

// Macro initializers for Read-Only Elements in Flash/PROGMEM
//



#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_EX_XCOMBOBOX_H_

