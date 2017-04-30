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

#include "UICore/Display/Render/uniform_buffer.h"
#include "UICore/Core/System/disposable_object.h"

namespace uicore
{
	class GL1UniformBuffer : public UniformBuffer
	{
	public:
		GL1UniformBuffer(int size, BufferUsage usage);
		GL1UniformBuffer(const void *data, int size, BufferUsage usage);
		~GL1UniformBuffer();

		void *get_data() const { return data; }

		void upload_data(const std::shared_ptr<GraphicContext> &gc, const void *data, int size) override;
		void copy_from(const std::shared_ptr<GraphicContext> &gc, const std::shared_ptr<StagingBuffer> &buffer, int dest_pos, int src_pos, int size) override;
		void copy_to(const std::shared_ptr<GraphicContext> &gc, const std::shared_ptr<StagingBuffer> &buffer, int dest_pos, int src_pos, int size) override;

	private:
		char *data = nullptr;
		int size = 0;
	};
}
