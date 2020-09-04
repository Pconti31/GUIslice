//<File !Start!>
// FILE: [ex_busywait.ino]
// Created by GUIslice Builder version: [0.13.0-SNAPSHOT-0713]
//
// GUIslice Builder Generated File
//
// For the latest guides, updates and support view:
// https://github.com/ImpulseAdventure/GUIslice
//
//<File !End!>
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

// Include any extended elements
//<Includes !Start!>
//<Includes !End!>

#include "elem/XBusyWait.h"

// ------------------------------------------------
// Headers and Defines for fonts
// Note that font files are located within the Adafruit-GFX library folder:
// ------------------------------------------------
//<Fonts !Start!>
#include <Adafruit_GFX.h>
// Note that these files are located within the Adafruit-GFX library folder:
#include "Fonts/FreeMono12pt7b.h"
//<Fonts !End!>

// ------------------------------------------------
// Defines for resources
// ------------------------------------------------
//<Resources !Start!>
//<Resources !End!>

// ------------------------------------------------
// Enumerations for pages, elements, fonts, images
// ------------------------------------------------
//<Enum !Start!>
enum {E_PAGE_MAIN,E_PG_BUSY};
enum {E_ELEM_BUSYWAIT,E_ELEM_TEXT1};
// Must use separate enum for fonts with MAX_FONT at end to use gslc_FontSet.
enum {E_FONT_MONO12,MAX_FONT};
//<Enum !End!>

// ------------------------------------------------
// Instantiate the GUI
// ------------------------------------------------

// ------------------------------------------------
// Define the maximum number of elements and pages
// ------------------------------------------------
//<ElementDefines !Start!>
#define MAX_PAGE                2

#define MAX_ELEM_PAGE_MAIN 1                                          // # Elems total on page
#define MAX_ELEM_PAGE_MAIN_RAM MAX_ELEM_PAGE_MAIN // # Elems in RAM


#define MAX_ELEM_POPUP_BUSY 1                                          // # Elems total on page
#define MAX_ELEM_POPUP_BUSY_RAM MAX_ELEM_POPUP_BUSY // # Elems in RAM

//<ElementDefines !End!>

// ------------------------------------------------
// Create element storage
// ------------------------------------------------
gslc_tsGui                      m_gui;
gslc_tsDriver                   m_drv;
gslc_tsFont                     m_asFont[MAX_FONT];
gslc_tsPage                     m_asPage[MAX_PAGE];

//<GUI_Extra_Elements !Start!>
gslc_tsElem                     m_asPage1Elem[MAX_ELEM_PAGE_MAIN_RAM];
gslc_tsElemRef                  m_asPage1ElemRef[MAX_ELEM_PAGE_MAIN];
gslc_tsElem                     m_asPopup1Elem[MAX_ELEM_POPUP_BUSY_RAM];
gslc_tsElemRef                  m_asPopup1ElemRef[MAX_ELEM_POPUP_BUSY];

#define MAX_STR                 100

//<GUI_Extra_Elements !End!>

gslc_tsXBusyWait                m_sBusyWait;

// ------------------------------------------------
// Program Globals
// ------------------------------------------------
int32_t   m_timeDelay = 0;
bool      m_bQuit = false;

// Save some element references for direct access
//<Save_References !Start!>
//<Save_References !End!>

// Define debug message function
static int16_t DebugOut(char ch) { if (ch == (char)'\n') Serial.println(""); else Serial.write(ch); return 0; }

// ------------------------------------------------
// Callback Methods
// ------------------------------------------------
//<Button Callback !Start!>
//<Button Callback !End!>
//<Checkbox Callback !Start!>
//<Checkbox Callback !End!>
//<Input Callback !Start!>
//<Input Callback !End!>
//<Listbox Callback !Start!>
//<Listbox Callback !End!>
//<Draw Callback !Start!>
//<Draw Callback !End!>
//<Slider Callback !Start!>
//<Slider Callback !End!>
//<Tick Callback !Start!>
//<Tick Callback !End!>

// ------------------------------------------------
// Create page elements
// ------------------------------------------------
bool InitGUI()
{
  gslc_tsElemRef* pElemRef = NULL;

//<InitGUI !Start!>
  gslc_PageAdd(&m_gui,E_PAGE_MAIN,m_asPage1Elem,MAX_ELEM_PAGE_MAIN_RAM,m_asPage1ElemRef,MAX_ELEM_PAGE_MAIN);
  gslc_PageAdd(&m_gui,E_PG_BUSY,m_asPopup1Elem,MAX_ELEM_POPUP_BUSY_RAM,m_asPopup1ElemRef,MAX_ELEM_POPUP_BUSY);

  // NOTE: The current page defaults to the first page added. Here we explicitly
  //       ensure that the main page is the correct page no matter the add order.
  gslc_SetPageCur(&m_gui,E_PG_BUSY);
  
  // Background flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_BLACK);

  // -----------------------------------
  // PAGE: E_PAGE_MAIN
  
  
  // Create E_ELEM_TEXT1 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT1,E_PAGE_MAIN,(gslc_tsRect){101,90,117,33},
    (char*)"FINISHED",0,E_FONT_MONO12);

  // -----------------------------------
  // PAGE: E_PG_BUSY
  
   
//<InitGUI !End!>

  // Create E_ELEM_BUSYWAIT 
  gslc_ElemXBusyWaitCreate(&m_gui, E_ELEM_BUSYWAIT, E_PG_BUSY, &m_sBusyWait,
//  (gslc_tsRect){85,45,150,150},12,10,GSLC_COL_BLUE);
  (gslc_tsRect){110,70,100,100},10,5,GSLC_COL_BLUE);



  return true;
}

void setup()
{
  // ------------------------------------------------
  // Initialize
  // ------------------------------------------------
  Serial.begin(9600);
  // Wait for USB Serial 
  //delay(1000);  // NOTE: Some devices require a delay after Serial.begin() before serial port can be used

  gslc_InitDebug(&DebugOut);

  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT)) { return; }

  // ------------------------------------------------
  // Load Fonts
  // ------------------------------------------------
//<Load_Fonts !Start!>
    if (!gslc_FontSet(&m_gui,E_FONT_MONO12,GSLC_FONTREF_PTR,&FreeMono12pt7b,1)) { return; }
//<Load_Fonts !End!>

  // ------------------------------------------------
  // Create graphic elements
  // ------------------------------------------------
  InitGUI();

//<Startup !Start!>
//<Startup !End!>

}

// -----------------------------------
// Main event loop
// -----------------------------------
void loop()
{

  // ------------------------------------------------
  // Update GUI Elements
  // ------------------------------------------------
  
  if (m_timeDelay < 10 /* ten seconds */) {
    m_timeDelay++;
  } else {
    gslc_SetPageCur(&m_gui,E_PAGE_MAIN);
    m_bQuit = true;
  }
  
  // ------------------------------------------------
  // Periodically call GUIslice update function
  // ------------------------------------------------
  gslc_Update(&m_gui);

  if (m_bQuit) {
    while(1) {}
  }
  
}
