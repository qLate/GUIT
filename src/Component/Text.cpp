#include "Text.h"

Text::Text(const std::string& text, Component* parent, glm::vec2 size): SubComponent(parent, size)
{
	this->scaleImage = true;
	
	setText(text);
}

void Text::setText(const std::string& text, float fontSize, Color color)
{
	this->text = text;

	SubComponent::setColor(bgColor);

	cairo_t* cr = cairo_create(rSurf);

	cairo_select_font_face(cr, "cairo:serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, fontSize * size.y / 50.0f);

	cairo_text_extents_t extents;
	cairo_text_extents(cr, text.c_str(), &extents);

	auto pos = size / 2.0f;
	pos -= glm::vec2(extents.width / 2 + extents.x_bearing, extents.height / 2 + extents.y_bearing);
	cairo_move_to(cr, pos.x, pos.y);
	cairo_set_source_rgb(cr, color.r, color.g, color.b);
	cairo_show_text(cr, text.c_str());

	cairo_destroy(cr);
}
void Text::resize(glm::vec2 size)
{
	SubComponent::resize(size);

	imageSize = size;
}
void Text::setColor(Color color)
{
	bgColor = color;

	setText(text);
}
