#include "SubComponent.h"

#include "GUIToolkit.h"
#include "Window.h"

SubComponent::SubComponent(Component* parent, glm::vec2 size): Component(size), parent(parent)
{
	this->window = parent->window;

	subSurface = wl_subcompositor_get_subsurface(GUIToolkit::instance->subcompositor, surf, parent->surf);
	setAnchoredPos({0, 0});

	parent->subComponents.push_back(this);
}

void SubComponent::setPos(glm::vec2 pos)
{
	this->pos = pos;
	this->anchoredPos = pos - getAnchorCenter();

	updateSurfacePosition();
}
void SubComponent::setAnchoredPos(glm::vec2 pos)
{
	this->anchoredPos = pos;
	this->pos = anchoredPos + getAnchorCenter();

	updateSurfacePosition();
}
void SubComponent::setAnchors(glm::vec2 min, glm::vec2 max)
{
	this->anchorsMin = min;
	this->anchorsMax = max;
	anchoredPos = pos - getAnchorCenter();
}
void SubComponent::setPivot(glm::vec2 pivot)
{
	auto oldPivot = pivot;
	this->pivot = pivot;

	setPos(pos + (pivot - oldPivot) * size);
}
void SubComponent::updateSurfacePosition() const
{
	auto pivotedPos = pos - pivot * size;
	wl_subsurface_set_position(subSurface, pivotedPos.x, pivotedPos.y);
}

void SubComponent::resize(glm::vec2 prevContainerSize, glm::vec2 newContainerSize)
{
	glm::vec2 factor = (newContainerSize - prevContainerSize) / prevContainerSize * (anchorsMax - anchorsMin);
	glm::vec2 newSize = size + size * factor;
	resizeSurface(newSize);

	auto newAnchorCenter = (anchorsMax + anchorsMin) / 2.0f * newContainerSize;
	this->pos = anchoredPos + newAnchorCenter;
	updateSurfacePosition();
}

glm::vec2 SubComponent::getAnchorCenter() const
{
	return (anchorsMax + anchorsMin) / 2.0f * parent->size;
}
