#include "Button.h"

#include "Text.h"

Button::Button(const std::string& text, Component* parent, glm::vec2 size) : SubComponent(parent, size)
{
	OnPointerDown += OnClick;

	if (!text.empty())
	{
		this->text = new Text(text, this, size / 1.2f);
		this->text->OnPointerDown += OnClick;
	}
}
Button::~Button()
{
	delete text;
}
void Button::setColor(Color color)
{
	SubComponent::setColor(color);

	if (text != nullptr)
		text->setColor(color);
}
