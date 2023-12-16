/*
* File:   study.cpp
* Author: J. Ian Lindsay
* Date:   2023.12.16
*
*/
using namespace std;   // Flat namespace. Everything is classed out.

#include "study.h"   // Main program header


/*******************************************************************************
* Statics
*******************************************************************************/
/*
* The program has a set of configurations that it defines and loads at runtime.
* This defines everything required to handle that conf fluidly and safely.
*/
// Then, we bind those enum values each to a type code, and to a semantic string
//   suitable for storage or transmission to a counterparty.
static const EnumDef<StudyConfKey> CONF_KEY_LIST[] = {
  { StudyConfKey::SHOW_PANE_INTERNALS,   "SHOW_PANE_INTERNALS",    0, (uint8_t) TCode::BOOLEAN    },
  { StudyConfKey::INVALID,               "INVALID",                (ENUM_FLAG_MASK_INVALID_CATCHALL), 0}
};

// The top-level enum wrapper binds the above definitions into a tidy wad
//   of contained concerns.
static const EnumDefList<StudyConfKey> CONF_LIST(
  CONF_KEY_LIST, (sizeof(CONF_KEY_LIST) / sizeof(CONF_KEY_LIST[0])),
  "MainConf"
);


/*******************************************************************************
* Globals used outside of this translation unit.
*******************************************************************************/
StringBuilder  conf_path;
StringBuilder  input_path;
bool           continue_running  = true;

// Create the conf object. Load it later.
ConfRecordValidation<StudyConfKey> main_conf(0, &CONF_LIST);

StringBuilder  bin_field;

// TODO: Might be dropped or auto-generated from the executable's hash.
// NOTE: This program does no comm work, and so never really needs to report
//   itself in this fashion. Pedantry...
IdentityUUID ident_uuid("BIN_ID", (char*) "73a6a69d-f6f7-4547-88ad-ab9eb0290974");

// Do not be misled by the name. This program has no console. It does what a
//   UART or semihosted JTAG link would do for program diagnostics. It is
//   expected that output here will be ignored unless diagnosing problems.
// LinuxStdIO makes no distinction between STDOUT and STDERR. All output will be
//   via STDOUT.
// Input from STDIN will be read and dropped if it is not being observed.
LinuxStdIO console_adapter;


/*******************************************************************************
* The main function.                                                           *
*******************************************************************************/
int main(int argc, const char *argv[]) {
  platform.init();
  conf_path.concatf("~/.%s.conf", argv[0]);

  // Parse through all the command line arguments and flags...
  // Please note that the order matters. Put all the most-general matches at the bottom of the loop.
  for (int i = 1; i < argc; i++) {
    if ((strcasestr(argv[i], "--help")) || (strcasestr(argv[i], "-h"))) {
      printf("-h  --help          Print this output and exit.\n");
      printf("-i  --input         Specify an input file. If unsupplied, will default to the program binary.\n");
      printf("-c  --conf          Use a non-default conf blob.\n");
      printf("    --conf-dump     Load the program configuration, dump it, and exit.\n");
      printf("\n\n");
      exit(0);
    }
    else if (strcasestr(argv[i], "--conf-dump")) {
      // TODO: Something smarter needs to be done here to avoid dependence on argument order.
      StringBuilder tmp_str;
      StringBuilder serialized;
      printf("Serializing conf returns %d.\n", main_conf.serialize(&serialized, TCode::CBOR));
      serialized.printDebug(&tmp_str);
      main_conf.printConfRecord(&tmp_str);
      printf("%s\n", (char*) tmp_str.string());
      exit(0);
    }
    else if (argc - i >= 2) {    // Compound arguments go in this case block...
      if ((strcasestr(argv[i], "--conf")) || (strcasestr(argv[i], "-c"))) {
        if (argc - i < 2) {  // Mis-use of flag...
          printf("Using %s means you must supply a path to the desired conf blob.\n", argv[i]);
          exit(1);
        }
        conf_path.clear();
        conf_path.concat(argv[++i]);
      }
      else if ((strcasestr(argv[i], "--input")) || (strcasestr(argv[i], "-i"))) {
        if (argc - i < 2) {  // Mis-use of flag...
          printf("Using %s means you must supply a path to the blob you want to study.\n", argv[i]);
          exit(1);
        }
        input_path.clear();
        input_path.concat(argv[++i]);
      }
      else if ((strlen(argv[i]) > 3) && (argv[i][0] == '-') && (argv[i][1] == '-')) {
        i++;
      }
      else {
        i++;
      }
    }
    else {
      printf("Unhandled argument: %s\n", argv[i]);
    }
  }

  // Instance an X11 window.
  // NOTE: The window destructor will block until the GUI thread is shut down.
  //   So for the sake of enforcing thread termination order, we empty-scope the
  //   existance of the window such that the process will naturally wait for all
  //   the complicated cleanup to happen before it flushes its output buffer,
  //   and slams the door on the process.
  {
    MainGuiWindow c3p_root_window(0, 0, 1024, 768, argv[0]);
    if (0 == c3p_root_window.createWindow()) {
      // The window thread is running.
      StringBuilder output(PROGRAM_NAME);
      output.concatf(" v%s initialized\n\n", PROGRAM_VERSION);
      c3p_log(LOG_LEV_INFO, __PRETTY_FUNCTION__, &output);
      do {
        console_adapter.poll();     // The main loop. Run until told to stop.
      } while (continue_running);   // GUI thread handles the heavy-lifting.
    }
    else {
      c3p_log(LOG_LEV_ERROR, __PRETTY_FUNCTION__, "Failed to create the root GUI window (not great, not terrible).");
    }
  }

  console_adapter.poll();
  platform.firmware_shutdown(0);  // Clean up the platform.
  exit(0);
}
