#include <string.h>

#include "GUIToolkit.h"
#include "Component.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "filesystem"
#include "Subcomponent.h"
#include "Window.h"

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

	std::vector<uint8_t> image;
	int w, h;
	readImage(image, "../images/sviat.jpg", w, h);

	Window w1 {"Test window", {w, h}};
	memcpy(w1.pixels, image.data(), image.size());

	SubComponent s1 {&w1, {w, h / 4}};
	s1.isWindowMoveable = true;
	s1.setAnchors({0, 0.5f}, {1, 0.5f});
	s1.fillColor(Color::blue());

	toolkit.loop();
	return 0;
}
