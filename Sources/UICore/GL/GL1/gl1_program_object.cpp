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
**    Mark Page
*/

#include "UICore/precomp.h"
#include "gl1_program_object.h"
#include "UICore/Display/Render/shader_object.h"
#include "UICore/GL/opengl_wrap.h"
#include "UICore/Core/System/exception.h"
#include "UICore/Core/Text/string_format.h"
#include "UICore/Core/Text/text.h"
#include "gl1_graphic_context.h"

namespace uicore
{
	GL1ProgramObject::GL1ProgramObject(GL1GraphicContext *gc_provider) : gc_provider(gc_provider)
	{
	}

	GL1ProgramObject::~GL1ProgramObject()
	{
	}

	void GL1ProgramObject::on_dispose()
	{

	}

	unsigned int GL1ProgramObject::get_handle() const
	{
		return 0;
	}

	bool GL1ProgramObject::get_link_status() const
	{
		return false;
	}

	bool GL1ProgramObject::get_validate_status() const
	{
		return false;

	}

	std::vector<std::shared_ptr<ShaderObject>> GL1ProgramObject::shaders() const
	{
		return _shaders;
	}

	std::string GL1ProgramObject::info_log() const
	{
		return std::string("Not implemented");
	}

	int GL1ProgramObject::uniform_location(const std::string &name) const
	{
		return -1;
	}

	int GL1ProgramObject::attribute_location(const std::string &name) const
	{
		return -1;
	}

	int GL1ProgramObject::uniform_buffer_size(int block_index) const
	{
		return 0;
	}

	int GL1ProgramObject::uniform_buffer_index(const std::string &block_name) const
	{
		return -1;
	}

	int GL1ProgramObject::storage_buffer_index(const std::string &name) const
	{
		return -1;
	}

	void GL1ProgramObject::attach(const std::shared_ptr<ShaderObject> &obj)
	{
		throw_if_disposed();
		_shaders.push_back(obj);
	}

	void GL1ProgramObject::detach(const std::shared_ptr<ShaderObject> &obj)
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
	}

	void GL1ProgramObject::bind_attribute_location(int index, const std::string &name)
	{
	}

	void GL1ProgramObject::bind_frag_data_location(int color_number, const std::string &name)
	{
	}

	bool GL1ProgramObject::try_link()
	{
		return true;
	}

	bool GL1ProgramObject::validate()
	{
		return true;
	}

	void GL1ProgramObject::set_uniform1i(int location, int p1)
	{
	}


	void GL1ProgramObject::set_uniform2i(int location, int v1, int v2)
	{
	}

	void GL1ProgramObject::set_uniform3i(int location, int v1, int v2, int v3)
	{
	}

	void GL1ProgramObject::set_uniform4i(int location, int v1, int v2, int v3, int v4)
	{
	}

	void GL1ProgramObject::set_uniformiv(int location, int size, int count, const int *data)
	{
	}

	void GL1ProgramObject::set_uniform1f(int location, float v1)
	{
	}

	void GL1ProgramObject::set_uniform2f(int location, float v1, float v2)
	{
	}

	void GL1ProgramObject::set_uniform3f(int location, float v1, float v2, float v3)
	{
	}

	void GL1ProgramObject::set_uniform4f(int location, float v1, float v2, float v3, float v4)
	{
	}

	void GL1ProgramObject::set_uniformfv(int location, int size, int count, const float *data)
	{
	}

	void GL1ProgramObject::set_uniform_matrix(int location, int size, int count, bool transpose, const float *data)
	{
	}

	void GL1ProgramObject::set_uniform_buffer_index(int block_index, int bind_index)
	{
	}

	void GL1ProgramObject::set_storage_buffer_index(int buffer_index, int bind_unit_index)
	{
	}
}
