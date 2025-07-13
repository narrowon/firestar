#pragma once

#include <Windows.h>
#include <cmath>
#include <string>

#undef min
#undef max

#include "color.hpp"

namespace utils
{
	std::wstring utf8ToWide(std::string_view str);
	Color hsvToRGB(float H, float S, float V);
	void rgbToHSV(const Color rgb, float &h_out, float &s_out, float &v_out);
	Color rainbow(const float t, const uint8_t a = 255);
	void copyToClipboard(std::string_view str);
}