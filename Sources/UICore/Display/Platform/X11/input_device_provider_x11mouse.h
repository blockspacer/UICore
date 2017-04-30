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
**    Mark Page
*/

#pragma once

#include "UICore/Display/Window/input_device_provider.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace uicore
{
	class X11Window;

	class InputDeviceProvider_X11Mouse : public InputDeviceProvider
	{
	public:
		InputDeviceProvider_X11Mouse(X11Window *window);
		~InputDeviceProvider_X11Mouse();

		InputDevice::Type type() const override { return InputDevice::pointer; }
		Pointf position() const override;
		Point device_position() const override;
		bool keycode(int keycode) const override;
		std::string key_name(int id) const override;
		std::string name() const override;
		std::string device_name() const override;
		int button_count() const override;

		void set_position(float x, float y) override;
		void set_device_position(int x, int y) override;

		void received_mouse_input(std::shared_ptr<InputDevice> &mouse, XButtonEvent &event);
		void received_mouse_move(std::shared_ptr<InputDevice> &mouse, XMotionEvent &event);

	private:
		void on_dispose() override;

		bool key_states[32];

		X11Window *window;

		Point mouse_pos;

		Time time_at_last_press;
		int last_press_id;
	};
}
