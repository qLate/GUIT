#include <string.h>

#include "GUIToolkit.h"
#include "Window.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "filesystem"

bool readImage(std::vector<uint8_t>& image, const std::filesystem::path& filename, int& w, int& h)
{
	int n;
	unsigned char* data = stbi_load(filename.string().c_str(), &w, &h, &n, 4);
	if (data != nullptr)
		image = std::vector(data, data + w * h * 4);

	stbi_image_free(data);
	return (data != nullptr);
}

int main()
{
	GUIToolkit toolkit {};

	Window w1 {"Test window", 512, 512};
	w1.fillColor(Color::red());
	Window w2 {"Test window 2", 512, 512};
	w2.fillColor(Color::blue());

	std::vector<uint8_t> image;
	int w, h;
	readImage(image, "../images/sviat.jpg", w, h);
	w1.resize(w, h);
	memcpy(w1.pixels, image.data(), image.size());

	toolkit.loop();

	return 0;
}
