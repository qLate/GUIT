#pragma once

#include <filesystem>
#include <vector>

class Utils
{
public:
	static int32_t shm_alloc(uint64_t sz);

	static bool readImage(std::vector<uint8_t>& image, const std::filesystem::path& filename, int& w, int& h);
};
