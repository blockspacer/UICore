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

#pragma once

#include "UICore/Display/Window/input_device.h"
#include "UICore/Display/Window/input_device_provider.h"
#include "hid.h"
#include <map>

namespace uicore
{
	class InputDeviceProvider_Win32Hid : public InputDeviceProvider
	{
	public:
		InputDeviceProvider_Win32Hid(HANDLE rawinput_device);
		~InputDeviceProvider_Win32Hid();

		std::string name() const override;
		std::string device_name() const override;
		InputDevice::Type type() const override;
		std::string key_name(int id) const override;
		bool keycode(int keycode) const override;
		float axis(int index) const override;
		std::vector<int> axis_ids() const override;
		int hat(int index) const override;
		int button_count() const override;

		void update(const std::shared_ptr<InputDevice> &joystick, RAWINPUT *raw_input);

	private:
		void on_dispose();

		HANDLE open_device();
		std::shared_ptr<DataBuffer> get_preparse_data();

		void find_names(HANDLE device);
		void find_button_names(HANDLE device, void *preparse_data);
		void find_value_names(HANDLE device, void *preparse_data);

		void update(void *preparse_data, void *report, int report_size);
		void update_buttons(void *preparse_data, void *report, int report_size);
		void update_values(void *preparse_data, void *report, int report_size);

		Hid hid;
		HANDLE rawinput_device;

		std::string product_name;
		std::string manufacturer_name;
		std::string serial_number;

		std::vector<bool> buttons;
		std::vector<float> axis_values;
		std::vector<int> hat_values;

		std::vector<int> _axis_ids;

		std::vector<std::string> button_names;
		std::vector<std::string> axis_names;
		std::vector<std::string> hat_names;

		std::map<Hid::USAGE, int> usage_to_button_index;
		std::map<Hid::USAGE, int> usage_to_axis_index;
		std::map<Hid::USAGE, int> usage_to_hat_index;

		friend class Win32Window;
	};
}
