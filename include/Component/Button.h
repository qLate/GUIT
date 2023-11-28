#pragma once

#include "Component.h"
#include "SubComponent.h"

class Text;

class Button : public SubComponent
{

public:
	Text* text;
	Action<> OnClick {};

	Button(const std::string& text, Component* parent, glm::vec2 size);
	~Button() override;

	void setColor(Color color) override;
};
