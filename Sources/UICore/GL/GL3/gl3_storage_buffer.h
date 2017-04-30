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

#include "UICore/Display/Render/storage_buffer.h"
#include "UICore/GL/opengl.h"
#include "UICore/Core/System/disposable_object.h"
#include "gl3_buffer_object.h"

namespace uicore
{
	class GL3GraphicContext;

	class GL3StorageBuffer : public StorageBuffer
	{
	public:
		GL3StorageBuffer(int size, int stride, BufferUsage usage);
		GL3StorageBuffer(const void *data, int size, int stride, BufferUsage usage);

		GLuint get_handle() const { return buffer.get_handle(); }

		void upload_data(const std::shared_ptr<GraphicContext> &gc, const void *data, int size) override { buffer.upload_data(gc, data, size); }
		void copy_from(const std::shared_ptr<GraphicContext> &gc, const std::shared_ptr<StagingBuffer> &staging_buffer, int dest_pos, int src_pos, int size) override { buffer.copy_from(gc, staging_buffer, dest_pos, src_pos, size); }
		void copy_to(const std::shared_ptr<GraphicContext> &gc, const std::shared_ptr<StagingBuffer> &staging_buffer, int dest_pos, int src_pos, int size) override { buffer.copy_to(gc, staging_buffer, dest_pos, src_pos, size); }

	private:
		GL3BufferObject buffer;
	};
}
