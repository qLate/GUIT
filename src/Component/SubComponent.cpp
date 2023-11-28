#include "SubComponent.h"

#include "GUI.h"
#include "Window.h"
#include "gtx/string_cast.hpp"

SubComponent::SubComponent(Component* parent, glm::vec2 size, wl_surface* parentSurf): Component(size), parent(parent)
{
	this->window = parent->window;

	this->parentSurf = parentSurf != nullptr ? parentSurf : parent->surf;
	subsurf = wl_subcompositor_get_subsurface(GUI::subcompositor, surf, this->parentSurf);
	//wl_subsurface_set_desync(subsurf);

	SubComponent::resize(size);
	setAnchoredPos({0, 0});

	parent->subComponents.push_back(this);
}

void SubComponent::setLocalPos(glm::vec2 pos)
{
	this->localPos = pos;
	this->anchoredPos = pos - getAnchorCenter();
	this->pos = pos + parent->pos;

	updateSurfacePosition();
}
void SubComponent::setAnchoredPos(glm::vec2 pos)
{
	this->localPos = anchoredPos + getAnchorCenter();
	this->anchoredPos = pos;
	this->pos = localPos + parent->pos;

	updateSurfacePosition();
}
void SubComponent::setAnchors(glm::vec2 min, glm::vec2 max)
{
	this->anchorsMin = min;
	this->anchorsMax = max;
	anchoredPos = localPos - getAnchorCenter();
}
void SubComponent::setPivot(glm::vec2 pivot)
{
	auto oldPivot = this->pivot;
	this->pivot = pivot;

	setLocalPos(localPos + (pivot - oldPivot) * size);
}
void SubComponent::updateSurfacePosition() const
{
	if (!isActive) return;
	auto pivotedPos = localPos - pivot * size;
	wl_subsurface_set_position(subsurf, (int)pivotedPos.x, (int)pivotedPos.y);
}
void SubComponent::resize(glm::vec2 size)
{
	targetSize = size;

	Component::resize(min(size, parent->size));
}

void SubComponent::resizeRec(glm::vec2 prevContainerSize, glm::vec2 newContainerSize)
{
	glm::vec2 factor = (newContainerSize - prevContainerSize) / prevContainerSize * (anchorsMax - anchorsMin);
	glm::vec2 newSize = targetSize + targetSize * factor;
	resize(newSize);

	this->localPos = anchoredPos + getAnchorCenter();
	updateSurfacePosition();
}

void SubComponent::setActive(bool isActive)
{
	if (this->isActive == isActive) return;
	this->isActive = isActive;

	if (!isActive)
	{
		wl_subsurface_destroy(subsurf);
	}
	else
	{
		subsurf = wl_subcompositor_get_subsurface(GUI::subcompositor, surf, parentSurf);
	}

	for (const auto& subComponent : subComponents)
		subComponent->setActive(isActive);
}

glm::vec2 SubComponent::getAnchorCenter() const
{
	return (anchorsMax + anchorsMin) / 2.0f * parent->size + size * (pivot - glm::vec2(0.5f, 0.5f));
}
glm::vec2 SubComponent::getTopLeftPos() const
{
	return pos - pivot * size;
}
