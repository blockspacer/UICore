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

#include "UICore/Display/Render/staging_texture.h"
#include "UICore/Core/System/comptr.h"
#include "d3d_share_list.h"

namespace uicore
{
	class D3DGraphicContext;

	class D3DStagingTexture : public StagingTexture, D3DSharedResource
	{
	public:
		D3DStagingTexture(const ComPtr<ID3D11Device> &device, const void *data, const Size &new_size, StagingDirection direction, TextureFormat new_format, BufferUsage usage);
		~D3DStagingTexture();

		void *data() override;
		const void *data() const override;
		int pitch() const override;

		ComPtr<ID3D11Texture2D> &get_texture_2d(const ComPtr<ID3D11Device> &device);

		int width() const override { return _size.width; }
		int height() const override { return _size.height; }

		TextureFormat format() const override { return texture_format; };

		float pixel_ratio() const override { return _pixel_ratio; }
		void set_pixel_ratio(float ratio) override { _pixel_ratio = ratio; }

		void lock(const std::shared_ptr<GraphicContext> &gc, BufferAccess access) override;
		void unlock() override;
		void upload_data(const std::shared_ptr<GraphicContext> &gc, const Rect &dest_rect, const void *data) override;

	private:
		struct DeviceHandles
		{
			DeviceHandles(const ComPtr<ID3D11Device> &device) : device(device) { }

			ComPtr<ID3D11Device> device;
			ComPtr<ID3D11Texture2D> texture;
		};

		void device_destroyed(ID3D11Device *device);
		DeviceHandles &get_handles(const ComPtr<ID3D11Device> &device);

		static D3D11_MAP to_d3d_map_type(BufferAccess access);
		static UINT to_d3d_cpu_access(StagingDirection direction);

		std::vector<std::shared_ptr<DeviceHandles> > handles;
		D3D11_MAPPED_SUBRESOURCE map_data;
		D3DGraphicContext *map_gc_provider = nullptr;

		Size _size;
		TextureFormat texture_format;
		bool data_locked = false;	// lock() has been called
		float _pixel_ratio = 0.0f;
	};
}
