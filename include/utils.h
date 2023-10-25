#pragma once

#include <fcntl.h>

class Utils
{
public:
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
};
