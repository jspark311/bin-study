/*
* File:   study.h
* Author: J. Ian Lindsay
* Date:   2023.12.16
*
*/

#ifndef __BLOBSTUDY_HEADER_H__
#define __BLOBSTUDY_HEADER_H__


/* System-level includes */
#include <cstdio>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <math.h>
#include <fstream>
#include <iostream>

/* The parts of C3P we want. */
#include "CppPotpourri/src/CppPotpourri.h"
#include "CppPotpourri/src/AbstractPlatform.h"
#include "CppPotpourri/src/StringBuilder.h"
#include "CppPotpourri/src/C3PValue/KeyValuePair.h"
#include "CppPotpourri/src/TimerTools/TimerTools.h"
#include "CppPotpourri/src/Storage/RecordTypes/ConfRecord.h"
#include "CppPotpourri/src/uuid.h"
#include "CppPotpourri/src/cbor-cpp/cbor.h"
#include "CppPotpourri/src/Image/Image.h"
#include "CppPotpourri/src/Image/ImageUtils.h"
#include "CppPotpourri/src/Image/GfxUI.h"
#include "Platform/C3POnX11.h"
#include "Platform/Linux.h"
#include "Platform/LinuxStorage.h"



#define PROGRAM_NAME     "BlobStudy"                   // Program proper name.
#define PROGRAM_VERSION  "(stop pretending you care)"  // Program version.


/*******************************************************************************
* Limbo (TODO)
*******************************************************************************/
// TODO: This _must_ be uint32 until I promote the CONFIG option into C3P and
//   provide this typedef from that altitude.
// No one is going to use C3P's Image classes to handle X/Y axis coordinates in
//   excess of 64535, right? Would save massively on storage requirements.
// Should at least be an option...
typedef uint32_t PixelInt;

/* GUI parameters */
#define ELEMENT_MARGIN          3

/*******************************************************************************
* Configuration keys
*******************************************************************************/
enum class StudyConfKey : uint8_t {
  SHOW_PANE_INTERNALS,
  INVALID
};


/*******************************************************************************
* This is our one (only) window, and what it tracks.
* This is subtly different from the root GfxnUI element which the C3Px11Window
*   contains by composition.
* C3Px11Window is fundamentally an I/O class, and (consistent with C3P's
*   standards) has a poll() function to facilitate the carving of compute on
*   the chronological plane. On linux, that poll() function will be called by
*   a thread devoted to each instantiated C3Px11Window.
* Because this program is not designed to run with multiple real windows, we
*   only define one kind.
*******************************************************************************/
class MainGuiWindow : public C3Px11Window {
  public:
    // Firmware UIs are small. If the host is showing the UI on a 4K monitor, it
    //   will cause "the squints".
    // This element should not be added to the element list, and it ignores the
    //   position arguments.
    GfxUIMagnifier ui_magnifier;

    MainGuiWindow(PixelInt x, PixelInt y, PixelInt w, PixelInt h, const char* TITLE) :
      C3Px11Window(x, y, w, h, TITLE),
      ui_magnifier(&_fb, 0, 0,
        200, 200, 0xFFFFFF,
        (GFXUI_FLAG_DRAW_FRAME_U | GFXUI_FLAG_DRAW_FRAME_L | GFXUI_MAGNIFIER_FLAG_SHOW_FEED_FRAME)
      ),
      _modifiers(0),
      _key_target(nullptr),
      _paste_target(nullptr) {};

    /* Obligatory overrides from C3Px11Window. */
    int8_t poll();
    int8_t createWindow();
    int8_t closeWindow();
    int8_t render(bool force);
    int8_t render_overlay();
    void loadPathInSelection();
    void unloadInputFile();
    void unloadMapFile();


  private:
    FlagContainer32 _modifiers;
    GfxUIElement*   _key_target;
    GfxUIElement*   _paste_target;
    C3PFile*        file_in_ptr  = nullptr;
    C3PFile*        file_map_ptr = nullptr;
};


/*******************************************************************************
* Globals that are extern'd
*******************************************************************************/
extern IdentityUUID ident_uuid;
extern ConfRecordValidation<StudyConfKey> main_conf;
extern StringBuilder bin_field;   // The blob we are studying.
extern StringBuilder input_path;

#endif  // __BLOBSTUDY_HEADER_H__
