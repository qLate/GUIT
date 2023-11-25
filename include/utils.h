#pragma once

#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <sys/mman.h>

#include "stb_image.h"

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

	static int32_t shm_alloc(uint64_t sz)
	{
		char name[8];
		name[0] = '/';
		name[7] = 0;
		for (uint8_t i = 1; i < 6; i++)
		{
			name[i] = (rand() & 23) + 97;
		}

		int32_t fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH);
		shm_unlink(name);
		ftruncate(fd, sz);
		return fd;
	}
	static bool readImage(std::vector<uint8_t>& image, const std::filesystem::path& filename, int& w, int& h)
	{
		int n;
		unsigned char* data = stbi_load(filename.string().c_str(), &w, &h, &n, 4);
		if (data != nullptr)
			image = std::vector(data, data + w * h * 4);

		stbi_image_free(data);
		return (data != nullptr);
	}
};
