#include "GUIToolkit.h"
#include "Component.h"
#include "Window.h"
#include "SubComponent.h"

#include "cairo-gl.h"

int main()
{
    GUIToolkit toolkit {};

	Window w1 {"Test window", {1, 1}};
	w1.setImage("../images/sviat.jpg");
    w1.resize({500, 500});

	SubComponent s1 {&w1, {w1.size.x/2, 100}};
	s1.moveWindowOnDrag = true;
	s1.setAnchors({0, 0.5f}, {1, 0.5f});
	s1.setColor(Color::blue());

	toolkit.loop();

	return 0;
}