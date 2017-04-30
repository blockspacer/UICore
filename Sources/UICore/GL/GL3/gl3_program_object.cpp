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
**    Harry Storbacka
*/

#include "UICore/precomp.h"
#include "gl3_program_object.h"
#include "gl3_shader_object.h"
#include "UICore/Display/Render/shader_object.h"
#include "UICore/GL/opengl_wrap.h"
#include "UICore/Core/System/exception.h"
#include "UICore/Core/Text/string_format.h"
#include "UICore/Core/Text/text.h"
#include "gl3_graphic_context.h"
#include "gl3_uniform_buffer.h"

namespace uicore
{
	GL3ProgramObject::GL3ProgramObject()
	{
		OpenGL::set_active();
		handle = glCreateProgram();
	}

	GL3ProgramObject::~GL3ProgramObject()
	{
		dispose();
	}

	void GL3ProgramObject::on_dispose()
	{
		if (handle)
		{
			if (OpenGL::set_active())
			{
				glDeleteProgram(handle);
				handle = 0;
			}
		}
	}

	unsigned int GL3ProgramObject::get_handle() const
	{
		throw_if_disposed();
		return handle;
	}

	bool GL3ProgramObject::get_link_status() const
	{
		throw_if_disposed();
		OpenGL::set_active();
		GLint status = 0;
		glGetProgramiv(handle, GL_LINK_STATUS, &status);
		return (status != GL_FALSE);
	}

	bool GL3ProgramObject::get_validate_status() const
	{
		throw_if_disposed();
		OpenGL::set_active();
		GLint status = 0;
		glGetProgramiv(handle, GL_VALIDATE_STATUS, &status);
		return (status != GL_FALSE);
	}

	std::vector<std::shared_ptr<ShaderObject>> GL3ProgramObject::shaders() const
	{
		throw_if_disposed();
		return _shaders;
	}

	std::string GL3ProgramObject::info_log() const
	{
		throw_if_disposed();
		OpenGL::set_active();
		std::string result;
		GLsizei buffer_size = 16 * 1024;
		while (buffer_size < 2 * 1024 * 1024)
		{
			auto info_log = new GLchar[buffer_size];
			GLsizei length = 0;
			glGetProgramInfoLog(handle, buffer_size, &length, info_log);
			if (length < buffer_size - 1)
				result = std::string(info_log, length);
			delete[] info_log;
			if (length < buffer_size - 1)
				break;
			buffer_size *= 2;
		}
		return result;
	}

	int GL3ProgramObject::uniform_location(const std::string &name) const
	{
		throw_if_disposed();
		OpenGL::set_active();
		return glGetUniformLocation(handle, name.c_str());
	}

	int GL3ProgramObject::attribute_location(const std::string &name) const
	{
		throw_if_disposed();
		OpenGL::set_active();
		return glGetAttribLocation(handle, name.c_str());
	}

	int GL3ProgramObject::uniform_buffer_size(int block_index) const
	{
		throw_if_disposed();
		OpenGL::set_active();

		if (!glGetActiveUniformBlockiv)
			throw Exception("incorrect OpenGL version");

		GLsizei uniformBlockSize = 0;
		glGetActiveUniformBlockiv(handle, block_index, GL_UNIFORM_BLOCK_DATA_SIZE, &uniformBlockSize);

		return uniformBlockSize;
	}

	int GL3ProgramObject::uniform_buffer_index(const std::string &block_name) const
	{
		throw_if_disposed();
		OpenGL::set_active();

		if (!glGetUniformBlockIndex)
			throw Exception("incorrect OpenGL version");

		return glGetUniformBlockIndex(handle, block_name.c_str());
	}

	int GL3ProgramObject::storage_buffer_index(const std::string &name) const
	{
		throw_if_disposed();
		OpenGL::set_active();

		if (!glGetProgramResourceIndex)
			throw Exception("incorrect OpenGL version");

		return glGetProgramResourceIndex(handle, GL_SHADER_STORAGE_BLOCK, name.c_str());
	}

	void GL3ProgramObject::attach(const std::shared_ptr<ShaderObject> &obj)
	{
		throw_if_disposed();
		_shaders.push_back(obj);
		OpenGL::set_active();
		glAttachShader(handle, (GLuint)static_cast<GL3ShaderObject*>(obj.get())->get_handle());
	}

	void GL3ProgramObject::detach(const std::shared_ptr<ShaderObject> &obj)
	{
		throw_if_disposed();
		for (std::vector<ShaderObject>::size_type i = 0; i < _shaders.size(); i++)
		{
			if (_shaders[i] == obj)
			{
				_shaders.erase(_shaders.begin() + i);
				break;
			}
		}
		OpenGL::set_active();
		glDetachShader(handle, (GLuint)static_cast<GL3ShaderObject*>(obj.get())->get_handle());
	}

	void GL3ProgramObject::bind_attribute_location(int index, const std::string &name)
	{
		throw_if_disposed();
		OpenGL::set_active();
		glBindAttribLocation(handle, index, name.c_str());
	}

	void GL3ProgramObject::bind_frag_data_location(int color_number, const std::string &name)
	{
		throw_if_disposed();
		OpenGL::set_active();
		glBindFragDataLocation(handle, color_number, name.c_str());
	}

	bool GL3ProgramObject::try_link()
	{
		throw_if_disposed();
		OpenGL::set_active();
		glLinkProgram(handle);
		return get_link_status();
	}

	bool GL3ProgramObject::validate()
	{
		throw_if_disposed();
		OpenGL::set_active();
		glValidateProgram(handle);
		return get_validate_status();
	}

	void GL3ProgramObject::set_uniform1i(int location, int p1)
	{
		throw_if_disposed();
		if (location >= 0)
		{
			ProgramObjectStateTracker state_tracker(handle);
			glUniform1i(location, p1);
		}
	}

	void GL3ProgramObject::set_uniform2i(int location, int v1, int v2)
	{
		throw_if_disposed();
		if (location >= 0)
		{
			ProgramObjectStateTracker state_tracker(handle);
			glUniform2i(location, v1, v2);
		}
	}

	void GL3ProgramObject::set_uniform3i(int location, int v1, int v2, int v3)
	{
		throw_if_disposed();
		if (location >= 0)
		{
			ProgramObjectStateTracker state_tracker(handle);
			glUniform3i(location, v1, v2, v3);
		}
	}

	void GL3ProgramObject::set_uniform4i(int location, int v1, int v2, int v3, int v4)
	{
		throw_if_disposed();
		if (location >= 0)
		{
			ProgramObjectStateTracker state_tracker(handle);
			glUniform4i(location, v1, v2, v3, v4);
		}
	}

	void GL3ProgramObject::set_uniformiv(int location, int size, int count, const int *data)
	{
		throw_if_disposed();
		if (location >= 0)
		{
			ProgramObjectStateTracker state_tracker(handle);
			if (size == 1) glUniform1iv(location, count, data);
			else if (size == 2) glUniform2iv(location, count, data);
			else if (size == 3) glUniform3iv(location, count, data);
			else if (size == 4) glUniform4iv(location, count, data);
		}
	}

	void GL3ProgramObject::set_uniform1f(int location, float v1)
	{
		throw_if_disposed();
		if (location >= 0)
		{
			ProgramObjectStateTracker state_tracker(handle);
			glUniform1f(location, v1);
		}
	}

	void GL3ProgramObject::set_uniform2f(int location, float v1, float v2)
	{
		throw_if_disposed();
		if (location >= 0)
		{
			ProgramObjectStateTracker state_tracker(handle);
			glUniform2f(location, v1, v2);
		}
	}

	void GL3ProgramObject::set_uniform3f(int location, float v1, float v2, float v3)
	{
		throw_if_disposed();
		if (location >= 0)
		{
			ProgramObjectStateTracker state_tracker(handle);
			glUniform3f(location, v1, v2, v3);
		}
	}

	void GL3ProgramObject::set_uniform4f(int location, float v1, float v2, float v3, float v4)
	{
		throw_if_disposed();
		if (location >= 0)
		{
			ProgramObjectStateTracker state_tracker(handle);
			glUniform4f(location, v1, v2, v3, v4);
		}
	}

	void GL3ProgramObject::set_uniformfv(int location, int size, int count, const float *data)
	{
		throw_if_disposed();
		if (location >= 0)
		{
			ProgramObjectStateTracker state_tracker(handle);
			if (size == 1) glUniform1fv(location, count, data);
			else if (size == 2) glUniform2fv(location, count, data);
			else if (size == 3) glUniform3fv(location, count, data);
			else if (size == 4) glUniform4fv(location, count, data);
		}
	}

	void GL3ProgramObject::set_uniform_matrix(int location, int size, int count, bool transpose, const float *data)
	{
		throw_if_disposed();
		if (location >= 0)
		{
			ProgramObjectStateTracker state_tracker(handle);
			if (size == 2) glUniformMatrix2fv(location, count, transpose, data);
			else if (size == 3) glUniformMatrix3fv(location, count, transpose, data);
			else if (size == 4) glUniformMatrix4fv(location, count, transpose, data);
		}
	}

	void GL3ProgramObject::set_uniform_buffer_index(int block_index, int bind_index)
	{
		throw_if_disposed();
		if (block_index == -1)
			return;
		glUniformBlockBinding(handle, block_index, bind_index);
	}

	void GL3ProgramObject::set_storage_buffer_index(int buffer_index, int bind_unit_index)
	{
		throw_if_disposed();
		if (buffer_index == -1)
			return;
		glShaderStorageBlockBinding(handle, buffer_index, bind_unit_index);
	}

	/////////////////////////////////////////////////////////////////////////////

	ProgramObjectStateTracker::ProgramObjectStateTracker(GLuint handle) : program_set(false)
	{
		OpenGL::set_active();

		glGetIntegerv(GL_CURRENT_PROGRAM, (GLint *)&last_program_object);
		if (handle != last_program_object)
		{
			program_set = true;
			glUseProgram(handle);
		}
	}

	ProgramObjectStateTracker::~ProgramObjectStateTracker()
	{
		if (program_set)
			glUseProgram(last_program_object);
	}
}
