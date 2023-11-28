#include "Button.h"
#include "GUI.h"
#include "Component.h"
#include "InputField.h"
#include "Text.h"
#include "Window.h"

int main()
{
	GUI toolkit {};

	WindoW w1 {"Test window", {700, 700}};
	w1.setColor(Color::gray());
	w1.nameID = "w1";

	InputField i1 {&w1, {600, 100}};

	//Button b1 {"click me!", &w1, {400, 100}};
	//b1.setColor(Color::white());
	//b1.OnClick += [&b1]
	//{
	//	b1.setColor({rand() % 100 / 100.0f, rand() % 100 / 100.0f, rand() % 100 / 100.0f});
	//};


	//SubComponent s1 {&w1, w1.size};
	//s1.nameID = "s1";
	//s1.setImage("../images/sviat.jpg");
	//s1.setAnchors({0, 0}, {1, 1});
	//s1.setPivot({0, 0});

	//SubComponent s2 {&s1, s1.size / 1.2f};
	//s2.nameID = "s2";
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
