#pragma once
#include "Component.h"

class Window;

class SubComponent : public Component
{
	wl_subsurface* subsurf = nullptr;

public:
	Component* parent;

	glm::vec2 pos;
	glm::vec2 anchorsMin = {0.5f, 0.5f};
	glm::vec2 anchorsMax = {0.5f, 0.5f};
	glm::vec2 anchoredPos;
	glm::vec2 pivot = {0.5f, 0.5f};

	SubComponent(Component* parent, glm::vec2 size);

	void setPos(glm::vec2 pos);
	void setAnchoredPos(glm::vec2 pos);
	void setAnchors(glm::vec2 min, glm::vec2 max);
	void setPivot(glm::vec2 pivot);
	void updateSurfacePosition() const;

	void resizeRec(glm::vec2 prevContainerSize, glm::vec2 newContainerSize);

	glm::vec2 getAnchorCenter() const;
};
