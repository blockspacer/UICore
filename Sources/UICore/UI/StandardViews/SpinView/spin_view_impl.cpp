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
**    Harry Storbacka
**    Magnus Norddahl
**    Kenneth Gangstoe
**    Mark Page
*/

#include "UICore/precomp.h"
#include "UICore/UI/StandardViews/spin_view.h"
#include "UICore/UI/Style/style_property_parser.h"
#include "UICore/Display/2D/path.h"
#include "UICore/Display/System/timer.h"
#include "UICore/Display/2D/brush.h"
#include "UICore/UI/Events/pointer_event.h"
#include <algorithm>
#include "spin_view_impl.h"

namespace uicore
{
	void SpinBaseView_Impl::update_decrement_state()
	{
		set_standard_state(button_decrement.get(), _state_disabled, _state_decrement_hot, _state_decrement_pressed);
	}
	void SpinBaseView_Impl::update_increment_state()
	{
		set_standard_state(button_increment.get(), _state_disabled, _state_increment_hot, _state_increment_pressed);
	}

	void SpinBaseView_Impl::set_standard_state(View *view, bool state_disabled, bool state_hot, bool state_pressed)
	{
		bool target_hot = false;
		bool target_disabled = false;
		bool target_pressed = false;

		if (state_disabled)
		{
			target_disabled = true;
		}
		else if (state_pressed)
		{
			target_pressed = true;
		}
		else if (state_hot)
		{
			target_hot = true;
		}

		//view->set_state_cascade("hot", target_hot);
		//view->set_state_cascade("pressed", target_pressed);
		//view->set_state_cascade("disabled", target_disabled);
	}

	void SpinBaseView_Impl::on_focus_gained(FocusChangeEvent &e)
	{
	}

	void SpinBaseView_Impl::on_focus_lost(FocusChangeEvent &e)
	{
		mouse_down_mode = mouse_down_none;
		timer->stop();
	}

	void SpinBaseView_Impl::on_activated(ActivationChangeEvent &e)
	{
	}

	void SpinBaseView_Impl::on_deactivated(ActivationChangeEvent &e)
	{
		mouse_down_mode = mouse_down_none;
		timer->stop();
	}


	void SpinBaseView_Impl::on_pointer_decrement_press(PointerEvent &e)
	{
		if (_state_disabled || e.button() != PointerButton::left)
			return;
		_state_decrement_pressed = true;
		update_decrement_state();
		mouse_down_mode = mouse_down_button_decr;
		timer_expired();
	}

	void SpinBaseView_Impl::on_pointer_decrement_release(PointerEvent &e)
	{
		if (_state_disabled || e.button() != PointerButton::left)
			return;
		_state_decrement_pressed = false;
		update_decrement_state();
		mouse_down_mode = mouse_down_none;
		timer->stop();
	}

	void SpinBaseView_Impl::on_pointer_increment_press(PointerEvent &e)
	{
		if (_state_disabled || e.button() != PointerButton::left)
			return;
		_state_increment_pressed = true;
		update_increment_state();
		mouse_down_mode = mouse_down_button_incr;
		timer_expired();
	}

	void SpinBaseView_Impl::on_pointer_increment_release(PointerEvent &e)
	{
		if (_state_disabled || e.button() != PointerButton::left)
			return;
		_state_increment_pressed = false;
		update_increment_state();
		mouse_down_mode = mouse_down_none;
		timer->stop();
	}

	void SpinBaseView_Impl::timer_expired()
	{
		if (_state_disabled)
			return;
		if (mouse_down_mode == mouse_down_none)
			return;

		timer->start(100, false);

	}
}
