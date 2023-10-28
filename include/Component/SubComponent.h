#pragma once
#include "Component.h"

class Window;

class SubComponent : public Component
{
	wl_subsurface* subSurface = nullptr;

public:
	Component* parent;

	glm::vec2 anchors, pos;

	SubComponent(Component* parent, glm::vec2 size, glm::vec2 anchors, glm::vec2 pos);
};
