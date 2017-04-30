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
**    Mark Page
*/

#include "UICore/precomp.h"
#include "render_batch_point.h"
#include "canvas_impl.h"
#include "UICore/Display/Render/blend_state_description.h"
#include "UICore/Display/2D/canvas.h"

namespace uicore
{
	RenderBatchPoint::RenderBatchPoint(const std::shared_ptr<GraphicContext> &gc, RenderBatchBuffer *batch_buffer)
		: batch_buffer(batch_buffer)
	{
		vertices = (PointVertex *)batch_buffer->buffer;
	}

	void RenderBatchPoint::draw_point(const std::shared_ptr<Canvas> &canvas, Vec2f *line_positions, const Vec4f &point_color, int num_vertices)
	{
		set_batcher_active(canvas, num_vertices);

		for (; num_vertices > 0; num_vertices--)
		{
			vertices[position].color = point_color;
			vertices[position].position = to_position(line_positions->x, line_positions->y);
			line_positions++;
			position++;
		}
	}

	inline Vec4f RenderBatchPoint::to_position(float x, float y) const
	{
		return Vec4f(
			modelview_projection_matrix.matrix[0 * 4 + 0] * x + modelview_projection_matrix.matrix[1 * 4 + 0] * y + modelview_projection_matrix.matrix[3 * 4 + 0],
			modelview_projection_matrix.matrix[0 * 4 + 1] * x + modelview_projection_matrix.matrix[1 * 4 + 1] * y + modelview_projection_matrix.matrix[3 * 4 + 1],
			modelview_projection_matrix.matrix[0 * 4 + 2] * x + modelview_projection_matrix.matrix[1 * 4 + 2] * y + modelview_projection_matrix.matrix[3 * 4 + 2],
			modelview_projection_matrix.matrix[0 * 4 + 3] * x + modelview_projection_matrix.matrix[1 * 4 + 3] * y + modelview_projection_matrix.matrix[3 * 4 + 3]);
	}

	void RenderBatchPoint::set_batcher_active(const std::shared_ptr<Canvas> &canvas, int num_vertices)
	{
		if (position + num_vertices > max_vertices)
			static_cast<CanvasImpl*>(canvas.get())->batcher.flush();

		if (num_vertices > max_vertices)
			throw Exception("Too many vertices for RenderBatchPoint");

		static_cast<CanvasImpl*>(canvas.get())->set_batcher(this);
	}

	void RenderBatchPoint::flush(const std::shared_ptr<GraphicContext> &gc)
	{
		if (position > 0)
		{
			gc->set_program_object(program_color_only);

			int gpu_index;
			VertexArrayVector<PointVertex> gpu_vertices(batch_buffer->get_vertex_buffer(gc, gpu_index));

			if (!prim_array[gpu_index])
			{
				prim_array[gpu_index] = PrimitivesArray::create(gc);
				prim_array[gpu_index]->set_attributes(0, gpu_vertices, cl_offsetof(PointVertex, position));
				prim_array[gpu_index]->set_attributes(1, gpu_vertices, cl_offsetof(PointVertex, color));
			}

			gpu_vertices.upload_data(gc, 0, vertices, position);

			gc->draw_primitives(type_points, position, prim_array[gpu_index]);

			gc->reset_program_object();

			position = 0;
		}
	}

	void RenderBatchPoint::matrix_changed(const Mat4f &new_modelview, const Mat4f &new_projection, TextureImageYAxis image_yaxis, float pixel_ratio)
	{
		modelview_projection_matrix = new_projection * new_modelview;
	}
}
