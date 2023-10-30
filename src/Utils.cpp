#include "Utils.h"

#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int32_t Utils::shm_alloc(uint64_t sz)
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
bool Utils::readImage(std::vector<uint8_t>& image, const std::filesystem::path& filename, int& w, int& h)
{
	int n;
	unsigned char* data = stbi_load(filename.string().c_str(), &w, &h, &n, 4);
	if (data != nullptr)
		image = std::vector(data, data + w * h * 4);

	stbi_image_free(data);
	return (data != nullptr);
}
