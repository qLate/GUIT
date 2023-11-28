#pragma once
#include "Component.h"

class WindoW;

class SubComponent : public Component
{

public:
	Component* parent;

	wl_subsurface* subsurf = nullptr;

	glm::vec2 localPos;
	glm::vec2 anchorsMin = {0.5f, 0.5f};
	glm::vec2 anchorsMax = {0.5f, 0.5f};
	glm::vec2 anchoredPos;
	glm::vec2 pivot = {0.5f, 0.5f};

	glm::vec2 targetSize;


	SubComponent(Component* parent, glm::vec2 size = {100, 100}, wl_surface* parentSurf = nullptr);

	void setLocalPos(glm::vec2 pos);
	void setAnchoredPos(glm::vec2 pos);
	void setAnchors(glm::vec2 min, glm::vec2 max);
	void setPivot(glm::vec2 pivot);
	void updateSurfacePosition() const;

	void resize(glm::vec2 size) override;
	void resizeRec(glm::vec2 prevContainerSize, glm::vec2 newContainerSize);

	glm::vec2 getAnchorCenter() const;
	glm::vec2 getTopLeftPos() const override;;
};
