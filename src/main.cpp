#include "GUI.h"
#include "Component.h"
#include "Window.h"
#include "SubComponent.h"

int main()
{
	GUI toolkit {};

	WindoW w1 {"Test window", {800, 800}};
	w1.setColor(Color::black());
	w1.nameID = "w1";

	SubComponent s1 {&w1, w1.size};
	s1.nameID = "s1";
	s1.setAnchors({0, 0}, {1, 1});
	s1.setImage("../images/sviat.jpg");
	s1.setPivot({0, 0});

	//SubComponent s2 {&s1, s1.size / 1.2f};
	//s2.setAnchors({0, 0}, {1, 1});
	//s2.setColor({rand() % 256 / 256.0f, rand() % 256 / 256.0f, rand() % 256 / 256.0f, 1});

	//SubComponent s3 {&s1, s2.size / 1.2f};
	//s3.setColor({ rand() % 256 / 256.0f, rand() % 256 / 256.0f, rand() % 256 / 256.0f, 1 });

	//SubComponent s4 {&s1, s3.size / 1.2f};
	//s4.setColor({ rand() % 256 / 256.0f, rand() % 256 / 256.0f, rand() % 256 / 256.0f, 1 });

	//SubComponent s5 {&s1, s4.size / 1.2f};
	//s5.setColor({ rand() % 256 / 256.0f, rand() % 256 / 256.0f, rand() % 256 / 256.0f, 1 });

	//SubComponent s6 {&s1, s5.size / 1.2f};
	//s6.setColor({ rand() % 256 / 256.0f, rand() % 256 / 256.0f, rand() % 256 / 256.0f, 1 });

	toolkit.loop();

	return 0;
}
