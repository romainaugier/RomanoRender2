#pragma once

#include "renderer/render.h"
#include "utils/glutils.h"
#include "scene/camera.h"
#include "renderer/renderer.h"
#include "application/node_editor.h"
#include "application/outliner.h"
#include "application/shortcuts.h"
#include "application/code_editor.h"
#include "application/menubar.h"

#include "OSL/oslexec.h"

#ifdef _WIN32
#include <Windows.h>
#include <WinUser.h>
#endif

int application(int argc, char** argv);