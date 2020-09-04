// =======================================================================
// GUIslice library (extensions)
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
/// \file XCombobox.c



// GUIslice library
#include "GUIslice.h"
#include "GUIslice_drv.h"

#include "elem/XListbox.h"
#include "elem/XCombobox.h"

#include <stdio.h>

#if (GSLC_USE_PROGMEM)
    #include <avr/pgmspace.h>
#endif

// ============================================================================
// Extended Element: Combobox
// - This is a complex but not compound element containing one text button
//   with storage that displays the current value and is used to expand
//   an overlay page with one element. A Listbox that represents the choices.
// ============================================================================

// Private sub Element ID definitions
static const int16_t  COMBOBOX_ID_BTN     = 100;
static const int16_t  COMBOBOX_ID_LIST    = 101;

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

// ----------------------------------------------------------------------------

// ============================================================================
// Extended Compound Element: Combobox
// - A Combobox control
// ============================================================================

void gslc_ElemXComboboxSetMargin(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int8_t nMarginW, int8_t nMarginH)
{
  gslc_tsXCombobox* pCombobox = (gslc_tsXCombobox*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_COMBOBOX, __LINE__);
  if (!pCombobox) return;
  gslc_tsElem*      pElem = gslc_GetElemFromRef(pGui,pElemRef);

  // pass-through to our listbox
  gslc_ElemXListboxSetMargin(pGui,pCombobox->pListElemRef,nMarginW,nMarginH);
  
}

void gslc_ElemXComboboxItemsSetGap(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int8_t nGap, gslc_tsColor colGap)
{
  gslc_tsXCombobox* pCombobox = (gslc_tsXCombobox*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_COMBOBOX, __LINE__);
  if (!pCombobox) return;
  gslc_tsElem*      pElem = gslc_GetElemFromRef(pGui,pElemRef);

  // pass-through to our listbox
  gslc_ElemXListboxItemsSetGap(pGui,pCombobox->pListElemRef,nGap,colGap);
  
}

void gslc_ElemXComboboxReset(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef)
{
  gslc_tsXCombobox* pCombobox = (gslc_tsXCombobox*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_COMBOBOX, __LINE__);
  if (!pCombobox) return;
  gslc_tsElem*      pElem = gslc_GetElemFromRef(pGui,pElemRef);

  // pass-through to our listbox
  gslc_ElemXListboxReset(pGui,pCombobox->pListElemRef);
  
}

bool gslc_ElemXComboboxAddItem(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, const char* pStrItem)
{
  gslc_tsXCombobox* pCombobox = (gslc_tsXCombobox*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_COMBOBOX, __LINE__);
  if (!pCombobox) return false;

  // Test string length and truncate if necessary 
  char acTxt[XCOMBOBOX_MAX_STR + 1];
  strncpy(acTxt,pStrItem,XCOMBOBOX_MAX_STR);
  acTxt[XCOMBOBOX_MAX_STR] = '\0';
  
  // pass-through to our listbox
  return gslc_ElemXListboxAddItem(pGui,pCombobox->pListElemRef, acTxt);

}


// Create a Combobox compound element
// - Defines default styling for the element
// - Defines callback for redraw and touch
gslc_tsElemRef* gslc_ElemXComboboxCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsXCombobox* pXData,gslc_tsRect rElem,int16_t nFontId,  int8_t nMaxItems,
  uint8_t* pBufItems,uint16_t nBufItemsMax, int8_t nButtonSz,int16_t nItemDefault, int16_t* nListId,
  GSLC_CB_TOUCH funcCb)
{
  if ((pGui == NULL) || (pXData == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXComboboxCreate";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return NULL;
  }
  gslc_tsElem     sElem;
  gslc_tsElemRef* pElemRef = NULL;
  
  // Initialize composite element
  sElem = gslc_ElemCreate(pGui,nElemId,nPage,GSLC_TYPEX_COMBOBOX,rElem,NULL,0,nFontId);
  
  sElem.pXData = (void*)(pXData);
  sElem.nFeatures        |= ~GSLC_ELEM_FEA_FRAME_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_FILL_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_CLICK_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_GLOW_EN;

  sElem.nGroup            = GSLC_GROUP_ID_NONE;
  pXData->nButtonSz       = nButtonSz;
  pXData->pfunctUser      = funcCb; // save user's callback in our extra data
  strcpy(pXData->acButton,"+");     // expand down arrow

  sElem.pXData            = (void*)(pXData);
  // Specify the custom drawing callback
  sElem.pfuncXDraw        = &gslc_ElemXComboboxDraw;
  // Specify the custom touch tracking callback
  sElem.pfuncXTouch       = &gslc_ElemXComboboxButtonClick;

  sElem.colElemFill       = GSLC_COL_BLACK;
  sElem.colElemFillGlow   = GSLC_COL_BLACK;
  sElem.colElemFrame      = GSLC_COL_BLUE;
  sElem.colElemFrameGlow  = GSLC_COL_BLUE;
  sElem.colElemText       = GSLC_COL_YELLOW;
  sElem.colElemTextGlow   = GSLC_COL_YELLOW;

  // Set default text alignment:
  // - Vertical center, left justify
  sElem.eTxtAlign        = GSLC_ALIGN_MID_LEFT;

  // - The element IDs assigned to the sub-elements are
  //   arbitrary (with local scope in the compound element),
  //   so they don't need to be unique globally across the GUI.
  // Create dynamic listbox overlay
  gslc_tsPage* pPage = &(pXData->asOverlayPage);

  gslc_OverlayAdd(pGui,pPage,pXData->asPageElem,1,pXData->asPageElemRef,1);

  // Calculate size of its rectangle enclosure
  gslc_tsRect rList;
  rList.x = rElem.x+nButtonSz;
  rList.y = rElem.y+rElem.h;
  rList.w = rElem.w-nButtonSz;
  rList.h = rElem.h*nMaxItems;

  pXData->pListElemRef = gslc_ElemXListboxCreate(pGui,GSLC_ID_AUTO,GSLC_PAGE_OVERLAY,
    &(pXData->sListbox),rList,nFontId,pBufItems,nBufItemsMax,nItemDefault);
  gslc_ElemSetTxtMarginXY(pGui, pXData->pListElemRef, 5, 0);
  gslc_ElemXListboxSetSize(pGui, pXData->pListElemRef, nMaxItems, 1); // nMaxItems rows, 1 columns
  gslc_ElemXListboxItemsSetSize(pGui, pXData->pListElemRef, XLISTBOX_SIZE_AUTO, XLISTBOX_SIZE_AUTO);
//TODO add colors to API
  gslc_ElemSetCol(pGui,pXData->pListElemRef,GSLC_COL_BLUE,GSLC_COL_GRAY_DK3,GSLC_COL_GRAY_DK2);
  gslc_ElemSetTxtCol(pGui,pXData->pListElemRef,GSLC_COL_WHITE);
  gslc_ElemSetFrameEn(pGui,pXData->pListElemRef,true);
  gslc_ElemXListboxSetSelFunc(pGui, pXData->pListElemRef, &gslc_ElemXComboboxListClick);

  // TODO need API call to reset the overlay page to NULL when we finish
  pGui->asOverlayPage = NULL;

  if (nPage != GSLC_PAGE_NONE) {
    pElemRef = gslc_ElemAdd(pGui,nPage,&sElem,GSLC_ELEMREF_DEFAULT);
    gslc_tsElem* pElemList = gslc_GetElemFromRef(pGui,pXData->pListElemRef);
    pElemList->pElemRefParent = pElemRef;
    return pElemRef;
  }
  
  return NULL;
}


// Redraw the Combobox
// - Note that this redraw is for the entire element rect region
// - The Draw function parameters use void pointers to allow for
//   simpler callback function definition & scalability.
bool gslc_ElemXComboboxDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw)
{
  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*       pGui  = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef*   pElemRef = (gslc_tsElemRef*)(pvElemRef);

  gslc_tsXCombobox* pCombobox = (gslc_tsXCombobox*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_COMBOBOX, __LINE__);
  if (!pCombobox) return false;

  gslc_tsElem*      pElem = gslc_GetElemFromRef(pGui,pElemRef);

  bool bGlow = (pElem->nFeatures & GSLC_ELEM_FEA_GLOW_EN) && gslc_ElemGetGlow(pGui,pElemRef);

  // Draw the compound element fill (background)
  // - Should only need to do this in full redraw
  if (eRedraw == GSLC_REDRAW_FULL) {
    gslc_DrawFillRect(pGui,pElem->rElem,(bGlow)?pElem->colElemFillGlow:pElem->colElemFill);
  }

  // Draw the sub-elements
  // - Only redraw expand text button 
  //   unless user has clicked it then draw the listbox.
  
  // custom draw the expand button so we can add the down arrow

  int16_t   nElemX,nElemY;
  uint16_t  nElemW,nElemH;

  nElemX    = pElem->rElem.x;
  nElemY    = pElem->rElem.y;
  nElemW    = pElem->rElem.w;
  nElemH    = pElem->rElem.h;
  
  // - Assume both fill and frame are enabled so contract
  //   the fill region slightly so that we don't overdraw
  //   the frame (prevent unnecessary flicker).
  gslc_tsRect rElemInner = gslc_ExpandRect(pElem->rElem,-1,-1);
//TODO  add colors
  gslc_tsColor colBg = pElem->colElemFill;

  gslc_DrawFillRect(pGui, rElemInner, colBg);

  // --------------------------------------------------------------------------
  // Text overlays
  // --------------------------------------------------------------------------

  // Draw text string but first get the current selection if any
  // and add the down arrow to it.  

  int16_t nSelId = gslc_ElemXListboxGetSel(pGui,pCombobox->pListElemRef);
  
  if (nSelId != XLISTBOX_SEL_NONE) {
    gslc_ElemXListboxGetItem(pGui,pCombobox->pListElemRef,nSelId,pCombobox->acElemTxt,sizeof(pCombobox->acElemTxt));
  } else {
    gslc_ElemXListboxGetItem(pGui,pCombobox->pListElemRef,0,pCombobox->acElemTxt,sizeof(pCombobox->acElemTxt));
  }
  
  gslc_tsRect   rText = (gslc_tsRect) { nElemX, nElemY, nElemW - (2*pCombobox->nButtonSz), nElemH };
  gslc_tsColor  colTxt    = pElem->colElemText;
//  gslc_tsColor  colTxt    = GSLC_COL_WHITE;
   int8_t        nMarginX  = pElem->nTxtMarginX;
  int8_t        nMarginY  = pElem->nTxtMarginY;

  // Set the text flags to indicate that the user has separately
  // allocated memory for the text strings.
  gslc_teTxtFlags eTxtFlags = GSLC_TXT_MEM_RAM | GSLC_TXT_ALLOC_EXT;

  // Draw the aligned text string (by default it is GSLC_ALIGN_MID_LEFT)
  gslc_DrawTxtBase(pGui, pCombobox->acElemTxt, rText, pElem->pTxtFont, eTxtFlags,
    pElem->eTxtAlign,colTxt,colBg,nMarginX,nMarginY);
      
  // output the pull down arrow
  gslc_tsRect   rArrow = (gslc_tsRect) { nElemX+rText.w, nElemY, 2*pCombobox->nButtonSz, nElemH };
  gslc_tsColor  colArrow  = GSLC_COL_WHITE;
  colBg = GSLC_COL_BLUE;

  gslc_DrawFillRect(pGui, rArrow, colBg);

  gslc_DrawTxtBase(pGui, pCombobox->acButton, rArrow, pElem->pTxtFont, eTxtFlags, 
      GSLC_ALIGN_MID_LEFT,colArrow,colBg,nMarginX,nMarginY);

  // Frame the button
  gslc_DrawFrameRect(pGui, pElem->rElem, pElem->colElemFrame);

  // finally, draw frame
  if (eRedraw == GSLC_REDRAW_FULL) {
    gslc_DrawFrameRect(pGui, pElem->rElem, (bGlow) ? pElem->colElemFrameGlow : pElem->colElemFrame);
  }

  // Clear the redraw flag
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_NONE);

  // Mark page as needing flip
  gslc_PageFlipSet(pGui,true);

  return true;
}

bool gslc_ElemXComboboxButtonClick(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
#if defined(DRV_TOUCH_NONE)
  return false;
#else

  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return false;

  gslc_tsXCombobox* pCombobox = (gslc_tsXCombobox*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_COMBOBOX, __LINE__);
  if (pCombobox == NULL) {
    GSLC_DEBUG2_PRINT("ERROR: gslc_ElemXComboboxButtonClick() element (ID=%d) has NULL pXData\n",pElem->nId);
    return false;
  }

  switch(eTouch) {

    case GSLC_TOUCH_UP_IN:
      gslc_OverlayShow(pGui, &(pCombobox->asOverlayPage), true);
      break;
    default:
      return false;
      break;
  }

  return true;
#endif // !DRV_TOUCH_NONE
}

bool gslc_ElemXComboboxListClick(void* pvGui, void* pvElemRef, int16_t nSelId)
{
#if defined(DRV_TOUCH_NONE)
  return false;
#else
  gslc_tsGui*     pGui = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem = gslc_GetElemFromRef(pGui, pElemRef);
  char            acTxt[XCOMBOBOX_MAX_STR+1];
  
  if (pElemRef == NULL) {
    return false;
  }
  
  // Fetch the parent of the clicked element which is the compound
  // element itself. This enables us to access the extra control data.
  gslc_tsElemRef*    pElemRefParent = pElem->pElemRefParent;
  if (pElemRefParent == NULL) {
    GSLC_DEBUG2_PRINT("ERROR: gslc_ElemXComboboxButtonClick(%s) parent ElemRef ptr NULL\n","");
    return false;
  }

  gslc_tsElem*    pElemParent = gslc_GetElemFromRef(pGui,pElemRefParent);
  gslc_tsXCombobox* pCombobox     = (gslc_tsXCombobox*)(pElemParent->pXData);
  if (pCombobox == NULL) {
    GSLC_DEBUG2_PRINT("ERROR: gslc_ElemXComboboxButtonClick() element (ID=%d) has NULL pXData\n",pElem->nId);
    return false;
  }

  // We only have one listbox so no need to check pElem_nId
  // Update the status message with the selection
  if (nSelId != XLISTBOX_SEL_NONE) {
    gslc_OverlayHide(pGui);
    gslc_ElemSetRedraw(pGui,pElemRefParent,GSLC_REDRAW_INC);
    // turn off click for listbox and on for expand button
    gslc_ElemSetRedraw(pGui, pElemRefParent, GSLC_REDRAW_INC);
    // Invoke the callback function
    if (pCombobox->pfunctUser != NULL) {
      gslc_teTouch eTouch = GSLC_TOUCH_UP_IN;
      (*pCombobox->pfunctUser)((void*)(pGui), (void*)(pElemRefParent), eTouch, 0, 0);
    }
  }
  return true;
#endif // !DRV_TOUCH_NONE
}

bool gslc_ElemXComboboxGetItem(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int16_t nItemCurSel, char* pStrItem, uint8_t nStrItemLen)
{
  gslc_tsXCombobox* pCombobox = (gslc_tsXCombobox*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_COMBOBOX, __LINE__);
  if (!pCombobox) return false;

  // pass-through to our listbox
  return gslc_ElemXListboxGetItem(pGui,pCombobox->pListElemRef, nItemCurSel, pStrItem, nStrItemLen);

}

bool gslc_ElemXComboboxGetSelItem(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, char* pStrItem, uint8_t nStrItemLen)
{
  gslc_tsXCombobox* pCombobox = (gslc_tsXCombobox*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_COMBOBOX, __LINE__);
  if (!pCombobox) return false;

  // pass-through to our listbox
  int16_t nSelId = gslc_ElemXListboxGetSel(pGui,pCombobox->pListElemRef);
  if (nSelId == XLISTBOX_SEL_NONE) nSelId = 0;

  return gslc_ElemXListboxGetItem(pGui,pCombobox->pListElemRef, nSelId, pStrItem, nStrItemLen);

}

int16_t gslc_ElemXComboboxGetItemCnt(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef)
{
  gslc_tsXCombobox* pCombobox = (gslc_tsXCombobox*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_COMBOBOX, __LINE__);
  if (!pCombobox) return 0;

  // pass-through to our listbox
  return gslc_ElemXListboxGetItemCnt(pGui,pCombobox->pListElemRef);

}

int16_t gslc_ElemXComboboxGetSel(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef)
{
  gslc_tsXCombobox* pCombobox = (gslc_tsXCombobox*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_COMBOBOX, __LINE__);
  if (!pCombobox) return XCOMBOBOX_SEL_NONE;

  // pass-through to our listbox
  return gslc_ElemXListboxGetSel(pGui,pCombobox->pListElemRef);
}

bool gslc_ElemXComboboxSetSel(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef, int16_t nItemCurSel)
{
  gslc_tsXCombobox* pCombobox = (gslc_tsXCombobox*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_COMBOBOX, __LINE__);
  if (!pCombobox) return false;

  // pass-through to our listbox
  return gslc_ElemXListboxSetSel(pGui,pCombobox->pListElemRef,nItemCurSel);
}

/* UNIMPLEMENTED 
bool gslc_ElemXComboboxSetScrollPos(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, uint16_t nScrollPos)
{
  gslc_tsXCombobox* pCombobox = (gslc_tsXCombobox*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_COMBOBOX, __LINE__);
  if (!pCombobox) return false;

  bool bOk = false;
  if ((nScrollPos >= 0) && (nScrollPos < pCombobox->nItemCnt)) {
    bOk = true;
  }

  int16_t nCols = pCombobox->nCols;
  int16_t nItemCnt = pCombobox->nItemCnt;

  // Error handling: in case position out of bounds
  if (nScrollPos >= nItemCnt) {
    nScrollPos = (nItemCnt > 0) ? nItemCnt - 1 : 0;
  }

  // Adjust the top item index to the start of its row
  // - This is done because we may have multiple columns
  //   per row. This ensures nItemTop points to the list
  //   index at the start of a row.
  nScrollPos = (nScrollPos / nCols) * nCols;
  pCombobox->nItemTop = nScrollPos;

  // Need to update all rows in display
  gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_FULL);
  return bOk;
}
*/

// ============================================================================
