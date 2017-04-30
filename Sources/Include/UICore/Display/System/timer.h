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

#include <memory>
#include <functional>

namespace uicore
{
	/// \brief Timer class that invokes a callback on a specified interval
	class Timer
	{
	public:
		/// \brief Constructs a timer object
		static std::shared_ptr<Timer> create();

		/// \brief Returns true if the timer repeats until it is stopped
		virtual bool repeating() const = 0;

		/// \brief Returns the current timeout. In milliseconds.
		virtual unsigned int timeout() const = 0;

		/// \brief Callback invoked every time the timer interval occurs
		virtual std::function<void()> &func_expired() = 0;

		/// \brief Starts the timer. Timeout in milliseconds.
		virtual void start(unsigned int timeout, bool repeat = true) = 0;

		/// \brief Stop the timer.
		virtual void stop() = 0;
	};
}
