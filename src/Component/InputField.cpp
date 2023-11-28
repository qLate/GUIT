#include "InputField.h"

#include "GUI.h"
#include "Text.h"

InputField::InputField(Component* parent, glm::vec2 size): SubComponent(parent, size)
{
	text = new Text("asdasd", this, size);
	text->OnFocus += [this] { onFocus(); };
	text->OnFocusLost += [this] { onFocusLost(); };

	GUI::onKeyDown += [this](int key, int) { onKeyDown(key); };
}
InputField::~InputField()
{
	delete text;
}

void InputField::setColor(Color color)
{
	SubComponent::setColor(color);

	text->setColor(color);
}

std::string InputField::getValue() const
{
	return text->text;
}

void InputField::onFocus()
{
	isFocused = true;

	GUI::allowSwitchFullscreen = false;
}
void InputField::onFocusLost()
{
	isFocused = false;

	GUI::allowSwitchFullscreen = true;
}

void InputField::onKeyDown(int key) const
{
	if (!isFocused) return;

	if (key == KEY_BACKSPACE)
	{
		if (text->text.size() > 0)
			text->setText(text->text.substr(0, text->text.size() - 1));
	}
	else
	{
		auto keyChar = Utils::keyToChar(key);
		text->setText(text->text + keyChar);
	}
}
