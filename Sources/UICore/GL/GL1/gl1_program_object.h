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

#pragma once


#include "UICore/GL/opengl_wrap.h"
#include "UICore/GL/opengl.h"
#include "UICore/Display/Render/program_object_impl.h"
#include "UICore/Core/System/disposable_object.h"

namespace uicore
{
	class GL1GraphicContext;

	class GL1ProgramObject : public ProgramObjectImpl, DisposableObject
	{
	public:
		GL1ProgramObject(GL1GraphicContext *gc_provider);
		virtual ~GL1ProgramObject();

		/// \brief Returns the OpenGL program object handle.
		unsigned int get_handle() const;

		/// \brief Returns true if the link succeeded.
		bool get_link_status() const;

		/// \brief Returns true if validation succeeded.
		bool get_validate_status() const;

		/// \brief Returns the current info log for the program object.
		std::string info_log() const override;

		/// \brief Returns the shaders used in this program.
		std::vector<std::shared_ptr<ShaderObject>> shaders() const override;

		/// \brief Returns the location of a named active attribute.
		int attribute_location(const std::string &name) const override;

		/// \brief Returns the location of a named uniform variable.
		int uniform_location(const std::string &name) const override;

		using ProgramObject::uniform_buffer_size;
		int uniform_buffer_size(int block_index) const override;
		using ProgramObject::uniform_buffer_index;
		int uniform_buffer_index(const std::string &block_name) const override;
		using ProgramObject::storage_buffer_index;
		int storage_buffer_index(const std::string &name) const override;

		/// \brief Add shader to program object.
		void attach(const std::shared_ptr<ShaderObject> &obj) override;

		/// \brief Remove shader from program object.
		void detach(const std::shared_ptr<ShaderObject> &obj) override;

		/// \brief Bind attribute to specific location.
		/** <p>This function must be called before linking.</p>*/
		void bind_attribute_location(int index, const std::string &name) override;

		/// \brief Bind shader out variable a specific color buffer location.
		/** <p>This function must be called before linking.</p>*/
		void bind_frag_data_location(int color_number, const std::string &name) override;

		/// \brief Link program.
		/** <p>If the linking fails, get_link_status() will return false and
			get_info_log() will return the link log.</p>*/
		bool try_link() override;

		/// \brief Validate program.
		/** <p>If the validation fails, get_validate_status() will return
			false and get_info_log() will return the validation log.</p>*/
		bool validate() override;

		/// \brief Set uniform variable(s).
		void set_uniform1i(int location, int) override;
		void set_uniform2i(int location, int, int) override;
		void set_uniform3i(int location, int, int, int) override;
		void set_uniform4i(int location, int, int, int, int) override;
		void set_uniformiv(int location, int size, int count, const int *data) override;
		void set_uniform1f(int location, float) override;
		void set_uniform2f(int location, float, float) override;
		void set_uniform3f(int location, float, float, float) override;
		void set_uniform4f(int location, float, float, float, float) override;
		void set_uniformfv(int location, int size, int count, const float *data) override;
		void set_uniform_matrix(int location, int size, int count, bool transpose, const float *data) override;

		using ProgramObject::set_uniform_buffer_index;
		void set_uniform_buffer_index(int block_index, int bind_index) override;
		using ProgramObject::set_storage_buffer_index;
		void set_storage_buffer_index(int buffer_index, int bind_unit_index) override;

	private:
		void on_dispose() override;

		std::vector<std::shared_ptr<ShaderObject>> _shaders;

		Mat4f modelview_matrix;
		Mat4f projection_matrix;

		GL1GraphicContext *gc_provider;

		friend class GL1GraphicContext;
	};
}
