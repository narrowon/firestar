#pragma once

#pragma warning (push)
#pragma warning (disable : 26819)

#include "libs/json/include/json.hpp"
#include "libs/minhook/include/minhook.hpp"
#include "libs/imgui/imgui.hpp"
#include "libs/imgui/imgui_impl_dx9.hpp"
#include "libs/imgui/imgui_impl_win32.hpp"
#include "libs/imgui/imgui_freetype.hpp"
#include <d3d9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "freetype.lib")

#pragma warning(pop)


#include "utils/color.hpp"
#include "utils/funcs.hpp"
#include "utils/hash.hpp"
#include "utils/insttracker.hpp"
#include "utils/math.hpp"
#include "utils/mem.hpp"

#include <functional>
#include <fstream>
#include <string>
#include <unordered_set>
#include <regex>
#include <intrin.h>
#include <deque>
#include <random>