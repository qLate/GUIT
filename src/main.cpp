#include "Button.h"
#include "GUI.h"
#include "Component.h"
#include "InputField.h"
#include "Text.h"
#include "Window.h"

void setup1()
{
	GUI toolkit{};

	WindoW w1 {"Test window", {800, 800}};
	w1.setColor(Color::gray());
	w1.nameID = "w1";

	SubComponent s1 {&w1, w1.size};
	s1.nameID = "s1";
	s1.setImage("../images/sviat.jpg");
	s1.setAnchors({0, 0}, {1, 1});
	s1.setPivot({0, 0});

	SubComponent s2 {&s1, s1.size / 1.2f};
	s2.setAnchors({0, 0}, {1, 1});
	s2.setColor({rand() % 256 / 256.0f, rand() % 256 / 256.0f, rand() % 256 / 256.0f, 1});

	SubComponent s3 {&s1, s2.size / 1.2f};
	s3.setColor({rand() % 256 / 256.0f, rand() % 256 / 256.0f, rand() % 256 / 256.0f, 1});

	Button b1 {"click me!", &s3, {400, 100}};
	b1.setColor(Color::white());
	b1.OnClick += [&b1]
	{
		b1.setColor({rand() % 100 / 100.0f, rand() % 100 / 100.0f, rand() % 100 / 100.0f});
	};

	toolkit.loop();
}

void setup2()
{
	GUI toolkit{};

	WindoW w1{ "Test window", {800, 800} };
	w1.setColor(Color::gray());
	w1.nameID = "w1";

	SubComponent s1{ &w1, w1.size };
	s1.nameID = "s1";
	s1.setImage("../images/sviat.jpg");
	s1.setAnchors({ 0, 0 }, { 1, 1 });
	s1.setPivot({ 0, 0 });

	toolkit.loop();
}

int main()
{
	setup1();

	return 0;
}
