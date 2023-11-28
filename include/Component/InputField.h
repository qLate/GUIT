#pragma once
#include "SubComponent.h"

class Text;

class InputField : SubComponent
{
public:
	Text* text;

	InputField(Component* parent, glm::vec2 size);
	~InputField() override;

	void setColor(Color color) override;

	std::string getValue() const;

	bool isFocused = false;
	void onFocus() override;
	void onFocusLost() override;

	void onKeyDown(int key) const;

	//bool doPreserveAspect() const override { return true; }
};
