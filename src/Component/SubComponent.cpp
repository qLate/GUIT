#include "SubComponent.h"

#include "GUIToolkit.h"
#include "Window.h"

SubComponent::SubComponent(Component* parent, glm::vec2 size): Component(size), parent(parent)
{
	this->window = parent->window;
	parent->subComponents.push_back(this);

	subSurface = wl_subcompositor_get_subsurface(GUIToolkit::instance->subcompositor, surf, parent->surf);
}

void SubComponent::setPos(glm::vec2 pos)
{
	this->pos = pos;

	wl_subsurface_set_position(subSurface, pos.x, pos.y);
}

void SubComponent::resize(glm::vec2 prevSize, glm::vec2 newSize)
{
	glm::vec2 factor = newSize / prevSize;
	glm::vec2 newSelfSize = size * factor;

	resizeSurface(newSelfSize);
}
