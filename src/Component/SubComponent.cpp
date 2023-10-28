#include "SubComponent.h"

#include "GUIToolkit.h"
#include "Window.h"

SubComponent::SubComponent(Component* parent, glm::vec2 size, glm::vec2 anchors, glm::vec2 pos): Component(size), parent(parent), anchors(anchors), pos {pos}
{
	this->window = parent->window;

	if (dynamic_cast<Window*>(parent) != nullptr)
		window = (Window*)parent;
	else
		window = ((SubComponent*)parent)->window;

	subSurface = wl_subcompositor_get_subsurface(GUIToolkit::instance->subcompositor, surf, parent->surf);
	wl_subsurface_set_position(subSurface, pos.x, pos.y);
}
