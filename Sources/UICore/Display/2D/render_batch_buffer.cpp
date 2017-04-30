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
#include "render_batch_buffer.h"
#include "UICore/Display/Render/blend_state_description.h"
#include "UICore/Display/2D/canvas.h"

namespace uicore
{
	RenderBatchBuffer::RenderBatchBuffer(const std::shared_ptr<GraphicContext> &gc)
	{
		for (auto & elem : vertex_buffers)
		{
			elem = VertexArrayBuffer::create(gc, vertex_buffer_size, usage_stream_draw);
		}
	}

	std::shared_ptr<VertexArrayBuffer> RenderBatchBuffer::get_vertex_buffer(const std::shared_ptr<GraphicContext> &gc, int &out_index)
	{
		out_index = current_vertex_buffer;

		current_vertex_buffer++;
		if (current_vertex_buffer == num_vertex_buffers)
			current_vertex_buffer = 0;

		return vertex_buffers[out_index];
	}

	std::shared_ptr<Texture2D> RenderBatchBuffer::get_texture_rgba32f(const std::shared_ptr<GraphicContext> &gc)
	{
		current_rgba32f_texture++;
		if (current_rgba32f_texture == num_r8_buffers)
			current_rgba32f_texture = 0;

		if (!textures_rgba32f[current_rgba32f_texture])
		{
			textures_rgba32f[current_rgba32f_texture] = Texture2D::create(gc, rgba32f_width, rgba32f_height, tf_rgba32f);
			textures_rgba32f[current_rgba32f_texture]->set_min_filter(filter_nearest);
			textures_rgba32f[current_rgba32f_texture]->set_mag_filter(filter_nearest);

		}

		return textures_rgba32f[current_rgba32f_texture];
	}

	std::shared_ptr<Texture2D> RenderBatchBuffer::get_texture_r8(const std::shared_ptr<GraphicContext> &gc)
	{
		current_r8_texture++;
		if (current_r8_texture == num_r8_buffers)
			current_r8_texture = 0;

		if (!textures_r8[current_r8_texture])
		{
			textures_r8[current_r8_texture] = Texture2D::create(gc, r8_size, r8_size, tf_r8);
			textures_r8[current_r8_texture]->set_min_filter(filter_nearest);
			textures_r8[current_r8_texture]->set_mag_filter(filter_nearest);
		}
		return textures_r8[current_r8_texture];
	}

	std::shared_ptr<StagingTexture> RenderBatchBuffer::get_transfer_rgba32f(const std::shared_ptr<GraphicContext> &gc)
	{
		current_rgba32f_transfer++;
		if (current_rgba32f_transfer == num_r8_buffers)
			current_rgba32f_transfer = 0;

		if (!transfers_rgba32f[current_rgba32f_transfer])
			transfers_rgba32f[current_rgba32f_transfer] = StagingTexture::create(gc, rgba32f_width, rgba32f_height, StagingDirection::to_gpu, tf_rgba32f);

		return transfers_rgba32f[current_rgba32f_transfer];
	}

	std::shared_ptr<StagingTexture> RenderBatchBuffer::get_transfer_r8(const std::shared_ptr<GraphicContext> &gc, int &out_index)
	{
		current_r8_transfer++;
		if (current_r8_transfer == num_r8_buffers)
			current_r8_transfer = 0;

		if (!transfers_r8[current_r8_transfer])
		{
			transfers_r8[current_r8_transfer] = StagingTexture::create(gc, r8_size, r8_size, StagingDirection::to_gpu, tf_r8);
		}
		out_index = current_r8_transfer;

		return transfers_r8[current_r8_transfer];
	}
}
