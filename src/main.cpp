#include "GUIToolkit.h"
#include "Component.h"
#include "Window.h"
#include "SubComponent.h"

int main()
{
	GUIToolkit toolkit {};

	WindowW w1 {"Test window", {500, 500}};
	w1.setColor(Color::red());
	//w1.resize({ 500, 500 });

	SubComponent s1 {&w1, w1.size / 2.0f};
	s1.moveWindowOnDrag = true;
	s1.setAnchors({0, 0.5f}, {1, 0.5f});
	s1.setImage("../images/sviat.jpg");

	toolkit.loop();

	return 0;
}
