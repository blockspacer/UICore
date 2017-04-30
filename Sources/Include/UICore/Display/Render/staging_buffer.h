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
#include "buffer_usage.h"

namespace uicore
{
	class GraphicContext;

	/// \brief Transfer Buffer
	class StagingBuffer
	{
	public:
		/// \brief Constructs a transfer buffer
		static std::shared_ptr<StagingBuffer> create(const std::shared_ptr<GraphicContext> &gc, int size, BufferUsage usage = usage_dynamic_copy);
		static std::shared_ptr<StagingBuffer> create(const std::shared_ptr<GraphicContext> &gc, const void *data, int size, BufferUsage usage = usage_dynamic_copy);

		/// \brief Retrieves a pointer to the mapped buffer.
		virtual void *data() = 0;

		/// \brief Maps buffer into system memory.
		virtual void lock(const std::shared_ptr<GraphicContext> &gc, BufferAccess access) = 0;

		/// \brief Unmaps buffer.
		virtual void unlock() = 0;

		/// \brief Uploads data to transfer buffer.
		virtual void upload_data(const std::shared_ptr<GraphicContext> &gc, int offset, const void *data, int size) = 0;
	};
}
