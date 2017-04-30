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

#include <climits>
#include <vector>
#include "UICore/Display/2D/canvas.h"
#include "UICore/Display/2D/path.h"
#include "UICore/Display/2D/brush.h"
#include "UICore/Display/Render/blend_state_description.h"
#include "UICore/Display/Render/shader_object.h"
#include "UICore/Display/Render/texture_2d.h"
#include "UICore/Display/Render/staging_texture.h"
#include "UICore/Display/Image/pixel_buffer.h"
#include "UICore/Display/Render/program_object.h"
#include "render_batch_buffer.h"
#include "path_renderer.h"

namespace uicore
{
	enum class PathFillMode;
	class Brush;
	class PathRasterRange;
	class PathMaskBuffer;

	class PathScanlineEdge
	{
	public:
		PathScanlineEdge() { }
		PathScanlineEdge(float x, bool up_direction) : x(x), up_direction(up_direction) { }

		float x = 0.0f;
		bool up_direction = false;
	};

	class PathScanline
	{
	public:
		std::vector<PathScanlineEdge> edges;
		std::vector<unsigned char> pixels;

		void insert_sorted(PathScanlineEdge edge)
		{
			edges.push_back(edge);

			for (size_t pos = edges.size() - 1; pos > 0 && edges[pos - 1].x >= edge.x; pos--)
			{
				PathScanlineEdge temp = edges[pos - 1];
				edges[pos - 1] = edges[pos];
				edges[pos] = temp;
			}
		}
	};

	class PathInstanceBuffer
	{
	public:
		void reset(const std::shared_ptr<GraphicContext> &gc, Vec4f *buffer, int max_entries);
		int push(const std::shared_ptr<Canvas> &canvas, const Brush &brush, const Mat4f &transform);

		Vec4f *get_buffer() const { return buffer; }
		int get_position() const { return end_position; }

		std::shared_ptr<Texture2D> get_texture() const { return current_texture; }

	private:
		static Pointf transform_point(Pointf point, const Mat3f &brush_transform, const Mat4f &fill_transform);

		int next_position(int size);

		int store_solid(const std::shared_ptr<Canvas> &canvas, const Brush &brush, const Mat4f &transform);
		int store_linear(const std::shared_ptr<Canvas> &canvas, const Brush &brush, const Mat4f &transform);
		int store_radial(const std::shared_ptr<Canvas> &canvas, const Brush &brush, const Mat4f &transform);
		int store_image(const std::shared_ptr<Canvas> &canvas, const Brush &brush, const Mat4f &transform);

		Vec4f *buffer = nullptr;
		int max_entries = 0;
		int end_position = 0;		// The next free position

		std::shared_ptr<Texture2D> current_texture;
	};

	class PathVertexBuffer
	{
	public:
		void reset(Vec4i *vertices, int max_vertices);
		bool is_full() const;
		void push(int x, int y, int instance_offset, int mask_offset);

		Vec4i *get_vertices() const { return vertices; }
		int get_position() const { return position; }

	private:
		Vec4i *vertices = nullptr;
		int max_vertices = 0;
		int position = 0;
	};

	enum class PathShaderDrawMode : int
	{
		solid = 0,
		linear = 1,
		radial = 2,
		image = 3
	};

	namespace PathConstants
	{
		static const int antialias_level = 2;
		static const int mask_block_size = 16;		// *** If changing this, remember to modify the path shaders ***
		static const int scanline_block_size = mask_block_size * antialias_level;
		static const int mask_texture_size = RenderBatchBuffer::r8_size;
		static const int max_blocks = (mask_texture_size / mask_block_size) * (mask_texture_size / mask_block_size);
		static const int instance_buffer_width = RenderBatchBuffer::rgba32f_width;   // In rgbaf blocks
		static const int instance_buffer_height = RenderBatchBuffer::rgba32f_height; // In rgbaf blocks
	};

	class PathRasterRange
	{
	public:
		void begin(const PathScanline *scanline, PathFillMode mode);
		void next();

		bool found = false;
		int x0;
		int x1;

	private:
		const PathScanline *scanline = nullptr;
		PathFillMode mode;
		size_t i = 0;
		int nonzero_rule = 0;
	};

	class PathMaskBuffer
	{
	public:
		PathMaskBuffer();
		~PathMaskBuffer();
		PathMaskBuffer(const PathMaskBuffer &) = delete;
		PathMaskBuffer &operator=(const PathMaskBuffer&) = delete;

		bool is_full() const;

		void reset(unsigned char *mask_buffer_data, int mask_buffer_pitch);
		void flush_block();

		void begin_row(PathScanline *scanlines, PathFillMode mode);
		bool fill_block(int xpos);

		int block_index = 0;
		int next_block = 0;

	private:
		bool is_full_block(int xpos) const;
		void fill_full_block();

		PathRasterRange range[PathConstants::scanline_block_size];

		unsigned char *mask_buffer_data = nullptr;
		int mask_buffer_pitch = 0;

		unsigned char *mask_row_block_data = nullptr;

		bool found_filled_block = false;
		int filled_block_index = 0;
	};

	class PathFillRenderer : public PathRenderer
	{
	public:
		PathFillRenderer(const std::shared_ptr<GraphicContext> &gc, RenderBatchBuffer *batch_buffer);

		void clear(int width, int height);

		void line(float x, float y) override;
		void end(bool close) override;

		void fill(const std::shared_ptr<Canvas> &canvas, PathFillMode mode, const Brush &brush, const Mat4f &transform);
		void flush(const std::shared_ptr<GraphicContext> &gc);

		void set_yaxis(TextureImageYAxis yaxis) { image_yaxis = yaxis; }

		const float rcp_mask_texture_size = 1.0f / (float)PathConstants::mask_texture_size;

	private:
		void insert_sorted(PathScanline &scanline, const PathScanlineEdge &edge);

		void initialise_buffers(const std::shared_ptr<Canvas> &canvas);

		TextureImageYAxis image_yaxis = y_axis_top_down;

		struct Extent
		{
			Extent() : left(INT_MAX), right(0){}
			int left;
			int right;
		};

		Extent find_extent(const PathScanline *scanline, int max_width);

		int first_scanline = 0;
		int last_scanline = 0;

		int width = 0;
		int height = 0;
		std::vector<PathScanline> scanlines;

		class Block
		{
		public:
			Block(const Point &output_position, int mask_index) : output_position(output_position), mask_index(mask_index) {}
			Point output_position;
			int mask_index;
		};
		enum { max_vertices = RenderBatchBuffer::vertex_buffer_size / sizeof(Vec4i) };

		PathInstanceBuffer instances;
		PathVertexBuffer vertices;
		PathMaskBuffer mask_blocks;

		int current_instance_offset = 0;

		RenderBatchBuffer *batch_buffer;

		std::shared_ptr<StagingTexture> mask_buffer;
		int mask_buffer_id;	// Buffer index of the mask buffer
		std::shared_ptr<Texture2D> mask_texture;
		std::shared_ptr<StagingTexture> instance_buffer;
		std::shared_ptr<Texture2D> instance_texture;
		std::shared_ptr<PrimitivesArray> prim_array[RenderBatchBuffer::num_vertex_buffers];
		std::shared_ptr<BlendState> blend_state;
	};
}
