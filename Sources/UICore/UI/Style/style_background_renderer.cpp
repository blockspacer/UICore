/*
**  UICore
**  Copyright (c) 1997-2015 The UICore Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries UICore may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Magnus Norddahl
*/

#include "UICore/precomp.h"
#include "style_background_renderer.h"
#include "UICore/UI/View/view_geometry.h"
#include "UICore/UI/Style/style_cascade.h"
#include "UICore/UI/Style/style_get_value.h"
#include "UICore/UI/UIThread/ui_thread.h"
#include "UICore/Display/2D/canvas.h"
#include "UICore/Display/2D/image.h"
#include "UICore/Display/2D/path.h"
#include "UICore/Display/2D/brush.h"
#include "UICore/UI/Image/image_source.h"
#include "UICore/Core/Text/text.h"
#include "UICore/Core/Math/line.h"

namespace uicore
{
	StyleBackgroundRenderer::StyleBackgroundRenderer(const std::shared_ptr<Canvas> &canvas, const ViewGeometry &geometry, const StyleCascade &style) : canvas(canvas), geometry(geometry), style(style)
	{
	}

	void StyleBackgroundRenderer::render_background()
	{
		render_box_shadow();

		int num_layers = style.array_size("background-image");

		StyleGetValue bg_color = style.computed_value("background-color");
		if (bg_color.is_color() && bg_color.color().w != 0.0f)
		{
			auto border_points = get_border_points();

			if (is_render_border_antialias_fix_required())
			{
				float delta = 0.5f;
				border_points[0] += Pointf(delta, delta);
				border_points[1] += Pointf(-delta, delta);
				border_points[2] += Pointf(-delta, delta);
				border_points[3] += Pointf(-delta, -delta);
				border_points[4] += Pointf(-delta, -delta);
				border_points[5] += Pointf(delta, -delta);
				border_points[6] += Pointf(delta, -delta);
				border_points[7] += Pointf(delta, delta);
			}

			// To do: take get_layer_clip(num_layers - 1) into account

			std::shared_ptr<Path> background_area = get_border_area_path(border_points);
			background_area->fill(canvas, Brush(bg_color.color()));
		}

		for (int index = num_layers - 1; index >= 0; index--)
		{
			StyleGetValue layer_image = style.computed_value("background-image[" + Text::to_string(index) + "]");
			if (layer_image.is_keyword("none"))
				continue;

			if (layer_image.is_url())
			{
				render_background_image(layer_image, index);
			}
			else if (layer_image.is_keyword("linear-gradient"))
			{
				render_background_linear_gradient(index);
			}
			else if (layer_image.is_keyword("radial-gradient"))
			{
				render_background_radial_gradient(index);
			}
			else if (layer_image.is_keyword("repeating-linear-gradient"))
			{
				render_background_repeating_linear_gradient(index);
			}
			else if (layer_image.is_keyword("repeating-radial-gradient"))
			{
				render_background_repeating_radial_gradient(index);
			}
		}
	}

	void StyleBackgroundRenderer::render_background_image(const StyleGetValue &layer_image, int index)
	{
		std::shared_ptr<Image> image;

		if (layer_image.is_url())
			image = ImageSource::from_resource(layer_image.text())->image(canvas);

		if (image)
		{
			Rectf clip_box = get_clip_box(index);
			Rectf origin_box = get_origin_box(index);
			Sizef image_size = get_image_size(index, image, origin_box);

			//canvas->push_cliprect(clip_box);

			StyleGetValue repeat_x = get_layer_repeat_x(index);
			StyleGetValue repeat_y = get_layer_repeat_y(index);

			float y = get_start_y(index, clip_box, origin_box, image_size);
			while (true)
			{
				float x = get_start_x(index, clip_box, origin_box, image_size);
				while (true)
				{
					Rectf image_source(0.0f, 0.0f, image->size());
					Rectf image_dest(x, y, image_size);

					if (image_dest.left < clip_box.left)
					{
						float delta = clip_box.left - image_dest.left;
						image_source.left += delta * image->width() / image_size.width;
						image_dest.left += delta;
					}
					if (image_dest.right > clip_box.right)
					{
						float delta = clip_box.right - image_dest.right;
						image_source.right += delta * image->width() / image_size.width;
						image_dest.right += delta;
					}
					if (image_dest.top < clip_box.top)
					{
						float delta = clip_box.top - image_dest.top;
						image_source.top += delta * image->height() / image_size.height;
						image_dest.top += delta;
					}
					if (image_dest.bottom > clip_box.bottom)
					{
						float delta = clip_box.bottom - image_dest.bottom;
						image_source.bottom += delta * image->height() / image_size.height;
						image_dest.bottom += delta;
					}

					image->draw(canvas, image_source, image_dest);
	
					if (repeat_x.is_keyword("no-repeat"))
					{
						break;
					}
					else if (repeat_x.is_keyword("repeat"))
					{
						x += image_size.width;
					}
					else if (repeat_x.is_keyword("space"))
					{
						x += image_size.width;
					}
					else if (repeat_x.is_keyword("round"))
					{
						x += image_size.width;
					}

					if (x >= clip_box.right)
						break;
				}

				if (repeat_y.is_keyword("no-repeat"))
				{
					break;
				}
				else if (repeat_y.is_keyword("repeat"))
				{
					y += image_size.height;
				}
				else if (repeat_y.is_keyword("space"))
				{
					y += image_size.height;
				}
				else if (repeat_y.is_keyword("round"))
				{
					y += image_size.height;
				}

				if (y >= clip_box.bottom)
					break;
			}

			//canvas->pop_cliprect();
		}
	}

	void StyleBackgroundRenderer::render_background_linear_gradient(int index)
	{
		std::string prop_name = "background-image[" + Text::to_string(index) + "]";

		auto prop_angle = style.computed_value(prop_name + ".angle");
		int num_stops = style.array_size(prop_name + ".stop");
		if (num_stops <= 0)
			return;

		Rectf clip_box = get_clip_box(index);
		Pointf center = clip_box.center();

		Brush brush;
		brush.type = BrushType::linear;

		if (prop_angle.is_angle())
		{
			float angle = std::fmod(prop_angle.number(), 2 * PI);
			if (angle < 0.0f)
				angle += 2 * PI;

			float dx = std::sin(angle);
			float dy = -std::cos(angle);

			Pointf corner1, corner2;

			int corner_index = (int)(angle * 2 / PI);
			switch (corner_index)
			{
			default:
			case 0:
				corner1 = clip_box.bottom_left();
				corner2 = clip_box.top_right();
				break;
			case 1:
				corner1 = clip_box.top_left();
				corner2 = clip_box.bottom_right();
				break;
			case 2:
				corner1 = clip_box.top_right();
				corner2 = clip_box.bottom_left();
				break;
			case 3:
				corner1 = clip_box.bottom_right();
				corner2 = clip_box.top_left();
				break;
			}

			bool intersect = false;
			brush.start_point = Line2f(center, center + Vec2f(dx, dy)).intersection(Line2f(corner1, corner1 + Vec2f(-dy, dx)), intersect);
			brush.end_point = Line2f(center, center + Vec2f(dx, dy)).intersection(Line2f(corner2, corner2 + Vec2f(-dy, dx)), intersect);
		}
		else if (prop_angle.is_keyword("top-left"))
		{
			brush.end_point = Pointf(clip_box.right, clip_box.bottom);
			brush.start_point = Pointf(clip_box.left, clip_box.top);
		}
		else if (prop_angle.is_keyword("top-right"))
		{
			brush.end_point = Pointf(clip_box.left, clip_box.bottom);
			brush.start_point = Pointf(clip_box.right, clip_box.top);
		}
		else if (prop_angle.is_keyword("bottom-left"))
		{
			brush.start_point = Pointf(clip_box.right, clip_box.top);
			brush.end_point = Pointf(clip_box.left, clip_box.bottom);
		}
		else if (prop_angle.is_keyword("bottom-right"))
		{
			brush.start_point = Pointf(clip_box.left, clip_box.top);
			brush.end_point = Pointf(clip_box.right, clip_box.bottom);
		}
		else
		{
			return;
		}

		float gradient_length = (brush.end_point - brush.start_point).length();
		if (gradient_length <= 0.0f)
			return;

		std::shared_ptr<Path> border_area_path = get_border_area_path(get_border_points());

		float last_position = 0.0f;
		for (int stop_index = 0; stop_index < num_stops; stop_index++)
		{
			std::string stop_prop_name = prop_name + ".stop[" + Text::to_string(stop_index) + "]";

			auto prop_color = style.computed_value(stop_prop_name);
			auto prop_position = style.computed_value(stop_prop_name + ".position");

			float position = 0.0f;
			if (prop_position.is_number())
				position = prop_position.number();
			else if (prop_position.is_percentage())
				position = prop_position.number() / 100.0f;
			else if (prop_position.is_length())
				position = prop_position.number() / gradient_length;
			else if (stop_index == 0)
				position = 0.0f;
			else if (stop_index + 1 == num_stops)
				position = 1.0f;

			// to do: support evenly spread positions if position isn't specified for stops

			if (stop_index > 0)
				position = std::max(position, last_position);

			last_position = position;

			brush.stops.push_back(BrushGradientStop(prop_color.color(), position));
		}

		border_area_path->fill(canvas, brush);
	}

	void StyleBackgroundRenderer::render_background_radial_gradient(int index)
	{

	}

	void StyleBackgroundRenderer::render_background_repeating_linear_gradient(int index)
	{

	}

	void StyleBackgroundRenderer::render_background_repeating_radial_gradient(int index)
	{

	}

	void StyleBackgroundRenderer::render_border()
	{
		int num_layers = style.array_size("background-image");
		if (!get_layer_clip(num_layers - 1).is_keyword("border-box"))
			return;

		const char *style_names[4] = { "border-top-style", "border-right-style", "border-bottom-style", "border-left-style" };
		const char *color_names[4] = { "border-top-color", "border-right-color", "border-bottom-color", "border-left-color" };

		std::array<Pointf, 2 * 4> border_points, padding_points;
		bool points_calculated = false;

		for (int i = 0; i < 4; i++)
		{
			StyleGetValue border_style = style.computed_value(style_names[i]);
			if (border_style.is_keyword("solid"))
			{
				Colorf color = style.computed_value(color_names[i]).color();
				if (color.w > 0.0f)
				{
					if (!points_calculated)
					{
						border_points = get_border_points();
						padding_points = get_padding_points(border_points);
						points_calculated = true;
					}

					std::shared_ptr<Path> border_path = get_border_stroke_path(border_points, padding_points, i, i + 1);
					border_path->fill(canvas, Brush(color));
				}
			}
		}
	}

	bool StyleBackgroundRenderer::is_render_border_antialias_fix_required()
	{
		StyleGetValue style_top = style.computed_value("border-top-style");
		if (style_top.is_keyword("solid"))
		{
			Colorf color = style.computed_value("border-top-color").color();
			if (color.w > 0.5f)
				return true;
		}
		return false;
	}

	float StyleBackgroundRenderer::get_start_x(int index, const Rectf &clip_box, const Rectf &origin_box, const Sizef &image_size)
	{
		float x;
		if (get_layer_repeat_x(index).is_keyword("space") && image_size.width * 2 > origin_box.width())
		{
			x = origin_box.left;
		}
		else
		{
			x = origin_box.left;
			StyleGetValue pos_x = get_layer_position_x(index);
			if (pos_x.is_keyword("left"))
				x = origin_box.left;
			else if (pos_x.is_keyword("center"))
				x = origin_box.left + (origin_box.width() - image_size.width) / 2;
			else if (pos_x.is_keyword("right"))
				x = origin_box.right - image_size.width;
			else if (pos_x.is_percentage())
				x = origin_box.left + (origin_box.width() - image_size.width) * pos_x.number() / 100.0f;
			else if (pos_x.is_length())
				x = origin_box.left + pos_x.number();
		}

		StyleGetValue repeat_x = get_layer_repeat_x(index);
		if (repeat_x.is_keyword("repeat") || repeat_x.is_keyword("space"))
		{
			if (x > clip_box.left)
				x = clip_box.left - std::fmod(image_size.width - (x - clip_box.left), image_size.width);
		}

		return x;
	}

	float StyleBackgroundRenderer::get_start_y(int index, const Rectf &clip_box, const Rectf &origin_box, const Sizef &image_size)
	{
		float y;
		if (get_layer_repeat_y(index).is_keyword("space") && image_size.height * 2 > origin_box.height())
		{
			y = origin_box.top;
		}
		else
		{
			y = origin_box.top;
			StyleGetValue pos_y = get_layer_position_y(index);
			if (pos_y.is_keyword("top"))
				y = origin_box.top;
			else if (pos_y.is_keyword("center"))
				y = origin_box.top + (origin_box.height() - image_size.height) / 2;
			else if (pos_y.is_keyword("bottom"))
				y = origin_box.bottom - image_size.height;
			else if (pos_y.is_percentage())
				y = origin_box.top + (origin_box.height() - image_size.height) * pos_y.number() / 100.0f;
			else if (pos_y.is_length())
				y = origin_box.top + pos_y.number();
		}

		StyleGetValue repeat_y = get_layer_repeat_y(index);
		if (repeat_y.is_keyword("repeat") || repeat_y.is_keyword("space"))
		{
			if (y > clip_box.top)
				y = clip_box.top - std::fmod(image_size.height - (y - clip_box.top), image_size.height);
		}

		return y;
	}

	Sizef StyleBackgroundRenderer::get_image_size(int index, const std::shared_ptr<Image> &image, Rectf origin_box)
	{
		Sizef size;
		StyleGetValue size_x = get_layer_size_x(index);
		StyleGetValue size_y = get_layer_size_y(index);
		
		if (size_x.is_keyword("contain"))
		{
			if (origin_box.height()*image->width() / image->height() <= origin_box.width())
				size = Sizef(origin_box.height()*image->width() / image->height(), origin_box.height());
			else
				size = Sizef(origin_box.width(), origin_box.width()*image->height() / image->width());
		}
		else if (size_x.is_keyword("cover"))
		{
			if (origin_box.height()*image->width() / image->height() >= origin_box.width())
				size = Sizef(origin_box.height()*image->width() / image->height(), origin_box.height());
			else
				size = Sizef(origin_box.width(), origin_box.width()*image->height() / image->width());
		}
		else
		{
			float width = image->width();
			if (size_x.is_length())
			{
				width = size_x.number();
			}
			else if (size_x.is_percentage())
			{
				width = size_x.number() * width / 100.0f;
			}

			float height = image->height();
			if (size_y.is_length())
			{
				height = size_y.number();
			}
			else if (size_y.is_percentage())
			{
				height = size_y.number() * height / 100.0f;
			}

			size = Sizef(width, height);
		}

		StyleGetValue repeat_x = get_layer_repeat_x(index);
		StyleGetValue repeat_y = get_layer_repeat_y(index);

		if (repeat_x.is_keyword("round"))
		{
			if (size.width != 0)
				size.width = origin_box.width() / (int)(origin_box.width() / (float)size.width + 0.5f);
		}

		if (repeat_y.is_keyword("round"))
		{
			if (size.height != 0)
				size.height = origin_box.height() / (int)(origin_box.height() / (float)size.height + 0.5f);
		}

		if (repeat_x.is_keyword("round") && size_y.is_keyword("auto"))
		{
			size.height = size.width*image->height() / image->width();
		}
		else if (repeat_y.is_keyword("round") && size_x.is_keyword("auto"))
		{
			size.width = size.height*image->width() / image->height();
		}

		return size;
	}

	Rectf StyleBackgroundRenderer::get_clip_box(int index)
	{
		//if (is_root)
		//	return initial_containing_box;

		StyleGetValue clip = get_layer_clip(index);
		if (clip.is_keyword("border-box"))
			return geometry.border_box();
		else if (clip.is_keyword("padding-box"))
			return geometry.padding_box();
		else if (clip.is_keyword("content-box"))
			return geometry.content_box();
		else
			return Rectf();
	}

	Rectf StyleBackgroundRenderer::get_origin_box(int index)
	{
		StyleGetValue attachment = get_layer_attachment(index);
		/*if (attachment.is_keyword("fixed"))
		{
			return initial_containing_box;
		}
		else*/ if (attachment.is_keyword("local") || attachment.is_keyword("scroll"))
		{
			StyleGetValue origin = get_layer_origin(index);
			if (origin.is_keyword("border-box"))
				return geometry.border_box();
			else if (origin.is_keyword("padding-box"))
				return geometry.padding_box();
			else if (origin.is_keyword("content-box"))
				return geometry.content_box();
			else
				return Rectf();
		}
		else
		{
			return Rectf();
		}
	}

	StyleGetValue StyleBackgroundRenderer::get_layer_clip(int index)
	{
		int count = style.array_size("background-clip");
		return style.computed_value("background-clip[" + Text::to_string(index % count) + "]");
	}

	StyleGetValue StyleBackgroundRenderer::get_layer_origin(int index)
	{
		int count = style.array_size("background-origin");
		return style.computed_value("background-origin[" + Text::to_string(index % count) + "]");
	}

	StyleGetValue StyleBackgroundRenderer::get_layer_size_x(int index)
	{
		int count = style.array_size("background-size-x");
		return style.computed_value("background-size-x[" + Text::to_string(index % count) + "]");
	}

	StyleGetValue StyleBackgroundRenderer::get_layer_size_y(int index)
	{
		int count = style.array_size("background-size-y");
		return style.computed_value("background-size-y[" + Text::to_string(index % count) + "]");
	}

	StyleGetValue StyleBackgroundRenderer::get_layer_position_x(int index)
	{
		int count = style.array_size("background-position-x");
		return style.computed_value("background-position-x[" + Text::to_string(index % count) + "]");
	}

	StyleGetValue StyleBackgroundRenderer::get_layer_position_y(int index)
	{
		int count = style.array_size("background-position-y");
		return style.computed_value("background-position-y[" + Text::to_string(index % count) + "]");
	}

	StyleGetValue StyleBackgroundRenderer::get_layer_attachment(int index)
	{
		int count = style.array_size("background-attachment");
		return style.computed_value("background-attachment[" + Text::to_string(index % count) + "]");
	}

	StyleGetValue StyleBackgroundRenderer::get_layer_repeat_x(int index)
	{
		int count = style.array_size("background-repeat-x");
		return style.computed_value("background-repeat-x[" + Text::to_string(index % count) + "]");
	}

	StyleGetValue StyleBackgroundRenderer::get_layer_repeat_y(int index)
	{
		int count = style.array_size("background-repeat-y");
		return style.computed_value("background-repeat-y[" + Text::to_string(index % count) + "]");
	}

	std::array<Pointf, 2 * 4> StyleBackgroundRenderer::get_border_points()
	{
		float top_left_x = get_horizontal_radius(style.computed_value("border-top-left-radius-x"));
		float top_left_y = get_vertical_radius(style.computed_value("border-top-left-radius-y"));
		float top_right_x = get_horizontal_radius(style.computed_value("border-top-right-radius-x"));
		float top_right_y = get_vertical_radius(style.computed_value("border-top-right-radius-y"));
		float bottom_left_x = get_horizontal_radius(style.computed_value("border-bottom-left-radius-x"));
		float bottom_left_y = get_vertical_radius(style.computed_value("border-bottom-left-radius-y"));
		float bottom_right_x = get_horizontal_radius(style.computed_value("border-bottom-right-radius-x"));
		float bottom_right_y = get_vertical_radius(style.computed_value("border-bottom-right-radius-y"));

		Rectf border_box = geometry.border_box();

		std::array<Pointf, 2 * 4> border_points;
		border_points[0] = Pointf(border_box.left + top_left_x, border_box.top);
		border_points[1] = Pointf(border_box.right - top_right_x, border_box.top);
		border_points[2] = Pointf(border_box.right, border_box.top + top_right_y);
		border_points[3] = Pointf(border_box.right, border_box.bottom - bottom_right_y);
		border_points[4] = Pointf(border_box.right - bottom_right_x, border_box.bottom);
		border_points[5] = Pointf(border_box.left + bottom_left_x, border_box.bottom);
		border_points[6] = Pointf(border_box.left, border_box.bottom - bottom_left_y);
		border_points[7] = Pointf(border_box.left, border_box.top + top_left_y);
		return border_points;
	}

	std::array<Pointf, 2 * 4> StyleBackgroundRenderer::get_padding_points(const std::array<Pointf, 2 * 4> &border_points)
	{
		Rectf padding_box = geometry.padding_box();

		std::array<Pointf, 2 * 4> padding_points;
		padding_points[0] = Pointf(padding_box.left, padding_box.top);
		padding_points[1] = Pointf(padding_box.right, padding_box.top);
		padding_points[2] = Pointf(padding_box.right, padding_box.top);
		padding_points[3] = Pointf(padding_box.right, padding_box.bottom);
		padding_points[4] = Pointf(padding_box.right, padding_box.bottom);
		padding_points[5] = Pointf(padding_box.left, padding_box.bottom);
		padding_points[6] = Pointf(padding_box.left, padding_box.bottom);
		padding_points[7] = Pointf(padding_box.left, padding_box.top);

		padding_points[0].x = uicore::max(padding_points[0].x, border_points[0].x);
		padding_points[0].y = uicore::max(padding_points[0].y, border_points[0].y);
		padding_points[1].x = uicore::min(padding_points[1].x, border_points[1].x);
		padding_points[1].y = uicore::max(padding_points[1].y, border_points[1].y);
		padding_points[2].x = uicore::min(padding_points[2].x, border_points[2].x);
		padding_points[2].y = uicore::max(padding_points[2].y, border_points[2].y);
		padding_points[3].x = uicore::min(padding_points[3].x, border_points[3].x);
		padding_points[3].y = uicore::min(padding_points[3].y, border_points[3].y);
		padding_points[4].x = uicore::min(padding_points[4].x, border_points[4].x);
		padding_points[4].y = uicore::min(padding_points[4].y, border_points[4].y);
		padding_points[5].x = uicore::max(padding_points[5].x, border_points[5].x);
		padding_points[5].y = uicore::min(padding_points[5].y, border_points[5].y);
		padding_points[6].x = uicore::max(padding_points[6].x, border_points[6].x);
		padding_points[6].y = uicore::min(padding_points[6].y, border_points[6].y);
		padding_points[7].x = uicore::max(padding_points[7].x, border_points[7].x);
		padding_points[7].y = uicore::max(padding_points[7].y, border_points[7].y);

		return padding_points;
	}

	std::shared_ptr<Path> StyleBackgroundRenderer::get_border_area_path(const std::array<Pointf, 2 * 4> &border_points)
	{
		float kappa = 0.551784f;

		// Border area path (to be used for filling)

		auto border_area_path = Path::create();

		border_area_path->move_to(border_points[0]);
		border_area_path->line_to(border_points[1]);
		if (border_points[1] != border_points[2])
		{
			border_area_path->bezier_to(
				Pointf(mix(border_points[1].x, border_points[2].x, kappa), border_points[1].y),
				Pointf(border_points[2].x, mix(border_points[1].y, border_points[2].y, kappa)),
				border_points[2]);
		}

		border_area_path->line_to(border_points[3]);
		if (border_points[3] != border_points[4])
		{
			border_area_path->bezier_to(
				Pointf(border_points[3].x, mix(border_points[3].y, border_points[4].y, kappa)),
				Pointf(mix(border_points[3].x, border_points[4].x, kappa), border_points[4].y),
				border_points[4]);
		}

		border_area_path->line_to(border_points[5]);
		if (border_points[5] != border_points[6])
		{
			border_area_path->bezier_to(
				Pointf(mix(border_points[5].x, border_points[6].x, kappa), border_points[5].y),
				Pointf(border_points[6].x, mix(border_points[5].y, border_points[6].y, kappa)),
				border_points[6]);
		}

		border_area_path->line_to(border_points[7]);
		if (border_points[7] != border_points[0])
		{
			border_area_path->bezier_to(
				Pointf(border_points[7].x, mix(border_points[7].y, border_points[0].y, kappa)),
				Pointf(mix(border_points[7].x, border_points[0].x, kappa), border_points[0].y),
				border_points[0]);
		}

		border_area_path->close();

		return border_area_path;
	}

	std::shared_ptr<Path> StyleBackgroundRenderer::get_border_stroke_path(const std::array<Pointf, 2 * 4> &border_points, const std::array<Pointf, 2 * 4> &padding_points, int start, int end)
	{
		// Border path (the path defining the actual border)

		auto border_path = Path::create();
		const float kappa = 0.551784f;

		bool entire_border = (start + 4 == end);

		for (int i = start; i <= end; i++)
		{
			int from_corner = (i * 2 + 7) % 8;
			int to_corner = (from_corner + 1) % 8;

			if (border_points[from_corner] != border_points[to_corner])
			{
				Pointf cp0 = border_points[from_corner];
				Pointf cp1 = Pointf(mix(border_points[from_corner].x, border_points[to_corner].x, kappa), border_points[from_corner].y);
				Pointf cp2 = Pointf(border_points[to_corner].x, mix(border_points[from_corner].y, border_points[to_corner].y, kappa));
				Pointf cp3 = border_points[to_corner];

				if (i == start)
				{
					SplitBezier split(cp0, cp1, cp2, cp3, 0.5f);
					border_path->move_to(split.first[2]);
					border_path->bezier_to(split.second[0], split.second[1], split.second[2]);
				}
				else if (i == end && !entire_border)
				{
					SplitBezier split(cp0, cp1, cp2, cp3, 0.5f);
					border_path->line_to(cp0);
					border_path->bezier_to(split.first[0], split.first[1], split.first[2]);
				}
				else
				{
					border_path->line_to(cp0);
					border_path->bezier_to(cp1, cp2, cp3);
				}
			}
			else
			{
				if (i == start)
					border_path->move_to(border_points[to_corner]);
				else
					border_path->line_to(border_points[to_corner]);
			}
		}

		if (entire_border)
		{
			border_path->close();
		}

		for (int i = end; i >= start; i--)
		{
			int to_corner = (i * 2 + 7) % 8;
			int from_corner = (to_corner + 1) % 8;

			if (padding_points[from_corner] != padding_points[to_corner])
			{
				Pointf cp0 = padding_points[from_corner];
				Pointf cp1 = Pointf(mix(padding_points[from_corner].x, padding_points[to_corner].x, kappa), padding_points[from_corner].y);
				Pointf cp2 = Pointf(padding_points[to_corner].x, mix(padding_points[from_corner].y, padding_points[to_corner].y, kappa));
				Pointf cp3 = padding_points[to_corner];

				if (i == end)
				{
					if (!entire_border)
					{
						SplitBezier split(cp0, cp1, cp2, cp3, 0.5f);
						border_path->line_to(split.first[2]);
						border_path->bezier_to(split.second[0], split.second[1], split.second[2]);
					}
					else
					{
						border_path->move_to(cp0);
						border_path->bezier_to(cp1, cp2, cp3);
					}
				}
				else if (i == start && !entire_border)
				{
					SplitBezier split(cp0, cp1, cp2, cp3, 0.5f);
					border_path->line_to(cp0);
					border_path->bezier_to(split.first[0], split.first[1], split.first[2]);
				}
				else
				{
					border_path->line_to(cp0);
					border_path->bezier_to(cp1, cp2, cp3);
				}
			}
			else
			{
				if (i == end && entire_border)
					border_path->move_to(padding_points[to_corner]);
				else
					border_path->line_to(padding_points[to_corner]);
			}
		}

		border_path->close();

		return border_path;
	}

	std::shared_ptr<Path> StyleBackgroundRenderer::get_border_stroke_path(const std::array<Pointf, 2 * 4> &border_points, const std::array<Pointf, 2 * 4> &padding_points)
	{
		// Border path (the path defining the actual border)

		float kappa = 0.551784f;

		auto border_path = Path::create();

		border_path->move_to(border_points[0]);
		border_path->line_to(border_points[1]);
		if (border_points[1] != border_points[2])
		{
			border_path->bezier_to(
				Pointf(mix(border_points[1].x, border_points[2].x, kappa), border_points[1].y),
				Pointf(border_points[2].x, mix(border_points[1].y, border_points[2].y, kappa)),
				border_points[2]);
		}

		border_path->line_to(border_points[3]);
		if (border_points[3] != border_points[4])
		{
			border_path->bezier_to(
				Pointf(border_points[3].x, mix(border_points[3].y, border_points[4].y, kappa)),
				Pointf(mix(border_points[3].x, border_points[4].x, kappa), border_points[4].y),
				border_points[4]);
		}

		border_path->line_to(border_points[5]);
		if (border_points[5] != border_points[6])
		{
			border_path->bezier_to(
				Pointf(mix(border_points[5].x, border_points[6].x, kappa), border_points[5].y),
				Pointf(border_points[6].x, mix(border_points[5].y, border_points[6].y, kappa)),
				border_points[6]);
		}

		border_path->line_to(border_points[7]);
		if (border_points[7] != border_points[0])
		{
			border_path->bezier_to(
				Pointf(border_points[7].x, mix(border_points[7].y, border_points[0].y, kappa)),
				Pointf(mix(border_points[7].x, border_points[0].x, kappa), border_points[0].y),
				border_points[0]);
		}

		border_path->close();

		border_path->move_to(padding_points[0]);
		border_path->line_to(padding_points[1]);
		if (padding_points[1] != padding_points[2])
		{
			border_path->bezier_to(
				Pointf(mix(padding_points[1].x, padding_points[2].x, kappa), padding_points[1].y),
				Pointf(padding_points[2].x, mix(padding_points[1].y, padding_points[2].y, kappa)),
				padding_points[2]);
		}

		border_path->line_to(padding_points[3]);
		if (padding_points[3] != padding_points[4])
		{
			border_path->bezier_to(
				Pointf(padding_points[3].x, mix(padding_points[3].y, padding_points[4].y, kappa)),
				Pointf(mix(padding_points[3].x, padding_points[4].x, kappa), padding_points[4].y),
				padding_points[4]);
		}

		border_path->line_to(padding_points[5]);
		if (padding_points[5] != padding_points[6])
		{
			border_path->bezier_to(
				Pointf(mix(padding_points[5].x, padding_points[6].x, kappa), padding_points[5].y),
				Pointf(padding_points[6].x, mix(padding_points[5].y, padding_points[6].y, kappa)),
				padding_points[6]);
		}

		border_path->line_to(padding_points[7]);
		if (padding_points[7] != padding_points[0])
		{
			border_path->bezier_to(
				Pointf(padding_points[7].x, mix(padding_points[7].y, padding_points[0].y, kappa)),
				Pointf(mix(padding_points[7].x, padding_points[0].x, kappa), padding_points[0].y),
				padding_points[0]);
		}

		border_path->close();

		return border_path;
	}

	StyleBackgroundRenderer::SplitBezier::SplitBezier(const Pointf &cp0, const Pointf &cp1, const Pointf &cp2, const Pointf &cp3, float t)
	{
		const int num_cp = 4;

		float cp_x[4] = { cp0.x, cp1.x, cp2.x, cp3.x };
		float cp_y[4] = { cp0.y, cp1.y, cp2.y, cp3.y };

		// Perform deCasteljau iterations:
		// (linear interpolate between the control points)
		float a = 1.0f - t;
		float b = t;
		int pos = 0;
		for (int j = num_cp - 1; j > 0; j--)
		{
			for (int i = 0; i < j; i++)
			{
				cp_x[i] = a * cp_x[i] + b * cp_x[i + 1];
				cp_y[i] = a * cp_y[i] + b * cp_y[i + 1];
			}
			first[pos] = { cp_x[0], cp_y[0] };
			second[2 - pos] = { cp_x[j], cp_y[j] };
			pos++;
		}

		// To do: compare this to http://pomax.github.io/BezierInfo-2/#splitting and figure out what is going wrong here..
	}

	void StyleBackgroundRenderer::render_box_shadow()
	{
		int num_shadows = style.array_size("box-shadow-style");
		if (num_shadows == 0)
			return;

		Rectf border_box = geometry.border_box();
		auto border_points = get_border_points();

		for (int index = num_shadows - 1; index >= 0; index--)
		{
			auto layer_style = style.computed_value("box-shadow-style[" + Text::to_string(index) + "]");

			// To do: support inset

			if (!layer_style.is_keyword("outset"))
				continue;

			auto layer_color = style.computed_value("box-shadow-color[" + Text::to_string(index) + "]");
			if (layer_color.color().w <= 0.0f)
				continue;

			auto layer_offset_x = style.computed_value("box-shadow-horizontal-offset[" + Text::to_string(index) + "]");
			auto layer_offset_y = style.computed_value("box-shadow-vertical-offset[" + Text::to_string(index) + "]");
			auto layer_blur_radius = style.computed_value("box-shadow-blur-radius[" + Text::to_string(index) + "]");
			//auto layer_spread_distance = style.computed_value("box-shadow-spread-distance[" + Text::to_string(index) + "]");

			// To do: support shadow_spread_distance

			float shadow_blur_radius = layer_blur_radius.number();
			Pointf shadow_offset(layer_offset_x.number(), layer_offset_y.number());
			Colorf shadow_color = layer_color.color();
			Colorf transparent = shadow_color;
			transparent.w = 0.0f;

			float kappa = 0.551784f;

			float top_left_x = get_horizontal_radius(style.computed_value("border-top-left-radius-x"));
			float top_left_y = get_vertical_radius(style.computed_value("border-top-left-radius-y"));
			float top_right_x = get_horizontal_radius(style.computed_value("border-top-right-radius-x"));
			float top_right_y = get_vertical_radius(style.computed_value("border-top-right-radius-y"));
			float bottom_left_x = get_horizontal_radius(style.computed_value("border-bottom-left-radius-x"));
			float bottom_left_y = get_vertical_radius(style.computed_value("border-bottom-left-radius-y"));
			float bottom_right_x = get_horizontal_radius(style.computed_value("border-bottom-right-radius-x"));
			float bottom_right_y = get_vertical_radius(style.computed_value("border-bottom-right-radius-y"));

			if (shadow_blur_radius != 0.0f)
			{
				auto top_left = Path::create();
				top_left->move_to(Pointf(border_box.left - shadow_blur_radius, border_box.top - shadow_blur_radius));
				top_left->line_to(Pointf(border_points[0].x, border_points[0].y - shadow_blur_radius));
				top_left->line_to(border_points[0]);
				if (border_points[7] != border_points[0])
				{
					top_left->bezier_to(
						Pointf(border_points[0].x, mix(border_points[0].y, border_points[7].y, kappa)),
						Pointf(mix(border_points[0].x, border_points[7].x, kappa), border_points[7].y),
						border_points[7]);
				}
				top_left->line_to(Pointf(border_points[7].x - shadow_blur_radius, border_points[7].y));
				top_left->close();

				auto top_right = Path::create();
				top_right->move_to(Pointf(border_points[1].x, border_points[1].y - shadow_blur_radius));
				top_right->line_to(Pointf(border_box.right + shadow_blur_radius, border_box.top - shadow_blur_radius));
				top_right->line_to(Pointf(border_points[2].x + shadow_blur_radius, border_points[2].y));
				top_right->line_to(border_points[2]);
				if (border_points[1] != border_points[2])
				{
					top_right->bezier_to(
						Pointf(border_points[2].x, mix(border_points[2].y, border_points[1].y, kappa)),
						Pointf(mix(border_points[2].x, border_points[1].x, kappa), border_points[1].y),
						border_points[1]);
				}
				top_right->close();

				auto bottom_right = Path::create();
				bottom_right->move_to(Pointf(border_box.right + shadow_blur_radius, border_box.bottom + shadow_blur_radius));
				bottom_right->line_to(Pointf(border_points[4].x, border_points[4].y + shadow_blur_radius));
				bottom_right->line_to(border_points[4]);
				if (border_points[4] != border_points[3])
				{
					bottom_right->bezier_to(
						Pointf(border_points[4].x, mix(border_points[4].y, border_points[3].y, kappa)),
						Pointf(mix(border_points[4].x, border_points[3].x, kappa), border_points[3].y),
						border_points[3]);
				}
				bottom_right->line_to(Pointf(border_points[3].x + shadow_blur_radius, border_points[3].y));
				bottom_right->close();

				auto bottom_left = Path::create();
				bottom_left->move_to(Pointf(border_box.left - shadow_blur_radius, border_box.bottom + shadow_blur_radius));
				bottom_left->line_to(Pointf(border_points[6].x - shadow_blur_radius, border_points[6].y));
				bottom_left->line_to(border_points[6]);
				if (border_points[6] != border_points[5])
				{
					bottom_left->bezier_to(
						Pointf(border_points[6].x, mix(border_points[6].y, border_points[5].y, kappa)),
						Pointf(mix(border_points[6].x, border_points[5].x, kappa), border_points[5].y),
						border_points[5]);
				}
				bottom_left->line_to(Pointf(border_points[5].x, border_points[5].y + shadow_blur_radius));
				bottom_left->close();

				Brush brush_top_left;
				brush_top_left.type = BrushType::radial;
				brush_top_left.radius_x = shadow_blur_radius + top_left_x;
				brush_top_left.radius_y = shadow_blur_radius + top_left_y;
				brush_top_left.center_point = Pointf(border_box.left + top_left_x, border_box.top + top_left_y);
				brush_top_left.stops = shadow_blur_stops(shadow_color, shadow_blur_radius, top_left_x / brush_top_left.radius_x);

				Brush brush_top_right;
				brush_top_right.type = BrushType::radial;
				brush_top_right.radius_x = shadow_blur_radius + top_right_x;
				brush_top_right.radius_y = shadow_blur_radius + top_right_y;
				brush_top_right.center_point = Pointf(border_box.right - top_right_x, border_box.top + top_right_y);
				brush_top_right.stops = shadow_blur_stops(shadow_color, shadow_blur_radius, top_right_x / brush_top_right.radius_x);

				Brush brush_bottom_right;
				brush_bottom_right.type = BrushType::radial;
				brush_bottom_right.radius_x = shadow_blur_radius + bottom_right_x;
				brush_bottom_right.radius_y = shadow_blur_radius + bottom_right_y;
				brush_bottom_right.center_point = Pointf(border_box.right - bottom_right_x, border_box.bottom - bottom_right_y);
				brush_bottom_right.stops = shadow_blur_stops(shadow_color, shadow_blur_radius, bottom_right_x / brush_bottom_right.radius_x);

				Brush brush_bottom_left;
				brush_bottom_left.type = BrushType::radial;
				brush_bottom_left.radius_x = shadow_blur_radius + bottom_left_x;
				brush_bottom_left.radius_y = shadow_blur_radius + bottom_left_y;
				brush_bottom_left.center_point = Pointf(border_box.left + bottom_left_x, border_box.bottom - bottom_left_y);
				brush_bottom_left.stops = shadow_blur_stops(shadow_color, shadow_blur_radius, bottom_left_x / brush_bottom_left.radius_x);

				top_left->fill(canvas, brush_top_left);
				top_right->fill(canvas, brush_top_right);
				bottom_right->fill(canvas, brush_bottom_right);
				bottom_left->fill(canvas, brush_bottom_left);

				Brush brush_linear;
				brush_linear.type = BrushType::linear;
				brush_linear.stops = shadow_blur_stops(shadow_color, shadow_blur_radius, 0.0f);

				auto top = Path::create();
				top->move_to(Pointf(border_points[0].x, border_points[0].y - shadow_blur_radius));
				top->line_to(Pointf(border_points[1].x, border_points[1].y - shadow_blur_radius));
				top->line_to(border_points[1]);
				top->line_to(border_points[0]);
				top->close();

				auto right = Path::create();
				right->move_to(Pointf(border_points[2].x + shadow_blur_radius, border_points[2].y));
				right->line_to(Pointf(border_points[3].x + shadow_blur_radius, border_points[3].y));
				right->line_to(border_points[3]);
				right->line_to(border_points[2]);
				right->close();

				auto bottom = Path::create();
				bottom->move_to(Pointf(border_points[4].x, border_points[4].y + shadow_blur_radius));
				bottom->line_to(Pointf(border_points[5].x, border_points[5].y + shadow_blur_radius));
				bottom->line_to(border_points[5]);
				bottom->line_to(border_points[4]);
				bottom->close();

				auto left = Path::create();
				left->move_to(Pointf(border_points[6].x - shadow_blur_radius, border_points[6].y));
				left->line_to(Pointf(border_points[7].x - shadow_blur_radius, border_points[7].y));
				left->line_to(border_points[7]);
				left->line_to(border_points[6]);
				left->close();

				brush_linear.start_point = border_points[0];
				brush_linear.end_point = Pointf(border_points[0].x, border_points[0].y - shadow_blur_radius);
				top->fill(canvas, brush_linear);

				brush_linear.start_point = border_points[2];
				brush_linear.end_point = Pointf(border_points[2].x + shadow_blur_radius, border_points[2].y);
				right->fill(canvas, brush_linear);

				brush_linear.start_point = border_points[4];
				brush_linear.end_point = Pointf(border_points[4].x, border_points[4].y + shadow_blur_radius);
				bottom->fill(canvas, brush_linear);

				brush_linear.start_point = border_points[6];
				brush_linear.end_point = Pointf(border_points[6].x - shadow_blur_radius, border_points[6].y);
				left->fill(canvas, brush_linear);
			}
		}
	}

	std::vector<BrushGradientStop> StyleBackgroundRenderer::shadow_blur_stops(const Colorf &shadow_color, float shadow_blur_radius, float start_t)
	{
		std::vector<BrushGradientStop> stops;
		stops.push_back(BrushGradientStop(shadow_color, start_t));
		for (float step = 1.0f; step < shadow_blur_radius - 1.0f; step += 1.0f)
		{
			float t = step / shadow_blur_radius;
			float a = (1.0f - t) * (1.0f - t);
			float final_t = start_t + t * (1.0f - start_t);
			stops.push_back(BrushGradientStop(Colorf(shadow_color.x, shadow_color.y, shadow_color.z, shadow_color.w * a), final_t));
		}
		stops.push_back(BrushGradientStop(Colorf(shadow_color.x, shadow_color.y, shadow_color.z, 0.0f), 1.0f));
		return stops;
	}

	float StyleBackgroundRenderer::mix(float a, float b, float t)
	{
		return a * (1.0f - t) + b * t;
	}

	float StyleBackgroundRenderer::get_horizontal_radius(const StyleGetValue &border_radius) const
	{
		if (border_radius.is_length())
			return border_radius.number();
		else if (border_radius.is_percentage())
			return border_radius.number() * geometry.border_box().width() / 100.0f;
		else
			return 0.0f;
	}

	float StyleBackgroundRenderer::get_vertical_radius(const StyleGetValue &border_radius) const
	{
		if (border_radius.is_length())
			return border_radius.number();
		else if (border_radius.is_percentage())
			return border_radius.number() * geometry.border_box().height() / 100.0f;
		else
			return 0.0f;
	}

	Colorf StyleBackgroundRenderer::get_light_color(const StyleGetValue &border_color) const
	{
		Colorf light = border_color.color();
		light.x = min(1.0f, light.x * 1.2f);
		light.y = min(1.0f, light.y * 1.2f);
		light.z = min(1.0f, light.z * 1.2f);
		return light;
	}

	Colorf StyleBackgroundRenderer::get_dark_color(const StyleGetValue &border_color) const
	{
		Colorf dark = border_color.color();
		dark.x *= 0.8f;
		dark.y *= 0.8f;
		dark.z *= 0.8f;
		return dark;
	}
}
