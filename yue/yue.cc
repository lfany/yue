// Copyright 2016 Cheng Zhao. All rights reserved.
// Use of this source code is governed by the license that can be found in the
// LICENSE file.

#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"
#include "yue/builtin_loader.h"

#include "nativeui/events/keyboard_codes.h"

int main(int argc, const char *argv[]) {
  base::AtExitManager exit_manager;
  base::CommandLine::Init(argc, argv);

  logging::LoggingSettings settings;
  settings.logging_dest = logging::LOG_TO_SYSTEM_DEBUG_LOG;
  logging::InitLogging(settings);

  auto* cmd = base::CommandLine::ForCurrentProcess();
  if (cmd->GetArgs().size() != 1) {
    fprintf(stderr, "Usage: yue <path-to-script>\n");
    return 1;
  }

  lua::ManagedState state;
  if (!state) {
    fprintf(stderr, "Unable to create lua state\n");
    return 1;
  }

  luaL_openlibs(state);
  yue::InsertBuiltinModuleLoader(state);

#if defined(OS_WIN)
  std::string filename = base::UTF16ToUTF8(cmd->GetArgs()[0]);
#else
  std::string filename = cmd->GetArgs()[0];
#endif

  if (luaL_loadfile(state, filename.c_str()) != LUA_OK ||
      !lua::PCall(state, nullptr)) {
    std::string error;
    lua::Pop(state, &error);
    fprintf(stderr, "Error when running script: %s\n", error.c_str());
    return 1;
  }

  return 0;
}
