#include "SubComponent.h"

#include "Debug.h"
#include "GUI.h"
#include "Window.h"
#include "vec2.hpp"

SubComponent::SubComponent(Component* parent, glm::vec2 size): Component(size), parent(parent)
{
	this->window = parent->window;

	subsurf = wl_subcompositor_get_subsurface(GUI::subcompositor, surf, parent->surf);

	Component::resize(size);
	setAnchoredPos({0, 0});

	parent->subComponents.push_back(this);
}

void SubComponent::setLocalPos(glm::vec2 localPos)
{
	this->localPos = localPos;
	this->anchoredPos = localPos - getAnchorCenter();
	this->pos = localPos + parent->pos;

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
	auto oldPivot = pivot;
	this->pivot = pivot;

	setLocalPos(localPos + (pivot - oldPivot) * size);
}
void SubComponent::updateSurfacePosition() const
{
	auto pivotedPos = localPos - pivot * size;
	wl_subsurface_set_position(subsurf, (int)pivotedPos.x, (int)pivotedPos.y);
}

void SubComponent::resizeRec(glm::vec2 prevContainerSize, glm::vec2 newContainerSize)
{
	Debug::funcEntered(__FUNCTION__);
	glm::vec2 factor = (newContainerSize - prevContainerSize) / prevContainerSize * (anchorsMax - anchorsMin);
	glm::vec2 newSize = size + size * factor;
	resize(newSize);

	auto newAnchorCenter = (anchorsMax + anchorsMin) / 2.0f * newContainerSize;
	this->localPos = anchoredPos + newAnchorCenter;
	updateSurfacePosition();
	Debug::funcExit(__FUNCTION__);
}

glm::vec2 SubComponent::getAnchorCenter() const
{
	return (anchorsMax + anchorsMin) / 2.0f * parent->size;
}
glm::vec2 SubComponent::getTopLeftPos() const
{
	return pos - pivot * size;
}
