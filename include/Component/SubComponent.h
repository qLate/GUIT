#pragma once
#include "Component.h"

using pvv = std::pair<glm::vec2, glm::vec2>;

class Window;

class SubComponent : public Component
{
	wl_subsurface* subSurface = nullptr;

public:
	Component* parent;

	pvv anchors = {{0.5f, 0.5f}, {0.5f, 0.5f}};
	glm::vec2 pos = {0, 0};

	SubComponent(Component* parent, glm::vec2 size);

	void setPos(glm::vec2 pos);
	void resize(glm::vec2 prevSize, glm::vec2 newSize);
};
