#include "GUI.h"
#include "Component.h"
#include "Window.h"
#include "SubComponent.h"

int main()
{
	GUI toolkit {};

	WindoW w1 {"Test window", {800, 800}};
	w1.setColor(Color::black());

	SubComponent s1 {&w1, w1.size};
	s1.setAnchors({0, 0}, {1, 1});
	s1.setImage("../images/sviat.jpg");
	s1.setPivot({0, 0});

	toolkit.loop();

	return 0;
}
