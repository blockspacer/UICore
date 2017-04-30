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

#include "UICore/precomp.h"
#include "gl3_buffer_object.h"
#include "gl3_graphic_context.h"
#include "gl3_staging_buffer.h"
#include "UICore/GL/opengl_wrap.h"

namespace uicore
{
	GL3BufferObject::GL3BufferObject()
		: handle(0), data_ptr(nullptr)
	{
		OpenGL::set_active();

		glGenBuffers(1, &handle);
	}

	GL3BufferObject::~GL3BufferObject()
	{
		dispose();
	}

	void GL3BufferObject::on_dispose()
	{
		if (handle)
		{
			if (OpenGL::set_active())
			{
				glDeleteBuffers(1, &handle);
			}
		}
	}

	void GL3BufferObject::create(const void *data, int new_size, BufferUsage usage, GLenum new_binding, GLenum new_target)
	{
		throw_if_disposed();

		binding = new_binding;
		target = new_target;
		size = new_size;

		OpenGL::set_active();

		GLint last_buffer = 0;
		if (binding)
			glGetIntegerv(binding, &last_buffer);
		glBindBuffer(target, handle);
		if (glBufferStorage) // To do: redesign BufferUsage enum to something less useless!
			glBufferStorage(target, size, data, usage == usage_stream_draw ? GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT : GL_MAP_WRITE_BIT | GL_MAP_READ_BIT | GL_DYNAMIC_STORAGE_BIT);
		else
			glBufferData(target, size, data, OpenGL::to_enum(usage));
		glBindBuffer(target, last_buffer);
	}

	void *GL3BufferObject::get_data()
	{
		if (data_ptr == nullptr)
			throw Exception("PixelBuffer was not locked");
		return data_ptr;
	}

	const void *GL3BufferObject::get_data() const
	{
		if (data_ptr == nullptr)
			throw Exception("PixelBuffer was not locked");
		return data_ptr;
	}

	void GL3BufferObject::lock(const std::shared_ptr<GraphicContext> &gc, BufferAccess access)
	{
		throw_if_disposed();
		lock_gc = gc;
		OpenGL::set_active(lock_gc);
		GLint last_buffer = 0;
		if (binding)
			glGetIntegerv(binding, &last_buffer);
		glBindBuffer(target, handle);
		GLbitfield flags = 0;
		switch (access)
		{
		case access_read_only: flags = GL_MAP_READ_BIT; break;
		case access_write_only: flags = GL_MAP_WRITE_BIT; break;
		case access_read_write: flags = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT; break;
		case access_write_discard: flags = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT; break;
		}
		if (glMapBufferRange)
			data_ptr = (void *)glMapBufferRange(target, 0, size, flags);
		else
			data_ptr = (void *)glMapBuffer(target, OpenGL::to_enum(access));
		glBindBuffer(target, last_buffer);
	}

	void GL3BufferObject::unlock()
	{
		throw_if_disposed();
		OpenGL::set_active(lock_gc);
		GLint last_buffer = 0;
		if (binding)
			glGetIntegerv(binding, &last_buffer);
		glBindBuffer(target, handle);
		glUnmapBuffer(target);
		glBindBuffer(target, last_buffer);
		data_ptr = nullptr;
		lock_gc.reset();
	}

	void GL3BufferObject::upload_data(const std::shared_ptr<GraphicContext> &gc, int offset, const void *data, int size)
	{
		throw_if_disposed();
		OpenGL::set_active(gc);
		GLint last_buffer = 0;
		if (binding)
			glGetIntegerv(binding, &last_buffer);
		glBindBuffer(target, handle);
		glBufferSubData(target, offset, size, data);
		glBindBuffer(target, last_buffer);
	}

	void GL3BufferObject::upload_data(const std::shared_ptr<GraphicContext> &gc, const void *data, int size)
	{
		upload_data(gc, 0, data, size);
	}

	void GL3BufferObject::copy_from(const std::shared_ptr<GraphicContext> &gc, const std::shared_ptr<StagingBuffer> &buffer, int dest_pos, int src_pos, int size)
	{
		throw_if_disposed();
		OpenGL::set_active(gc);

		glBindBuffer(GL_COPY_WRITE_BUFFER, handle);
		glBindBuffer(GL_COPY_READ_BUFFER, static_cast<GL3StagingBuffer*>(buffer.get())->get_handle());

		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, src_pos, dest_pos, size);

		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
		glBindBuffer(GL_COPY_READ_BUFFER, 0);
	}

	void GL3BufferObject::copy_to(const std::shared_ptr<GraphicContext> &gc, const std::shared_ptr<StagingBuffer> &buffer, int dest_pos, int src_pos, int size)
	{
		throw_if_disposed();
		OpenGL::set_active(gc);

		glBindBuffer(GL_COPY_WRITE_BUFFER, static_cast<GL3StagingBuffer*>(buffer.get())->get_handle());
		glBindBuffer(GL_COPY_READ_BUFFER, handle);

		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, dest_pos, src_pos, size);

		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
		glBindBuffer(GL_COPY_READ_BUFFER, 0);
	}
}
