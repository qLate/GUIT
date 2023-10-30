#include "GUIToolkit.h"
#include "Component.h"

#include "Subcomponent.h"
#include "Window.h"


int main()
{
	GUIToolkit toolkit {};

	Window w1 {"Test window", {500, 500}};
	w1.setImage("../images/sviat.jpg");

	//SubComponent s1 {&w1, {w1.size.x, 250}};
	//s1.doMoveWindow = true;
	//s1.setAnchors({0.5f, 0.0f}, {0.5f, 1.0f});
	//s1.setColor(Color::blue());

	toolkit.loop();
	return 0;
}
