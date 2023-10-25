#include "GUIToolkit.h"
#include "Window.h"

int main()
{
	GUIToolkit toolkit {};

	Window w1 {"Test window", 512, 512};
	w1.fillColor(Color::red());
	Window w2 {"Test window 2", 512, 512};
	w2.fillColor(Color::blue());

	toolkit.loop();

	return 0;
}
