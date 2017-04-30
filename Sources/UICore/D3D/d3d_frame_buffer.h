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
*/

#pragma once

#include "UICore/Display/Render/frame_buffer.h"
#include "UICore/Display/Render/render_buffer.h"
#include "UICore/Display/Render/texture_2d.h"
#include "d3d_render_buffer.h"
#include "d3d_texture_object.h"

namespace uicore
{
	class D3DFrameBuffer : public FrameBuffer
	{
	public:
		D3DFrameBuffer(const ComPtr<ID3D11Device> &device);
		~D3DFrameBuffer();

		Size size() const override;
		FrameBufferBindTarget bind_target() const override;

		ComPtr<ID3D11Device> &get_device() { return device; }
		std::vector<ID3D11RenderTargetView*> get_views(ID3D11DepthStencilView *&out_dsv);

		void attach_color(int attachment_index, const std::shared_ptr<RenderBuffer> &render_buffer) override;
		void attach_color(int attachment_index, const std::shared_ptr<Texture1D> &texture, int level) override;
		void attach_color(int attachment_index, const std::shared_ptr<Texture1DArray> &texture, int array_index, int level) override;
		void attach_color(int attachment_index, const std::shared_ptr<Texture2D> &texture, int level) override;
		void attach_color(int attachment_index, const std::shared_ptr<Texture2DArray> &texture, int array_index, int level) override;
		void attach_color(int attachment_index, const std::shared_ptr<Texture3D> &texture, int depth, int level) override;
		void attach_color(int attachment_index, const std::shared_ptr<TextureCube> &texture, TextureSubtype subtype, int level) override;
		void detach_color(int attachment_index) override;

		void attach_stencil(const std::shared_ptr<RenderBuffer> &render_buffer) override;
		void attach_stencil(const std::shared_ptr<Texture2D> &texture, int level) override;
		void attach_stencil(const std::shared_ptr<TextureCube> &texture, TextureSubtype subtype, int level) override;
		void detach_stencil() override;

		void attach_depth(const std::shared_ptr<RenderBuffer> &render_buffer) override;
		void attach_depth(const std::shared_ptr<Texture2D> &texture, int level) override;
		void attach_depth(const std::shared_ptr<TextureCube> &texture, TextureSubtype subtype, int level) override;
		void detach_depth() override;

		void attach_depth_stencil(const std::shared_ptr<RenderBuffer> &render_buffer) override;
		void attach_depth_stencil(const std::shared_ptr<Texture2D> &texture, int level) override;
		void attach_depth_stencil(const std::shared_ptr<TextureCube> &texture, TextureSubtype subtype, int level) override;
		void detach_depth_stencil() override;

		void set_bind_target(FrameBufferBindTarget target) override;

	private:
		struct AttachedBuffer
		{
			AttachedBuffer() : level(0), slice(0), subtype() { }
			AttachedBuffer(std::shared_ptr<RenderBuffer> render_buffer) : render_buffer(render_buffer), level(0), slice(0), subtype() { }
			AttachedBuffer(std::shared_ptr<Texture> texture, int level = 0, int slice = 0, TextureSubtype subtype = TextureSubtype()) : texture(texture), level(level), slice(slice), subtype(subtype) { }

			D3DRenderBuffer *get_render_buffer_provider() const { return static_cast<D3DRenderBuffer*>(render_buffer.get()); }
			D3DTextureObject *get_texture_provider() const { return static_cast<D3DTextureObject*>(texture->texture_object()); }

			std::shared_ptr<RenderBuffer> render_buffer;
			std::shared_ptr<Texture> texture;
			int level;
			int slice;
			TextureSubtype subtype;

			ComPtr<ID3D11RenderTargetView> rtv;
			ComPtr<ID3D11DepthStencilView> dsv;
		};

		ComPtr<ID3D11Device> device;
		std::vector<AttachedBuffer> color_buffers;
		AttachedBuffer depth_buffer;
		AttachedBuffer stencil_buffer;
		FrameBufferBindTarget _bind_target;
	};
}
