#include "Utils.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define  STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Component.h"
#include "GUIToolkit.h"

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

	for (int i = 0; i < w * h * 4; i += 4)
	{
		std::swap(image[i], image[i + 2]);
	}

	stbi_image_free(data);
	return (data != nullptr);
}


void Utils::resizeSurface(glm::vec2 oldSize, glm::vec2 newSize, int& capacity, wl_surface* surf, wl_buffer*& buf, uint8_t*& pixels, wl_shm_pool*& pool)
{
	int len = oldSize.x * oldSize.y * 4;
	if (len != 0)
		wl_buffer_destroy(buf);

	int len_new = (int)newSize.x * (int)newSize.y * 4;
	if (len_new > capacity || len_new < capacity / 3)
	{
		if (pool != nullptr)
		{
			wl_shm_pool_destroy(pool);
			munmap(pixels, capacity);
		}

		capacity = len_new * 2;
		auto fd = shm_alloc(capacity);
		pixels = (uint8_t*)mmap(nullptr, capacity, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		pool = wl_shm_create_pool(GUIToolkit::sharedMemory, fd, capacity);
	}

	buf = wl_shm_pool_create_buffer(pool, 0, (int)newSize.x, (int)newSize.y, (int)newSize.x * 4, WL_SHM_FORMAT_ABGR8888);
	wl_surface_attach(surf, buf, 0, 0);
}
