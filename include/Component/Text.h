#pragma once

#include <string>

#include "Color.h"
#include "SubComponent.h"

class Text : public SubComponent
{
public:
	std::string text;
	Color bgColor;

	Text(const std::string& text, Component* parent, glm::vec2 size);

	void setText(const std::string& text, float fontSize = 24, Color color = Color::black());

	void resize(glm::vec2 size) override;
	void setColor(Color color) override;
};
