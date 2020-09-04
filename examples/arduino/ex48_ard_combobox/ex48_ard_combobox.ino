// FILE: [ex48_ard_combobox.ino]
//
// GUIslice Library Examples
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - Example 48 (Arduino): Combobox
//   - Demonstrates Combobox control
//   - Shows callback notifications for Combobox
//   - Provide example of additional Adafruit-GFX fonts
//     (see USE_EXTRA_FONTS)
//   - NOTE: This is the simple version of the example without
//     optimizing for memory consumption. Therefore, it may not
//     run on Arduino devices with limited memory. A "minimal"
//     version is located in the "arduino_min" folder which includes
//     FLASH memory optimization for reduced memory devices.
//
// ARDUINO NOTES:
// - GUIslice_config.h must be edited to match the pinout connections
//   between the Arduino CPU and the display controller (see ADAGFX_PIN_*).
//

// ------------------------------------------------
// Headers to include
// ------------------------------------------------
#include "GUIslice.h"
#include "GUIslice_drv.h"

// Include extended elements
#include "elem/XCombobox.h"

// ------------------------------------------------
// Headers and Defines for fonts
// Note that font files are located within the Adafruit-GFX library folder:
// ------------------------------------------------
#if defined(DRV_DISP_TFT_ESPI)
  #error Builder config "Edit->Options->General->Target Platform" should be "arduino TFT_eSPI"
#endif 
#include <Adafruit_GFX.h>
// Note that these files are located within the Adafruit-GFX library folder:
#include "Fonts/FreeMono9pt7b.h"

// ------------------------------------------------
// Defines for resources
// ------------------------------------------------

// ------------------------------------------------
// Enumerations for pages, elements, fonts, images
// ------------------------------------------------
enum {E_PG_MAIN};
enum {E_ELEM_BTN_QUIT,E_ELEM_COMBOBOX};
// Must use separate enum for fonts with MAX_FONT at end to use gslc_FontSet.
enum {E_FREE_MONO9,MAX_FONT};

// ------------------------------------------------
// Instantiate the GUI
// ------------------------------------------------

// ------------------------------------------------
// Define the maximum number of elements and pages
// ------------------------------------------------
#define MAX_PAGE                1

#define MAX_ELEM_PG_MAIN 2 // # Elems total on page
#define MAX_ELEM_PG_MAIN_RAM MAX_ELEM_PG_MAIN // # Elems in RAM

// ------------------------------------------------
// Create element storage
// ------------------------------------------------
gslc_tsGui                      m_gui;
gslc_tsDriver                   m_drv;
gslc_tsFont                     m_asFont[MAX_FONT];
gslc_tsPage                     m_asPage[MAX_PAGE];

gslc_tsElem                     m_asPage1Elem[MAX_ELEM_PG_MAIN_RAM];
gslc_tsElemRef                  m_asPage1ElemRef[MAX_ELEM_PG_MAIN];
gslc_tsXCombobox                m_sCombobox1;
// - Note that XCOMBOBOX_BUF_OH_R is extra required per item
char                            m_acComboboxBuf1[32 + XCOMBOBOX_BUF_OH_R];

#define MAX_STR                 20

// ------------------------------------------------
// Program Globals
// ------------------------------------------------
int16_t    m_nComboboxId;

// Save some element references for direct access
//<Save_References !Start!>
gslc_tsElemRef* m_pElemCombobox1   = NULL;

// Define debug message function
static int16_t DebugOut(char ch) { if (ch == (char)'\n') Serial.println(""); else Serial.write(ch); return 0; }

// ------------------------------------------------
// Callback Methods
// ------------------------------------------------
// Common Button callback
bool CbBtnCommon(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*     pGui     = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem    = gslc_GetElemFromRef(pGui,pElemRef);

  char    acTxt[MAX_STR+1];
  
  if ( eTouch == GSLC_TOUCH_UP_IN ) {
    // From the element's ID we can determine which button was pressed.
    switch (pElem->nId) {
      case E_ELEM_BTN_QUIT:
        Serial.println("QUIT");
        break;
    case E_ELEM_COMBOBOX:
      gslc_ElemXComboboxGetSelItem(&m_gui, m_pElemCombobox1, acTxt, MAX_STR);
      Serial.print("selected: "); Serial.println(acTxt);
      break;
      default:
        break;
    }
  }
  return true;
}

// ------------------------------------------------
// Create page elements
// ------------------------------------------------
bool InitGUI()
{
  gslc_tsElemRef* pElemRef = NULL;

  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPage1Elem,MAX_ELEM_PG_MAIN_RAM,m_asPage1ElemRef,MAX_ELEM_PG_MAIN);

  // NOTE: The current page defaults to the first page added. Here we explicitly
  //       ensure that the main page is the correct page no matter the add order.
  gslc_SetPageCur(&m_gui,E_PG_MAIN);
  
  // Set Background to a flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_BLACK);
//  gslc_SetBkgndColor(&m_gui,GSLC_COL_GRAY_LT3);

  // -----------------------------------
  // PAGE: E_PG_MAIN
  
   
  // Create box
  pElemRef = gslc_ElemXComboboxCreate(&m_gui,E_ELEM_COMBOBOX,E_PG_MAIN,&m_sCombobox1,
    (gslc_tsRect){48,54,160,30},E_FREE_MONO9,4,
    (uint8_t*)&m_acComboboxBuf1,sizeof(m_acComboboxBuf1),15,0,&m_nComboboxId,&CbBtnCommon);
//  gslc_ElemXComboboxSetSize(&m_gui, pElemRef, 5, 1); // 5 rows, 1 columns
//  gslc_ElemXComboboxItemsSetSize(&m_gui, pElemRef, XCOMBOBOX_SIZE_AUTO, XCOMBOBOX_SIZE_AUTO);
  gslc_ElemSetTxtMarginXY(&m_gui, pElemRef, 5, 0);
//  gslc_ElemXComboboxSetMargin(&m_gui, pElemRef,5,5);
//  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
//  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLUE,GSLC_COL_BLACK,GSLC_COL_BLACK);
  gslc_ElemXComboboxAddItem(&m_gui, pElemRef, "DEBIAN");
  gslc_ElemXComboboxAddItem(&m_gui, pElemRef, "FEDORA");
  gslc_ElemXComboboxAddItem(&m_gui, pElemRef, "LINUX MINT");
  gslc_ElemXComboboxAddItem(&m_gui, pElemRef, "UBUNTU");
  gslc_ElemSetFrameEn(&m_gui,pElemRef,true);
  m_pElemCombobox1 = pElemRef;
  
  // create E_ELEM_BTN_QUIT button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,
    (gslc_tsRect){48,90,50,50},(char*)"QUIT",0,E_FREE_MONO9,&CbBtnCommon);

  return true;
}

void setup()
{
  // ------------------------------------------------
  // Initialize
  // ------------------------------------------------
  Serial.begin(9600);
  // Wait for USB Serial 
  delay(1000);  // NOTE: Some devices require a delay after Serial.begin() before serial port can be used

  gslc_InitDebug(&DebugOut);

  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT)) { return; }

  // ------------------------------------------------
  // Load Fonts
  // ------------------------------------------------
    if (!gslc_FontSet(&m_gui,E_FREE_MONO9,GSLC_FONTREF_PTR,&FreeMono9pt7b,1)) { return; }

  // ------------------------------------------------
  // Create graphic elements
  // ------------------------------------------------
  InitGUI();

}

// -----------------------------------
// Main event loop
// -----------------------------------
void loop()
{

  // ------------------------------------------------
  // Update GUI Elements
  // ------------------------------------------------
  
  //TODO - Add update code for any text, gauges, or sliders
  
  // ------------------------------------------------
  // Periodically call GUIslice update function
  // ------------------------------------------------
  gslc_Update(&m_gui);
    
}
