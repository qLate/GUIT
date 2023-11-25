#pragma once

#include <filesystem>
#include <iostream>
#include <vector>

using std::string;
using std::vector;
using std::cout;


class Utils
{
public:
	inline static const vector<string> CURSOR_NAMES = {
		"left_ptr", "top_side", "bottom_side", "", "left_side", "top_left_corner",
		"bottom_left_corner", "", "right_side", "top_right_corner", "bottom_right_corner"
	};


	static int32_t shm_alloc(uint64_t sz);

	static bool readImage(std::vector<uint8_t>& image, const std::filesystem::path& filename, int& w, int& h);
};
