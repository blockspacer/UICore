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
#include "UICore/UI/TopLevel/texture_window.h"
#include "UICore/UI/Events/key_event.h"
#include "UICore/UI/Events/pointer_event.h"
#include "UICore/UI/Events/close_event.h"
#include "UICore/UI/Events/activation_change_event.h"
#include "UICore/Display/Render/blend_state_description.h"
#include "UICore/Display/Window/input_event.h"
#include "UICore/Display/2D/canvas.h"
#include "UICore/Display/2D/path.h"
#include "UICore/Display/2D/brush.h"
#include "texture_window_impl.h"

namespace uicore
{

	TextureWindow_Impl::TextureWindow_Impl(TextureWindow *view, const std::shared_ptr<Canvas> &canvas) : window_view(view), canvas(canvas)
	{
		canvas_rect = canvas->size();
	}

	void TextureWindow_Impl::set_window(const std::shared_ptr<DisplayWindow> &window, bool enable_automatic_events, const Mat4f &new_transform_mouse_matrix)
	{
		slots = SlotContainer();
		display_window = window;
		transform_mouse_matrix = new_transform_mouse_matrix;
		if (display_window && enable_automatic_events)
		{
			slots.connect(display_window->sig_lost_focus(), uicore::bind_member(this, &TextureWindow_Impl::on_lost_focus));
			slots.connect(display_window->sig_got_focus(), uicore::bind_member(this, &TextureWindow_Impl::on_got_focus));
			slots.connect(display_window->sig_window_close(), uicore::bind_member(this, &TextureWindow_Impl::on_window_close));
			slots.connect(display_window->keyboard()->sig_key_down(), uicore::bind_member(this, &TextureWindow_Impl::transform_on_key_down));
			slots.connect(display_window->keyboard()->sig_key_up(), uicore::bind_member(this, &TextureWindow_Impl::transform_on_key_up));
			slots.connect(display_window->mouse()->sig_key_down(), uicore::bind_member(this, &TextureWindow_Impl::transform_on_mouse_down));
			slots.connect(display_window->mouse()->sig_key_dblclk(), uicore::bind_member(this, &TextureWindow_Impl::transform_on_mouse_dblclk));
			slots.connect(display_window->mouse()->sig_key_up(), uicore::bind_member(this, &TextureWindow_Impl::transform_on_mouse_up));
			slots.connect(display_window->mouse()->sig_pointer_move(), uicore::bind_member(this, &TextureWindow_Impl::transform_on_mouse_move));
		}
	}

	void TextureWindow_Impl::update()
	{
		if (needs_render || always_render)
		{
			canvas->set_clip(canvas_rect);

			if (clear_background_enable)
			{
				canvas->clear(background_color);
			}

			needs_render = false;
			window_view->render(canvas, canvas_rect);
			canvas->reset_clip();
		}
	}
	
	void TextureWindow_Impl::on_lost_focus()
	{
		release_capture();
		window_view->dispatch_activation_change(ActivationChangeType::deactivated);
	}

	void TextureWindow_Impl::on_got_focus()
	{
		release_capture();
		window_view->dispatch_activation_change(ActivationChangeType::activated);
	}

	void TextureWindow_Impl::on_window_close()
	{
		CloseEvent e;
		window_view->root_view()->dispatch_event(&e);
	}

	void TextureWindow_Impl::window_key_event(KeyEvent &e)
	{
		View *view = window_view->focus_view();
		if (view)
		{
			view->dispatch_event(&e);
		}
	}

	void TextureWindow_Impl::dispatch_hot_event(std::shared_ptr<View> &view, PointerEvent &e)
	{
		if (view != hot_view)
		{
			auto common_parent = View::common_parent(view.get(), hot_view.get());

			if (hot_view)
			{
				PointerEvent e_exit(PointerEventType::leave, PointerButton::none, e.pos(window_view->root_view()), e.alt_down(), e.shift_down(), e.ctrl_down(), e.cmd_down());
				hot_view->dispatch_event(&e_exit, common_parent);
			}

			hot_view = view;

			if (hot_view)
			{
				PointerEvent e_enter(PointerEventType::enter, PointerButton::none, e.pos(window_view->root_view()), e.alt_down(), e.shift_down(), e.ctrl_down(), e.cmd_down());
				hot_view->dispatch_event(&e_enter, common_parent);
			}
		}

		if (hot_view)
		{
			if (display_window)
				hot_view->update_cursor(display_window);
		}

	}

	void TextureWindow_Impl::release_capture()
	{
		if (captured_view)
		{
			if (display_window)
				display_window->capture_mouse(false);
			captured_view.reset();
			capture_down_counter = 0;
		}
	}

	std::shared_ptr<View> TextureWindow_Impl::get_capture_view(PointerEvent &e, std::shared_ptr<View> &view_above_cursor)
	{
		if (e.type() == PointerEventType::press || e.type() == PointerEventType::double_click)
		{
			capture_down_counter++;
			if (capture_down_counter == 1)
			{
				captured_view = view_above_cursor;
				if (captured_view)
				{
					if (display_window)
						display_window->capture_mouse(true);
				}
			}
		}

		std::shared_ptr<View> view = captured_view;

		if (e.type() == PointerEventType::release)
		{
			if (capture_down_counter > 0)
			{
				capture_down_counter--;
				if (capture_down_counter == 0)
				{
					if (captured_view)
						release_capture();
				}
			}
		}
		return view;
	}

	void TextureWindow_Impl::window_pointer_event(PointerEvent &e_window)
	{
		PointerEvent e = e_window;
		Pointf pointer_pos = e.pos(window_view->root_view());
		pointer_pos.x -= window_view->root_view()->geometry().content_x;
		pointer_pos.y -= window_view->root_view()->geometry().content_y;
		e.set_pos(window_view->root_view().get(), pointer_pos);

		std::shared_ptr<View> view_above_cursor = window_view->root_view()->find_view_at(e.pos(window_view->root_view()));
		auto view = get_capture_view(e, view_above_cursor);
		if (!view)
			view = view_above_cursor;

		dispatch_hot_event(view, e);

		if (e.type() == PointerEventType::enter || e.type() == PointerEventType::leave)		// Ignore window enter/leave events
			return;

		if (view)
			view->dispatch_event(&e);
		else
			window_view->root_view()->dispatch_event(&e);
	}

	void TextureWindow_Impl::on_key_down(const uicore::InputEvent &e)
	{
		KeyEventType type = KeyEventType::press;
		Key key = decode_ic(e.id);
		bool key_repeat = e.key_repeat;
		const std::string text = e.str;
		const Pointf pointer_pos = e.mouse_pos;
		bool alt_down = e.alt;
		bool shift_down = e.shift;
		bool ctrl_down = e.ctrl;
		bool cmd_down = false;
		KeyEvent key_event(type, key, key_repeat, text, pointer_pos, alt_down, shift_down, ctrl_down, cmd_down);
		window_key_event(key_event);
	}

	void TextureWindow_Impl::on_key_up(const uicore::InputEvent &e)
	{
		KeyEventType type = KeyEventType::release;
		Key key = decode_ic(e.id);
		bool key_repeat = e.key_repeat;
		const std::string text = e.str;
		const Pointf pointer_pos = e.mouse_pos;
		bool alt_down = e.alt;
		bool shift_down = e.shift;
		bool ctrl_down = e.ctrl;
		bool cmd_down = false;
		KeyEvent key_event(type, key, key_repeat, text, pointer_pos, alt_down, shift_down, ctrl_down, cmd_down);
		window_key_event(key_event);
	}

	void TextureWindow_Impl::on_mouse_down(const uicore::InputEvent &e)
	{
		PointerEventType type = PointerEventType::press;
		PointerButton button = decode_id(e.id);
		const Pointf pos = e.mouse_pos;
		bool alt_down = e.alt;
		bool shift_down = e.shift;
		bool ctrl_down = e.ctrl;
		bool cmd_down = false;
		PointerEvent pointer_event(type, button, pos, alt_down, shift_down, ctrl_down, cmd_down);
		window_pointer_event(pointer_event);
	}

	void TextureWindow_Impl::on_mouse_dblclk(const uicore::InputEvent &e)
	{
		PointerEventType type = PointerEventType::double_click;
		PointerButton button = decode_id(e.id);
		const Pointf pos = e.mouse_pos;
		bool alt_down = e.alt;
		bool shift_down = e.shift;
		bool ctrl_down = e.ctrl;
		bool cmd_down = false;
		PointerEvent pointer_event(type, button, pos, alt_down, shift_down, ctrl_down, cmd_down);
		window_pointer_event(pointer_event);
	}

	void TextureWindow_Impl::on_mouse_up(const uicore::InputEvent &e)
	{
		PointerEventType type = PointerEventType::release;
		PointerButton button = decode_id(e.id);
		const Pointf pos = e.mouse_pos;
		bool alt_down = e.alt;
		bool shift_down = e.shift;
		bool ctrl_down = e.ctrl;
		bool cmd_down = false;
		PointerEvent pointer_event(type, button, pos, alt_down, shift_down, ctrl_down, cmd_down);
		window_pointer_event(pointer_event);
	}

	void TextureWindow_Impl::on_mouse_move(const uicore::InputEvent &uicore_event)
	{
		PointerEvent e(PointerEventType::move, PointerButton::none, uicore_event.mouse_pos, uicore_event.alt, uicore_event.shift, uicore_event.ctrl, false/*uicore_event.cmd*/);
		window_pointer_event(e);
	}

	PointerButton TextureWindow_Impl::decode_id(uicore::InputCode ic) const
	{
		switch (ic)
		{
		default:
			return PointerButton::none;
		case uicore::InputCode::mouse_left:
			return PointerButton::left;
		case uicore::InputCode::mouse_right:
			return PointerButton::right;
		case uicore::InputCode::mouse_middle:
			return PointerButton::middle;
		case uicore::InputCode::mouse_wheel_up:
			return PointerButton::wheel_up;
		case uicore::InputCode::mouse_wheel_down:
			return PointerButton::wheel_down;
		case uicore::InputCode::mouse_xbutton1:
			return PointerButton::xbutton1;
		case uicore::InputCode::mouse_xbutton2:
			return PointerButton::xbutton2;
		}
	}

	void TextureWindow_Impl::transform_on_key_down(const uicore::InputEvent &e)
	{
		on_key_down(transform_input_event(e));
	}

	void TextureWindow_Impl::transform_on_key_up(const uicore::InputEvent &e)
	{
		on_key_up(transform_input_event(e));
	}

	void TextureWindow_Impl::transform_on_mouse_down(const uicore::InputEvent &e)
	{
		on_mouse_down(transform_input_event(e));
	}

	void TextureWindow_Impl::transform_on_mouse_dblclk(const uicore::InputEvent &e)
	{
		on_mouse_dblclk(transform_input_event(e));
	}

	void TextureWindow_Impl::transform_on_mouse_up(const uicore::InputEvent &e)
	{
		on_mouse_up(transform_input_event(e));
	}

	void TextureWindow_Impl::transform_on_mouse_move(const uicore::InputEvent &uicore_event)
	{
		on_mouse_move(transform_input_event(uicore_event));
	}

}
