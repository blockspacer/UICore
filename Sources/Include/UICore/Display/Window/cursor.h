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

namespace uicore
{
	class DisplayWindow;
	class CursorDescription;

	/// \brief Mouse cursor class.
	class DisplayCursor
	{
	public:
		/// \brief Constructs a mouse cursor
		static std::shared_ptr<DisplayCursor> create(const std::shared_ptr<DisplayWindow> &window, const CursorDescription &cursor_description);

		virtual ~DisplayCursor() { } // To force a virtual table as we have no public virtual functions

	protected:
		DisplayCursor() { }
	};
}
