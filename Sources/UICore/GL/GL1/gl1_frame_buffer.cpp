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
#include "UICore/Display/Render/render_buffer.h"
#include "UICore/Core/Text/string_format.h"
#include "UICore/Display/Render/texture.h"
#include "UICore/GL/opengl_wrap.h"
#include "UICore/GL/opengl.h"
#include "gl1_frame_buffer.h"
#include "gl1_render_buffer.h"
#include "gl1_texture_object.h"

namespace uicore
{
	GL1FrameBuffer::GL1FrameBuffer(GL1GraphicContext *gc_provider) :
		gc_provider(gc_provider)
	{
		gc_provider->add_disposable(this);
	}

	GL1FrameBuffer::~GL1FrameBuffer()
	{
		dispose();
	}

	Size GL1FrameBuffer::size() const
	{
		if (!selected_surface)
			return Size();
		return selected_surface->size();
	}

	FrameBufferBindTarget GL1FrameBuffer::bind_target() const
	{
		return framebuffer_draw;
	}

	void GL1FrameBuffer::attach_color(int attachment_index, const std::shared_ptr<RenderBuffer> &render_buffer) {}
	void GL1FrameBuffer::attach_color(int attachment_index, const std::shared_ptr<Texture1D> &texture, int level) {}
	void GL1FrameBuffer::attach_color(int attachment_index, const std::shared_ptr<Texture1DArray> &texture, int array_index, int level) {}

	void GL1FrameBuffer::attach_color(int attachment_index, const std::shared_ptr<Texture2D> &texture, int level)
	{
		throw_if_disposed();

		if (!pbuffer.is_null())
			sync_texture();

		GL1TextureObject *texture_provider = dynamic_cast<GL1TextureObject*>(texture->texture_object());
		if (texture_provider == nullptr)
		{
			throw Exception("Selected texture is not a GL1 texture");
		}

		Size surface_size = texture_provider->get_surface_size();

		// Find existing pbuffer
		std::weak_ptr<Texture2D> texture_impl = texture;
		auto texture_it = texture_pbuffer_map.find(texture_impl);
		if (texture_it == texture_pbuffer_map.end())
		{
			// Not found, create a new entry
			pbuffer = PBuffer_GL1(gc_provider);
			pbuffer.create(gc_provider->get_opengl_window(), surface_size);
			texture_pbuffer_map[texture_impl] = pbuffer;
		}
		else
		{
			// Used cached pbuffer
			pbuffer = texture_it->second;
		}

		selected_texture_provider = texture_provider;
		selected_surface = texture;

		sync_pbuffer();

		// Purge the cache from unused pbuffers. Maybe we could reuse them instead? But for this to occur, the user would be recreating Textures anyway (which is slow). This added complexity is not required (at the moment)
		for (texture_it = texture_pbuffer_map.begin(); texture_it != texture_pbuffer_map.end();)
		{
			if (texture_it->first.expired())
			{
				// This "texture_it = texture_pbuffer_map.erase(texture_it);" works on visual studio,
				// but MSDN says This return type does not conform to the C++ standard. So do it a different way, so GCC does not complain
				texture_pbuffer_map.erase(texture_it);
				texture_it = texture_pbuffer_map.begin();
			}
			else
			{
				++texture_it;
			}
		}
	}
	void GL1FrameBuffer::attach_color(int attachment_index, const std::shared_ptr<Texture2DArray> &texture, int array_index, int level) {}
	void GL1FrameBuffer::attach_color(int attachment_index, const std::shared_ptr<Texture3D> &texture, int depth, int level) {}
	void GL1FrameBuffer::attach_color(int attachment_index, const std::shared_ptr<TextureCube> &texture, TextureSubtype subtype, int level) {}
	void GL1FrameBuffer::detach_color(int attachment_index) {}

	void GL1FrameBuffer::attach_stencil(const std::shared_ptr<RenderBuffer> &render_buffer) {}
	void GL1FrameBuffer::attach_stencil(const std::shared_ptr<Texture2D> &texture, int level) {}
	void GL1FrameBuffer::attach_stencil(const std::shared_ptr<TextureCube> &texture, TextureSubtype subtype, int level) {}
	void GL1FrameBuffer::detach_stencil() {}

	void GL1FrameBuffer::attach_depth(const std::shared_ptr<RenderBuffer> &render_buffer) {}
	void GL1FrameBuffer::attach_depth(const std::shared_ptr<Texture2D> &texture, int level) {}
	void GL1FrameBuffer::attach_depth(const std::shared_ptr<TextureCube> &texture, TextureSubtype subtype, int level) {}
	void GL1FrameBuffer::detach_depth() {}

	void GL1FrameBuffer::attach_depth_stencil(const std::shared_ptr<RenderBuffer> &render_buffer) {}
	void GL1FrameBuffer::attach_depth_stencil(const std::shared_ptr<Texture2D> &texture, int level) {}
	void GL1FrameBuffer::attach_depth_stencil(const std::shared_ptr<TextureCube> &texture, TextureSubtype subtype, int level) {}
	void GL1FrameBuffer::detach_depth_stencil() {}


	void GL1FrameBuffer::set_bind_target(FrameBufferBindTarget target)
	{
	}

	void GL1FrameBuffer::set_state(OpenGLBlendState &new_blend)
	{
		selected_state.blend.set(new_blend);
		selected_state.blend.apply();
	}

	void GL1FrameBuffer::set_state(OpenGLRasterizerState &new_rasterizer)
	{
		selected_state.rasterizer.set(new_rasterizer);
		selected_state.rasterizer.apply();
	}

	void GL1FrameBuffer::set_state(OpenGLDepthStencilState &new_depth_stencil)
	{
		selected_state.depth_stencil.set(new_depth_stencil);
		selected_state.depth_stencil.apply();
	}

	void GL1FrameBuffer::set_state(GL1State &new_selected_state)
	{
		set_state(new_selected_state.blend);
		set_state(new_selected_state.rasterizer);
		set_state(new_selected_state.depth_stencil);

	}

	void GL1FrameBuffer::on_dispose()
	{
		stop();
		pbuffer = PBuffer_GL1();
		texture_pbuffer_map.clear();

		gc_provider->remove_disposable(this);

	}

	void GL1FrameBuffer::stop()
	{
		sync_texture();
	}

	void GL1FrameBuffer::sync_pbuffer()
	{
		if (!selected_surface)
			return;

		set_active();

		Size surface_size = selected_texture_provider->get_surface_size();
		Size texture_size = selected_texture_provider->get_texture_size();

		GLint old_viewport[4], old_matrix_mode;
		GLfloat old_matrix_projection[16], old_matrix_modelview[16];
		glGetIntegerv(GL_VIEWPORT, old_viewport);
		glGetIntegerv(GL_MATRIX_MODE, &old_matrix_mode);
		glGetFloatv(GL_PROJECTION_MATRIX, old_matrix_projection);
		glGetFloatv(GL_MODELVIEW_MATRIX, old_matrix_modelview);
		GLboolean blending = glIsEnabled(GL_BLEND);
		glDisable(GL_BLEND);

		glViewport(0, 0, surface_size.width, surface_size.height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMultMatrixf(Mat4f::ortho_2d(0.0f, (float)surface_size.width, 0.0f, (float)surface_size.height, handed_right, clip_negative_positive_w));
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		// Copy texture to pbuffer:

		glActiveTexture(GL_TEXTURE0 + 0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, selected_texture_provider->get_handle());

		if (glActiveTexture != nullptr)
			glActiveTexture(GL_TEXTURE0 + 0);

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		glMatrixMode(GL_TEXTURE);
		GLfloat scale_matrix[16];
		memset(scale_matrix, 0, sizeof(GLfloat) * 16);
		scale_matrix[0] = 1.0f / float(texture_size.width);
		scale_matrix[5] = 1.0f / float(texture_size.height);
		scale_matrix[10] = 1.0f;
		scale_matrix[15] = 1.0f;
		glLoadMatrixf(scale_matrix);
		glMatrixMode(GL_MODELVIEW);

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		GLfloat vtx1[] = {
			float(surface_size.width), 0.0f, 0.0f,
			float(surface_size.width), float(surface_size.height), 0.0f,
			0.0f, float(surface_size.height), 0.0f,
			0.0f, 0.0f, 0.0f
		};
		GLfloat tex1[] = {
			float(surface_size.width), 0.0f,
			float(surface_size.width), float(surface_size.height),
			0.0f, float(surface_size.height),
			0.0f, 0.0f
		};

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glVertexPointer(3, GL_FLOAT, 0, vtx1);
		glTexCoordPointer(2, GL_FLOAT, 0, tex1);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);

		if (blending)
			glEnable(GL_BLEND);
		glViewport(old_viewport[0], old_viewport[1], old_viewport[2], old_viewport[3]);
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(old_matrix_projection);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(old_matrix_modelview);
		glMatrixMode(old_matrix_mode);

#ifdef __APPLE__
		glFlush();
#endif

		pbuffer_changed = true;
	}

	void GL1FrameBuffer::sync_texture()
	{
		if (pbuffer_changed && (!pbuffer.is_null()) && selected_surface)
		{
			set_active();

			Size surface_size = selected_texture_provider->get_surface_size();

			GLint old_viewport[4], old_matrix_mode;
			GLfloat old_matrix_projection[16], old_matrix_modelview[16];
			glGetIntegerv(GL_VIEWPORT, old_viewport);
			glGetIntegerv(GL_MATRIX_MODE, &old_matrix_mode);
			glGetFloatv(GL_PROJECTION_MATRIX, old_matrix_projection);
			glGetFloatv(GL_MODELVIEW_MATRIX, old_matrix_modelview);
			GLboolean blending = glIsEnabled(GL_BLEND);
			glDisable(GL_BLEND);

			glViewport(0, 0, surface_size.width, surface_size.height);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glMultMatrixf(Mat4f::ortho_2d(0.0f, (float)surface_size.width, 0.0f, (float)surface_size.height, handed_right, clip_negative_positive_w));
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			if (glActiveTexture != nullptr)
				glActiveTexture(GL_TEXTURE0);

#ifndef __ANDROID__
			glPixelZoom(1.0f, 1.0f);
#endif

			// Copy pbuffer to texture:
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, selected_texture_provider->get_handle());
			glCopyTexSubImage2D(
				GL_TEXTURE_2D,
				0,
				0, 0,
				0, 0,
				surface_size.width, surface_size.height);

			glDisable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);

			if (blending)
				glEnable(GL_BLEND);
			glViewport(old_viewport[0], old_viewport[1], old_viewport[2], old_viewport[3]);
			glMatrixMode(GL_PROJECTION);
			glLoadMatrixf(old_matrix_projection);
			glMatrixMode(GL_MODELVIEW);
			glLoadMatrixf(old_matrix_modelview);
			glMatrixMode(old_matrix_mode);

#ifdef __APPLE__
			glFlush();
#endif
		}
		pbuffer_changed = false;
	}

	void GL1FrameBuffer::detach_all()
	{
		stop();
		pbuffer = PBuffer_GL1();
		selected_surface.reset();
	}

	void GL1FrameBuffer::set_active()
	{
		pbuffer.set_active();
		pbuffer_changed = true;
	}

	void GL1FrameBuffer::start()
	{
		if (pbuffer.is_null())
		{
			throw Exception("GL1 Framebuffer does not contain a texture");
		}
	}
}
