/*
* File:   study_x11_win.cpp
* Author: J. Ian Lindsay
* Date:   2023.12.16
*
*
* TODO: All of base_style (GfxUIStyle) should be in main_conf.
* TODO: Color-blind, light, dark profiles for base_style.
*/

#include "study.h"
#include "C3POnX11.h"

// NOTE: The GUI is running in a separate thread. It should only be
//   manipulated indirectly by an IPC mechanism, or by a suitable stand-ins.
extern bool continue_running;       // TODO: (rolled up newspaper) Bad...



/*******************************************************************************
* Definitions and mappings
*******************************************************************************/
/* Flags used by the top-level window */
#define STUDY_GUI_MOD_CTRL_HELD       0x00000001  // Modifier key state.
#define STUDY_GUI_MOD_ALT_HELD        0x00000002  // Modifier key state.
#define STUDY_GUI_SEL_BUF_HAS_PATH    0x00000004  // Inbound selection buffer will be holding a path.



/* Define the mouse buttons we observe and map them to the GUI layer in C3P. */
MouseButtonDef mouse_buttons[] = {
  { .label = "Left",
    .button_id = 1,
    .gfx_event_down = GfxUIEvent::TOUCH,
    .gfx_event_up   = GfxUIEvent::RELEASE
  },
  { .label = "Middle",
    .button_id = 2,
    .gfx_event_down = GfxUIEvent::DRAG,
    .gfx_event_up   = GfxUIEvent::NONE
  },
  { .label = "Right",
    .button_id = 3,
    .gfx_event_down = GfxUIEvent::SELECT,
    .gfx_event_up   = GfxUIEvent::NONE
  },
  { .label = "ScrlUp",
    .button_id = 4,
    .gfx_event_down = GfxUIEvent::MOVE_UP,
    .gfx_event_up   = GfxUIEvent::NONE
  },
  { .label = "ScrlDwn",
    .button_id = 5,
    .gfx_event_down = GfxUIEvent::MOVE_DOWN,
    .gfx_event_up   = GfxUIEvent::NONE
  },
  { .label = "TiltLeft",
    .button_id = 6,
    .gfx_event_down = GfxUIEvent::MOVE_LEFT,
    .gfx_event_up   = GfxUIEvent::NONE
  },
  { .label = "TiltRight",
    .button_id = 7,
    .gfx_event_down = GfxUIEvent::MOVE_RIGHT,
    .gfx_event_up   = GfxUIEvent::NONE
  }
};



/*******************************************************************************
* Styles
*******************************************************************************/

GfxUIStyle base_style;
//base_style.color_bg          = 0;
//base_style.color_border      = 0xFFFFFF;
//base_style.color_header      = 0x20B2AA;
//base_style.color_active      = 0x20B2AA;
//base_style.color_inactive    = 0xA0A0A0;
//base_style.color_selected    = 0x202020;
//base_style.color_unselected  = 0x202020;
//base_style.text_size         = 2;

// C3PValue base style
GfxUIStyle c3pvalue_style(
  0,          // bg
  0x202020,   // border
  0xFFFFFF,   // header
  0xC0C0C0,   // active
  0xA0A0A0,   // inactive
  0xFFFFFF,   // selected
  0x202020,   // unselected
  1           // t_size
);


/*******************************************************************************
* The visible root GfxUI container is a GfxUITabbedContentPane
*******************************************************************************/
GfxUITabbedContentPane _main_nav(
  GfxUILayout(
    0, 0,
    1280, 1024,
    ELEMENT_MARGIN, ELEMENT_MARGIN, ELEMENT_MARGIN, ELEMENT_MARGIN,
    0, 0, 0, 0               // Border_px(t, b, l, r)
  ),
  GfxUIStyle(0, // bg
    0xFFFFFF,   // border
    0xFFFFFF,   // header
    0xFFFFFF,   // active
    0xA0A0A0,   // inactive
    0xFFFFFF,   // selected
    0x202020,   // unselected
    2           // t_size
  ),
  0 //(GFXUI_FLAG_DRAW_FRAME_MASK)
);


/*******************************************************************************
* The overview pane
*******************************************************************************/
GfxUIGroup main_nav_overview(0, 0, 0, 0);

GfxUITextButton button_paste_from_selbuf(
  GfxUILayout(
    0, 20,
    120, 26,
    ELEMENT_MARGIN, ELEMENT_MARGIN, ELEMENT_MARGIN, ELEMENT_MARGIN,
    0, 0, 0, 0
  ),
  GfxUIStyle(0, // bg
    0xFFFFFF,   // border
    0xFFFFFF,   // header
    0x10bbcc,   // active
    0xA0A0A0,   // inactive
    0xFFFFFF,   // selected
    0x202020,   // unselected
    1           // t_size
  ),
  "Selection Buf",
  (GFXUI_BUTTON_FLAG_MOMENTARY)
);

GfxUITextButton button_paste_from_clpbrd(
  GfxUILayout(
    (button_paste_from_selbuf.elementPosX() + button_paste_from_selbuf.elementWidth()), button_paste_from_selbuf.elementPosY(),
    button_paste_from_selbuf.elementWidth(), button_paste_from_selbuf.elementHeight(),
    ELEMENT_MARGIN, ELEMENT_MARGIN, ELEMENT_MARGIN, ELEMENT_MARGIN,
    0, 0, 0, 0
  ),
  GfxUIStyle(0, // bg
    0xFFFFFF,   // border
    0xFFFFFF,   // header
    0x10bbcc,   // active
    0xA0A0A0,   // inactive
    0xFFFFFF,   // selected
    0x202020,   // unselected
    1           // t_size
  ),
  "Clipboard",
  (GFXUI_BUTTON_FLAG_MOMENTARY)
);


GfxUITextButton button_paste_from_testfield(
  GfxUILayout(
    (button_paste_from_clpbrd.elementPosX() + button_paste_from_clpbrd.elementWidth()), button_paste_from_clpbrd.elementPosY(),
    button_paste_from_clpbrd.elementWidth(), button_paste_from_clpbrd.elementHeight(),
    ELEMENT_MARGIN, ELEMENT_MARGIN, ELEMENT_MARGIN, ELEMENT_MARGIN,
    0, 0, 0, 0
  ),
  GfxUIStyle(0, // bg
    0xFFFFFF,   // border
    0xFFFFFF,   // header
    0x10bbcc,   // active
    0xA0A0A0,   // inactive
    0xFFFFFF,   // selected
    0x202020,   // unselected
    1           // t_size
  ),
  "Test Field",
  (GFXUI_BUTTON_FLAG_MOMENTARY)
);

C3PValue c3p_value_input_path("Nothing loaded");
GfxUIC3PValue input_path_render(
  &c3p_value_input_path,
  GfxUILayout(
    button_paste_from_selbuf.elementPosX(), (button_paste_from_selbuf.elementPosY() + button_paste_from_selbuf.elementHeight()),
    750, 28,
    ELEMENT_MARGIN, ELEMENT_MARGIN, ELEMENT_MARGIN, ELEMENT_MARGIN,
    0, 0, 0, 0   // Border_px(t, b, l, r)
  ),
  GfxUIStyle(0, // bg
    0xc0c0c0,   // border
    0xFFFFFF,   // header
    0xc0c0c0,   // active
    0xA0A0A0,   // inactive
    0xFFFFFF,   // selected
    0x202020,   // unselected
    2           // t_size
  ),
  (GFXUI_FLAG_ALWAYS_REDRAW)
);



C3PValue _c3p_value_bin(nullptr, 0);
GfxUIBlobRender value_render_bin(
  &_c3p_value_bin,
  GfxUILayout(
    input_path_render.elementPosX(), (input_path_render.elementPosY() + input_path_render.elementHeight() + 1),
    1080, 900,
    ELEMENT_MARGIN, ELEMENT_MARGIN, ELEMENT_MARGIN, ELEMENT_MARGIN,
    1, 1, 1, 1   // Border_px(t, b, l, r)
  ),
  GfxUIStyle(0, // bg
    0xc0c0c0,   // border
    0xFFFFFF,   // header
    0xc0c0c0,   // active
    0xA0A0A0,   // inactive
    0xFFFFFF,   // selected
    0x202020,   // unselected
    1           // t_size
  ),
  (GFXUI_FLAG_ALWAYS_REDRAW)
);


GfxUITextButton button_unload_file(
  GfxUILayout(
    (input_path_render.elementPosX() + input_path_render.elementWidth() + 20), input_path_render.elementPosY(),
    90, input_path_render.elementHeight(),
    ELEMENT_MARGIN, ELEMENT_MARGIN, ELEMENT_MARGIN, ELEMENT_MARGIN,
    0, 0, 0, 0
  ),
  GfxUIStyle(0, // bg
    0xFFFFFF,   // border
    0xFFFFFF,   // header
    0xFF1010,   // active
    0xA0A0A0,   // inactive
    0xFFFFFF,   // selected
    0x202020,   // unselected
    1           // t_size
  ),
  "Unload File",
  (GFXUI_BUTTON_FLAG_MOMENTARY)
);


GfxUITextButton button_paste_from_file(
  GfxUILayout(
    (button_unload_file.elementPosX() + button_unload_file.elementWidth() + 20), button_unload_file.elementPosY(),
    90, button_unload_file.elementHeight(),
    ELEMENT_MARGIN, ELEMENT_MARGIN, ELEMENT_MARGIN, ELEMENT_MARGIN,
    0, 0, 0, 0
  ),
  GfxUIStyle(0, // bg
    0xFFFFFF,   // border
    0xFFFFFF,   // header
    0x10FF10,   // active
    0xA0A0A0,   // inactive
    0xFFFFFF,   // selected
    0x202020,   // unselected
    1           // t_size
  ),
  "Paste Path",
  (GFXUI_BUTTON_FLAG_MOMENTARY)
);


/*******************************************************************************
* The contexts pane
*******************************************************************************/
GfxUIGroup main_nav_contexts(0, 0, 0, 0);


/*******************************************************************************
* The settings pane
*******************************************************************************/
GfxUIGroup main_nav_settings(0, 0, 0, 0);

GfxUITextButton _button_0(
  GfxUILayout(
    0, 0, 30, 30,
    0, ELEMENT_MARGIN, 0, ELEMENT_MARGIN,
    0, 0, 0, 0               // Border_px(t, b, l, r)
  ),
  GfxUIStyle(0, // bg
    0xFFFFFF,   // border
    0xFFFFFF,   // header
    0x9932CC,   // active
    0xA0A0A0,   // inactive
    0xFFFFFF,   // selected
    0x202020,   // unselected
    1           // t_size
  ),
  "ST"
);

GfxUIButton _button_1(
  GfxUILayout(
    (_button_0.elementPosX() + _button_0.elementWidth() + 1), _button_0.elementPosY(),
    30, 30,
    0, ELEMENT_MARGIN, 0, ELEMENT_MARGIN,
    0, 0, 0, 0               // Border_px(t, b, l, r)
  ),
  GfxUIStyle(0, // bg
    0xFFFFFF,   // border
    0xFFFFFF,   // header
    0x9932CC,   // active
    0xA0A0A0,   // inactive
    0xFFFFFF,   // selected
    0x202020,   // unselected
    1           // t_size
  )
);

GfxUITextButton _button_2(
  GfxUILayout(
    (_button_1.elementPosX() + _button_1.elementWidth() + 1), _button_1.elementPosY(),
    30, 30,
    0, ELEMENT_MARGIN, 0, ELEMENT_MARGIN,
    0, 0, 0, 0               // Border_px(t, b, l, r)
  ),
  GfxUIStyle(0, // bg
    0xFFFFFF,   // border
    0xFFFFFF,   // header
    0xFF8C00,   // active
    0xA0A0A0,   // inactive
    0xFFFFFF,   // selected
    0x202020,   // unselected
    1           // t_size
  ),
  "Rm",
  (GFXUI_BUTTON_FLAG_MOMENTARY)
);

GfxUIButton _button_3(
  GfxUILayout(
    (_button_2.elementPosX() + _button_2.elementWidth() + 1), _button_2.elementPosY(),
    30, 30,
    0, ELEMENT_MARGIN, 0, ELEMENT_MARGIN,
    0, 0, 0, 0               // Border_px(t, b, l, r)
  ),
  GfxUIStyle(0, // bg
    0xFFFFFF,   // border
    0xFFFFFF,   // header
    0xFF8C00,   // active
    0xA0A0A0,   // inactive
    0xFFFFFF,   // selected
    0x202020,   // unselected
    1           // t_size
  ),
  (GFXUI_BUTTON_FLAG_MOMENTARY)
);


GfxUISlider _slider_0(
  GfxUILayout(
    _button_0.elementPosX(), (_button_0.elementPosY() + _button_0.elementHeight() + 1),
    ((_button_3.elementPosX() + _button_3.elementWidth()) - _button_0.elementPosX()), 20,
    0, ELEMENT_MARGIN, 0, ELEMENT_MARGIN,
    0, 0, 0, 0               // Border_px(t, b, l, r)
  ),
  GfxUIStyle(0, // bg
    0xFFFFFF,   // border
    0xFFFFFF,   // header
    0x20B2AA,   // active
    0xA0A0A0,   // inactive
    0xFFFFFF,   // selected
    0x202020,   // unselected
    1           // t_size
  ),
  (GFXUI_SLIDER_FLAG_RENDER_VALUE)
);

GfxUISlider _slider_1(
  GfxUILayout(
    _slider_0.elementPosX(), (_slider_0.elementPosY() + _slider_0.elementHeight() + 1),
    ((_button_3.elementPosX() + _button_3.elementWidth()) - _button_0.elementPosX()), 20,
    0, ELEMENT_MARGIN, 0, ELEMENT_MARGIN,
    0, 0, 0, 0               // Border_px(t, b, l, r)
  ),
  GfxUIStyle(0, // bg
    0xFFFFFF,   // border
    0xFFFFFF,   // header
    0xFFA07A,   // active
    0xA0A0A0,   // inactive
    0xFFFFFF,   // selected
    0x202020,   // unselected
    1           // t_size
  ),
  (GFXUI_SLIDER_FLAG_RENDER_VALUE)
);

GfxUISlider _slider_2(
  GfxUILayout(
    _slider_1.elementPosX(), (_slider_1.elementPosY() + _slider_1.elementHeight() + 1),
    ((_button_3.elementPosX() + _button_3.elementWidth()) - _button_0.elementPosX()), 20,
    0, ELEMENT_MARGIN, 0, ELEMENT_MARGIN,
    0, 0, 0, 0               // Border_px(t, b, l, r)
  ),
  GfxUIStyle(0, // bg
    0xFFFFFF,   // border
    0xFFFFFF,   // header
    0xFFA07A,   // active
    0xA0A0A0,   // inactive
    0xFFFFFF,   // selected
    0x202020,   // unselected
    1           // t_size
  ),
  (GFXUI_SLIDER_FLAG_RENDER_VALUE)
);

GfxUISlider _slider_3(
  GfxUILayout(
    _button_3.elementPosX() + _button_3.elementWidth(), (_button_3.elementPosY() + 1),
    24, ((_slider_2.elementPosY() + _slider_2.elementHeight()) - _button_0.elementPosY()),
    0, ELEMENT_MARGIN, 0, ELEMENT_MARGIN,
    0, 0, 0, 0               // Border_px(t, b, l, r)
  ),
  GfxUIStyle(0, // bg
    0xFFFFFF,   // border
    0xFFFFFF,   // header
    0x90F5EE,   // active
    0xA0A0A0,   // inactive
    0xFFFFFF,   // selected
    0x202020,   // unselected
    1           // t_size
  ),
  (GFXUI_SLIDER_FLAG_RENDER_VALUE | GFXUI_SLIDER_FLAG_VERTICAL)
);

GfxUISlider _slider_4(
  GfxUILayout(
    (_slider_3.elementPosX() + _slider_3.elementWidth() + 1), (_slider_3.elementPosY() + 1),
    24, ((_slider_2.elementPosY() + _slider_2.elementHeight()) - _button_0.elementPosY()),
    0, ELEMENT_MARGIN, 0, ELEMENT_MARGIN,
    0, 0, 0, 0               // Border_px(t, b, l, r)
  ),
  GfxUIStyle(0, // bg
    0xFFFFFF,   // border
    0xFFFFFF,   // header
    0xDC143C,   // active
    0xA0A0A0,   // inactive
    0xFFFFFF,   // selected
    0x202020,   // unselected
    1           // t_size
  ),
  (GFXUI_SLIDER_FLAG_RENDER_VALUE | GFXUI_SLIDER_FLAG_VERTICAL)
);



/*******************************************************************************
* The internals pane, which may not be rendered.
*******************************************************************************/
GfxUIGroup main_nav_internals(0, 0, 0, 0);

C3PValue _c3p_value_ident((Identity*) &ident_uuid);

GfxUIC3PValue _value_render_ident(
  &_c3p_value_ident,
  GfxUILayout(
    50, 50,
    240, 32,
    0, ELEMENT_MARGIN, 0, ELEMENT_MARGIN,
    0, 0, 0, 0               // Border_px(t, b, l, r)
  ), c3pvalue_style,
  (GFXUI_C3PVAL_FLAG_SHOW_TYPE_INFO)
);


GfxUIKeyValuePair _kvp_render_main_conf(
  main_conf.getKVP(),
  GfxUILayout(
    _value_render_ident.elementPosX(), (_value_render_ident.elementPosY() + _value_render_ident.elementHeight()),
    240, 300,
    0, ELEMENT_MARGIN, 0, ELEMENT_MARGIN,
    1, 1, 1, 1               // Border_px(t, b, l, r)
  ),
  GfxUIStyle(0, // bg
    0xFFFFFF,   // border
    0xFFFFFF,   // header
    0xfefe00,   // active
    0xA0A0A0,   // inactive
    0xFFFFFF,   // selected
    0x202020,   // unselected
    1           // t_size
  ),
  (0)
);


/*******************************************************************************
* Callbacks and other non-class functions
*******************************************************************************/
static MainGuiWindow* todo_shim = nullptr;

void ui_value_change_callback(GfxUIElement* element) {
  if (nullptr == todo_shim) return;   // Bailout

  if (element == ((GfxUIElement*) &_slider_1)) {
    c3p_log(LOG_LEV_INFO, __PRETTY_FUNCTION__, "Slider-1 %.2f", (double) _slider_1.value());
  }
  else if (element == ((GfxUIElement*) &_slider_2)) {
    c3p_log(LOG_LEV_INFO, __PRETTY_FUNCTION__, "Slider-2 %.2f", (double) _slider_2.value());
  }
  else if (element == ((GfxUIElement*) &_slider_3)) {
    c3p_log(LOG_LEV_INFO, __PRETTY_FUNCTION__, "Slider-3 %.2f", (double) _slider_3.value());
  }

  else if (element == ((GfxUIElement*) &button_paste_from_clpbrd)) {
    if (button_paste_from_clpbrd.pressed()) {
      todo_shim->request_clipboard();
    }
  }
  else if (element == ((GfxUIElement*) &button_paste_from_selbuf)) {
    if (button_paste_from_selbuf.pressed()) {
      todo_shim->request_selection_buffer();
    }
  }
  else if (element == ((GfxUIElement*) &button_paste_from_file)) {
    if (button_paste_from_file.pressed()) {
      todo_shim->loadPathInSelection();
    }
  }
  else if (element == ((GfxUIElement*) &button_unload_file)) {
    if (button_unload_file.pressed()) {
      c3p_value_input_path.set("Nothing loaded");
      todo_shim->unloadInputFile();
    }
  }

  else if (element == ((GfxUIElement*) &button_paste_from_testfield)) {
    if (button_paste_from_testfield.pressed()) {
      // Byte test field.
      uint8_t test_field[16384];
      for (uint32_t i = 0; i < sizeof(test_field); i++) {
        if (0x00000100 & i) {
          test_field[i] = (255 - (uint8_t) i);
        }
        else {
          test_field[i] = (uint8_t) i;
        }
      }
      c3p_value_input_path.set("Test field");
      bin_field.clear();
      bin_field.concat(test_field, sizeof(test_field));
    }
  }

  else {
    c3p_log(LOG_LEV_INFO, __PRETTY_FUNCTION__, "VALUE_CHANGE %p", element);
  }
}


void MainGuiWindow::loadPathInSelection() {
  _modifiers.set(STUDY_GUI_SEL_BUF_HAS_PATH);
  request_selection_buffer();
}

void MainGuiWindow::unloadInputFile() {
  if (nullptr != file_in_ptr) {
    delete file_in_ptr;
    file_in_ptr = nullptr;
    bin_field.clear();
    unloadMapFile();
  }
}


void MainGuiWindow::unloadMapFile() {
  if (nullptr != file_map_ptr) {
    delete file_map_ptr;
    file_map_ptr = nullptr;
  }
}



/*******************************************************************************
* MainGuiWindow implementation
*******************************************************************************/

int8_t MainGuiWindow::createWindow() {
  int8_t ret = _init_window();
  if (0 == ret) {
    todo_shim = this;
    map_button_inputs(mouse_buttons, sizeof(mouse_buttons) / sizeof(mouse_buttons[0]));
    _overlay.reallocate();

    if (input_path.length() > 0) {
      file_in_ptr = new C3PFile((char*) input_path.string());
      if (nullptr != file_in_ptr) {
        bool should_free_file = true;
        if (file_in_ptr->isFile() & file_in_ptr->exists()) {
          StringBuilder file_data;
          int32_t file_read_ret = file_in_ptr->read(&file_data);
          if (0 < file_read_ret) {
            should_free_file = false;
            c3p_value_input_path.set(file_in_ptr->path());
            bin_field.clear();
            bin_field.concatHandoff(&file_data);
          }
          else c3p_log(LOG_LEV_ERROR, __PRETTY_FUNCTION__, "Path %s appears to be an empty file.", (char*) input_path.string());
        }
        else c3p_log(LOG_LEV_ERROR, __PRETTY_FUNCTION__, "Path %s doesn't exist or isn't a file.", (char*) input_path.string());
        if (should_free_file) {
          delete file_in_ptr;
          file_in_ptr = nullptr;
        }
      }
    }

    // Assemble the overview pane...
    main_nav_overview.add_child(&input_path_render);
    main_nav_overview.add_child(&button_paste_from_file);
    main_nav_overview.add_child(&button_unload_file);
    main_nav_overview.add_child(&value_render_bin);
    main_nav_overview.add_child(&button_paste_from_clpbrd);
    main_nav_overview.add_child(&button_paste_from_selbuf);
    main_nav_overview.add_child(&button_paste_from_testfield);

    // Assemble the contexts pane...
    //main_nav_contexts.add_child(&_txt_area_0);

    // Assemble the contexts pane...
    main_nav_settings.add_child(&_button_0);
    main_nav_settings.add_child(&_button_1);
    main_nav_settings.add_child(&_button_2);
    main_nav_settings.add_child(&_button_3);
    main_nav_settings.add_child(&_slider_0);
    main_nav_settings.add_child(&_slider_1);
    main_nav_settings.add_child(&_slider_2);
    main_nav_settings.add_child(&_slider_3);
    main_nav_settings.add_child(&_slider_4);

    // Assemble the internals pane...
    main_nav_internals.add_child(&_value_render_ident);
    main_nav_internals.add_child(&_kvp_render_main_conf);

    // Adding the contant panes will cause the proper screen co-ords to be imparted
    //   to the group objects. We can then use them for element flow.
    _main_nav.addTab("Overview", &main_nav_overview, true);
    _main_nav.addTab("Contexts", &main_nav_contexts);
    _main_nav.addTab("Settings", &main_nav_settings);
    _main_nav.addTab("Internals", &main_nav_internals);

    // Add the whole mess to the root GfxUI element, the presence of which is an
    //   organizational formality of C3Px11Window.
    root.add_child(&_main_nav);

    _slider_0.value(0.5);
    _refresh_period.reset();

    _c3p_value_bin.set(bin_field.string(), bin_field.length(), TCode::BINARY);
    setCallback(ui_value_change_callback);
  }
  return ret;
}



int8_t MainGuiWindow::closeWindow() {
  continue_running = false;
  return _deinit_window();   // TODO: Ensure that this is both neccessary and safe.
}



int8_t MainGuiWindow::render_overlay() {
  // If the pointer is within the window, we note its location and
  //   annotate the overlay.
  ui_magnifier.pointerLocation(_pointer_x, _pointer_y);
  if (_modifiers.value(STUDY_GUI_MOD_CTRL_HELD)) {
    ui_magnifier.render(&gfx_overlay);
  }
  return 0;
}


/*
* Called to unconditionally show the elements in the GUI.
*/
int8_t MainGuiWindow::render(bool force) {
  int8_t ret = 0;
  uint8_t* render_ptr = nullptr;
  uint32_t render_len = 0;
  if (0 != _c3p_value_bin.get_as(&render_ptr, &render_len)) {
    c3p_log(LOG_LEV_DEBUG, __PRETTY_FUNCTION__, "Failed to get BLOB from _c3p_value_bin.");
  }
  const bool BLOB_CHANGED = (bin_field.string() != render_ptr);
  if (BLOB_CHANGED) {
    c3p_log(LOG_LEV_DEBUG, __PRETTY_FUNCTION__, "BLOB_CHANGED");
    _c3p_value_bin.set(bin_field.string(), bin_field.length(), TCode::BINARY);
  }
  else if (force) {
  }

  if (force | BLOB_CHANGED) {
    // TODO: Example bottom-float logic that (although reliable) is uncommonly
    //   brittle. Even for PoC code.
    // This is one of the many TODOs that will be resolved by a simple element
    //   flow API/capability in GfxUI.
    // But unit tests on that feature must preceed a committed attempt in C3P.
    // I refuse to do it in any manner other than test-driven development. It is
    //   simply too complex a concern.
    //const uint  CONSOLE_INPUT_X_POS = 0;
    //const uint  CONSOLE_INPUT_Y_POS = (height() - CONSOLE_INPUT_HEIGHT) - 1;
    // _txt_area_0.reposition(CONSOLE_INPUT_X_POS, CONSOLE_INPUT_Y_POS);
    // _txt_area_0.resize(width(), CONSOLE_INPUT_HEIGHT);

    // TODO: Generalize or relocate to logic for the internals window.
    // StringBuilder pitxt;
    // pitxt.concat("Right Hand of Manuvr\nBuild date " __DATE__ " " __TIME__);
    // struct utsname sname;
    // if (1 != uname(&sname)) {
    //   pitxt.concatf("%s %s (%s)", sname.sysname, sname.release, sname.machine);
    //   pitxt.concatf("\n%s", sname.version);
    // }
    // pitxt.concatf("Window: %dx%d", _fb.x(), _fb.y());
    // _program_info_txt.clear();
    // _program_info_txt.pushBuffer(&pitxt);
    ret = 1;
  }
  return ret;
}



// Called from the thread.
int8_t MainGuiWindow::poll() {
  int8_t ret = 0;
  if (0 < XPending(_dpy)) {
    Atom WM_DELETE_WINDOW = XInternAtom(_dpy, "WM_DELETE_WINDOW", False);
    Atom UTF8      = XInternAtom(_dpy, "UTF8_STRING", True);
    Atom CLIPBOARD = XInternAtom(_dpy, "CLIPBOARD", 0);
    Atom PRIMARY   = XInternAtom(_dpy, "PRIMARY", 0);
    XEvent e;
    XNextEvent(_dpy, &e);

    switch (e.type) {
      case SelectionNotify:
        c3p_log(LOG_LEV_DEBUG, __PRETTY_FUNCTION__, "SelectionNotify");
        if ((CLIPBOARD == e.xselection.selection) || (PRIMARY == e.xselection.selection)) {
          if (e.xselection.property) {
            uint8_t* data = nullptr;
            Atom target;
            int format;
            unsigned long ele_count;
            unsigned long size;
            XGetWindowProperty(e.xselection.display, e.xselection.requestor, e.xselection.property, 0L,(~0L), 0, AnyPropertyType, &target, &format, &size, &ele_count, &data);
            if ((target == UTF8) || (target == XA_STRING)) {
              if (_modifiers.value(STUDY_GUI_SEL_BUF_HAS_PATH)) {
                _modifiers.clear(STUDY_GUI_SEL_BUF_HAS_PATH);
                // If it hasn't been done already, create a working C3PFile with
                //   the given path name.
                if (nullptr == file_in_ptr) {
                  file_in_ptr = new C3PFile((char*) data);
                  if (nullptr != file_in_ptr) {
                    bool should_free_file = true;
                    if (file_in_ptr->isFile() & file_in_ptr->exists()) {
                      StringBuilder file_data;
                      int32_t file_read_ret = file_in_ptr->read(&file_data);
                      if (0 < file_read_ret) {
                        should_free_file = false;
                        c3p_value_input_path.set(file_in_ptr->path());
                        bin_field.clear();
                        bin_field.concatHandoff(&file_data);
                      }
                      else c3p_log(LOG_LEV_ERROR, __PRETTY_FUNCTION__, "Path %s appears to be an empty file.", (char*) data);
                    }
                    else c3p_log(LOG_LEV_ERROR, __PRETTY_FUNCTION__, "Path %s doesn't exist or isn't a file.", (char*) data);

                    if (should_free_file) {
                      delete file_in_ptr;
                      file_in_ptr = nullptr;
                    }
                  }
                  else c3p_log(LOG_LEV_ALERT, __PRETTY_FUNCTION__, "Failed to instance a C3PFile.");
                }
                else c3p_log(LOG_LEV_WARN, __PRETTY_FUNCTION__, "Can't set a new file until the one already open has been closed.");
              }
              else {
                // Deep-copy the clipboard data into the blob buffer.
                bin_field.clear();
                bin_field.concat(data, size);
                c3p_log(LOG_LEV_INFO, __PRETTY_FUNCTION__, "Set blob from clipboard (%d bytes).", size);
              }
              XFree(data);
            }
            else c3p_log(LOG_LEV_NOTICE, __PRETTY_FUNCTION__, "target: %d", (int) target);

            XDeleteProperty(e.xselection.display, e.xselection.requestor, e.xselection.property);
          }
        }
        break;
      case SelectionRequest:
        c3p_log(LOG_LEV_DEBUG, __PRETTY_FUNCTION__, "SelectionRequest");
        break;
      case SelectionClear:
        c3p_log(LOG_LEV_DEBUG, __PRETTY_FUNCTION__, "SelectionClear");
        break;
      case PropertyNotify:
        c3p_log(LOG_LEV_DEBUG, __PRETTY_FUNCTION__, "PropertyNotify");
        break;


      case Expose:
        {
          // Try to resize the window. If it isn't required, _refit_window()
          //   will return zero.
          int8_t local_ret = _refit_window();
          if (0 == local_ret) {
          }
          else {
            c3p_log(LOG_LEV_ERROR, __PRETTY_FUNCTION__, "Window resize failed (%d).", local_ret);
          }
        }
        break;

      case ButtonPress:
      case ButtonRelease:
        {
          uint16_t btn_id = e.xbutton.button;
          if (_modifiers.value(STUDY_GUI_MOD_CTRL_HELD)) {
            const GfxUIEvent event = (btn_id == 5) ? GfxUIEvent::MOVE_DOWN : GfxUIEvent::MOVE_UP;
            switch (btn_id) {
              case 4:
              case 5:
                // Scroll events adjust the magnifier scale if CTRL is held.
                ui_magnifier.notify(
                  ((btn_id == 5) ? GfxUIEvent::MOVE_DOWN : GfxUIEvent::MOVE_UP),
                  ui_magnifier.elementPosX(), ui_magnifier.elementPosY(), nullptr
                );
              default:
                break;
            }
          }
          else {
            int8_t ret = _proc_mouse_button(btn_id, e.xbutton.x, e.xbutton.y, (e.type == ButtonPress));
            if (0 == ret) {
              // Any unclaimed input can be handled in this block.
            }
          }
        }
        break;


      case KeyRelease:
        {
          char buf[128] = {0, };
          KeySym keysym;
          XLookupString(&e.xkey, buf, sizeof(buf), &keysym, nullptr);
          if ((keysym == XK_Control_L) | (keysym == XK_Control_R)) {
            _modifiers.clear(STUDY_GUI_MOD_CTRL_HELD);
            //c3p_log(LOG_LEV_DEBUG, __PRETTY_FUNCTION__, "CTRL release");
          }
          else if ((keysym == XK_Alt_L) | (keysym == XK_Alt_R)) {
            _modifiers.clear(STUDY_GUI_MOD_ALT_HELD);
          }
        }
        break;


      case KeyPress:
        {
          char buf[128] = {0, };
          KeySym keysym;
          int ret_local = XLookupString(&e.xkey, buf, sizeof(buf), &keysym, nullptr);
          if (keysym == XK_Escape) {
            _keep_polling = false;
          }
          else if (keysym == XK_Return) {
            // StringBuilder _tmp_sbldr;
            // _tmp_sbldr.concat('\n');
            // console.pushBuffer(&_tmp_sbldr);
          }
          else if ((keysym == XK_Control_L) | (keysym == XK_Control_R)) {
            _modifiers.set(STUDY_GUI_MOD_CTRL_HELD);
          }
          else if ((keysym == XK_Alt_L) | (keysym == XK_Alt_R)) {
            _modifiers.set(STUDY_GUI_MOD_ALT_HELD);
          }
          else {
            c3p_log(LOG_LEV_DEBUG, __PRETTY_FUNCTION__, "Key press: %s (%s)", buf, XKeysymToString(keysym));
          }
        }
        break;

      case ClientMessage:
        if (static_cast<unsigned int>(e.xclient.data.l[0]) == WM_DELETE_WINDOW) {
          _keep_polling = false;
        }
        else {
          //c3p_log(LOG_LEV_DEBUG, __PRETTY_FUNCTION__, "ClientMessage");
        }
        break;

      case MotionNotify:
        _pointer_x = e.xmotion.x;
        _pointer_y = e.xmotion.y;
        //c3p_log(LOG_LEV_INFO, __PRETTY_FUNCTION__, "_process_motion(%d, %d) returns %d.", _pointer_x, _pointer_y, _process_motion());
        _process_motion();
        break;

      default:
        c3p_log(LOG_LEV_DEBUG, __PRETTY_FUNCTION__, "Unhandled XEvent: %d", e.type);
        break;
    }
  }

  if (!_keep_polling) {
    closeWindow();
    ret = -1;
  }
  else {
    // Render the UI elements...
    //_c3p_value_3.set(test_filter_0.snr());
    if (1 == _redraw_window()) {
      // TODO: We aren't flexing the TimeSeries render here. But we should still
      //   capture some render and processing timing stats for internals.
      // if (1 == test_filter_0.feedSeries(_redraw_timer.lastTime())) {
      //   test_filter_stdev.feedSeries(test_filter_0.stdev());
      // }
    }
  }

  return ret;
}
