//-----------------------------------------------------------------------
// JrPlugin_MyPlugin
//-----------------------------------------------------------------------


//---------------------------------------------------------------------------
// Header Guard
#ifndef JrPlugin_MyPluginH
#define JrPlugin_MyPluginH
//---------------------------------------------------------------------------



//------------------------------------------------------------------------
// system includes
#include <windows.h>
//-----------------------------------------------------------------------


//-----------------------------------------------------------------------
// includes generic plugin shell of other headers
#include "JrPlugin_GenericShell.h"
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
// Inlcude FARR SPECIFIC plugin func definitions (shared with host)
#include "JrPluginFuncs_FARR.h"
//-----------------------------------------------------------------------



















//-----------------------------------------------------------------------
// Plugin defines refered to in JrPlugin_GenericShell.cpp
// IMPORTANT: you need to customize there
//
#define ThisPlugin_DisplayName			"FarrMultiMonitor"
#define ThisPlugin_Author			    "Philipp Tschannen (phitsc)"
#define ThisPlugin_HomepageURL			"http://farrmultimonitor.objecttechnology.com"
#define ThisPlugin_UpdateURL			""
#define ThisPlugin_ShortDescription		"Display FARR on active monitor and/or center FARR main window."
#define ThisPlugin_LongDescription		"Adds support for multiple monitors by displaying the FARR main window on the currently active monitor and/or allows centering the FARR main window on the screen."
//
// If you support advanced options button, specify caption here, otherwise ""
#define ThisPlugin_AdvConfigString		"Advanced Options"
//
// If you support a read me file or help file, specify caption here, otherwise ""
#define ThisPlugin_ReadMeString			"View Help File"
// Used internally to display help file
#define ThisPlugin_ReadMeFile			"readme.txt"
//
// If this plugin has an icon (recommended), specify icon file in same dir as dll - shown in plugin list
#define ThisPlugin_IconFilename			"multimonitor.ico"
//
//-----------------------------------------------------------------------






















//-----------------------------------------------------------------------
// FARR specific stuff
#define ThisPlugin_FARR_DefaultAlias		"multimonitor"
#define ThisPlugin_FARR_DefaultRegex		""
#define ThisPlugin_FARR_DefaultRegexFilter	""
#define ThisPlugin_FARR_DefaultKeywordStr	""
#define ThisPlugin_FARR_DefaultScoreStr		""
//-----------------------------------------------------------------------






//-----------------------------------------------------------------------
// external globals defined in the .cpp
extern E_SearchStateT current_searchstate;
extern E_ResultAvailableStateT resultsavailabletype;
extern int numresultsavailable;
extern BOOL current_lockstate;
extern bool isready;
//-----------------------------------------------------------------------


//-----------------------------------------------------------------------
// FARR: global callback when we want to tell host our search state has changed asynchronously
extern Fp_GlobalPluginCallback_NotifySearchStateChanged callbackfp_notifysearchstatechanged;
//-----------------------------------------------------------------------


//-----------------------------------------------------------------------
// Forward declarations
void ExecuteCallback_SearchStateChanged();
//-----------------------------------------------------------------------


//-----------------------------------------------------------------------
// Our plugin-specific functions called by generic plugin shell
BOOL MyPlugin_DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved);
//
BOOL MyPlugin_DoInit();
BOOL MyPlugin_DoShutdown();
BOOL MyPlugin_GetStrVal(const char* varname,char *destbuf, int maxlen);
BOOL MyPlugin_SetStrVal(const char* varname, void *val);
BOOL MyPlugin_SupportCheck(const char* testname, int version);
BOOL MyPlugin_DoAdvConfig();
BOOL MyPlugin_DoShowReadMe();
BOOL MyPlugin_SetState(int newstateval);
//-----------------------------------------------------------------------





//-----------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------


