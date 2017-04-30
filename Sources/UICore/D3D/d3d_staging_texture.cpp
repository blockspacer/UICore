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

#include "UICore/precomp.h"
#include "d3d_staging_texture.h"
#include "d3d_texture_object.h"
#include "d3d_graphic_context.h"
#include "d3d_display_window.h"
#include "UICore/D3D/d3d_target.h"

namespace uicore
{
	D3DStagingTexture::D3DStagingTexture(const ComPtr<ID3D11Device> &device, const void *data, const Size &new_size, StagingDirection direction, TextureFormat format, BufferUsage usage)
	{
		handles.push_back(std::shared_ptr<DeviceHandles>(new DeviceHandles(device)));
		map_data.pData = 0;
		map_data.RowPitch = 0;
		map_data.DepthPitch = 0;

		data_locked = false;
		texture_format = format;
		_size = new_size;
		handles.resize(1);
		handles.front()->texture.clear();

		D3D11_TEXTURE2D_DESC texture_desc;
		texture_desc.Width = _size.width;
		texture_desc.Height = _size.height;
		texture_desc.MipLevels = 1;
		texture_desc.ArraySize = 1;
		texture_desc.Format = D3DTextureObject::to_d3d_format(format);
		texture_desc.SampleDesc.Count = 1;
		texture_desc.SampleDesc.Quality = 0;
		texture_desc.Usage = D3D11_USAGE_STAGING;
		texture_desc.BindFlags = 0;
		texture_desc.CPUAccessFlags = to_d3d_cpu_access(direction);
		texture_desc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;

		if ((usage == usage_stream_draw) && (direction == StagingDirection::to_gpu)) // To do: find a buffer usage API that works well for both Direct3D and OpenGL
		{
			texture_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			texture_desc.Usage = D3D11_USAGE_DYNAMIC;
			texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			texture_desc.MiscFlags = 0;
		}

		HRESULT result = handles.front()->device->CreateTexture2D(&texture_desc, 0, handles.front()->texture.output_variable());
		D3DTarget::throw_if_failed("ID3D11Device.CreateTexture2D failed", result);
	}

	D3DStagingTexture::~D3DStagingTexture()
	{
	}

	void *D3DStagingTexture::data()
	{
		if (!data_locked)
			throw Exception("lock() not called before get_data()");

		return map_data.pData;
	}

	const void *D3DStagingTexture::data() const
	{
		if (!data_locked)
			throw Exception("lock() not called before get_data()");

		return map_data.pData;
	}

	ComPtr<ID3D11Texture2D> &D3DStagingTexture::get_texture_2d(const ComPtr<ID3D11Device> &device)
	{
		if (device)
			return get_handles(device).texture;
		else
			return handles.front()->texture;
	}

	int D3DStagingTexture::pitch() const
	{
		if (!data_locked)
			throw Exception("lock() not called before get_pitch()");

		return map_data.RowPitch;
	}

	void D3DStagingTexture::lock(const std::shared_ptr<GraphicContext> &gc, BufferAccess access)
	{
		map_gc_provider = static_cast<D3DGraphicContext *>(gc.get());
		DeviceHandles &handle = get_handles(map_gc_provider->get_window()->get_device());

		HRESULT result = map_gc_provider->get_window()->get_device_context()->Map(handle.texture, 0, to_d3d_map_type(access), 0, &map_data);

		D3DTarget::throw_if_failed("ID3D11DeviceContext.Map failed", result);
		data_locked = true;
	}

	void D3DStagingTexture::unlock()
	{
		map_gc_provider->get_window()->get_device_context()->Unmap(get_handles(map_gc_provider->get_window()->get_device()).texture, 0);
		map_gc_provider = 0;
		map_data.pData = 0;
		map_data.RowPitch = 0;
		map_data.DepthPitch = 0;
		data_locked = false;
	}

	void D3DStagingTexture::upload_data(const std::shared_ptr<GraphicContext> &gc, const Rect &dest_rect, const void *data)
	{
		D3DGraphicContext *gc_provider = static_cast<D3DGraphicContext *>(gc.get());
		DeviceHandles &handle = get_handles(gc_provider->get_window()->get_device());

		D3D11_BOX box;
		box.left = dest_rect.left;
		box.top = dest_rect.top;
		box.right = dest_rect.right;
		box.bottom = dest_rect.bottom;
		box.front = 0;
		box.back = 1;

		D3D11_TEXTURE2D_DESC texture_desc;
		handle.texture->GetDesc(&texture_desc);
		int pitch = D3DTextureObject::get_bytes_per_pixel(texture_desc.Format) * texture_desc.Width;

		gc_provider->get_window()->get_device_context()->UpdateSubresource(handle.texture, 0, &box, data, pitch, pitch * dest_rect.height());
	}

	void D3DStagingTexture::device_destroyed(ID3D11Device *device)
	{
		for (size_t i = 0; i < handles.size(); i++)
		{
			if (handles[i]->device.get() == device)
			{
				handles.erase(handles.begin() + i);
				return;
			}
		}
	}

	D3DStagingTexture::DeviceHandles &D3DStagingTexture::get_handles(const ComPtr<ID3D11Device> &device)
	{
		for (size_t i = 0; i < handles.size(); i++)
			if (handles[i]->device == device)
				return *handles[i];

		ComPtr<IDXGIResource> resource;
		HRESULT result = handles.front()->texture->QueryInterface(__uuidof(IDXGIResource), (void**)resource.output_variable());
		D3DTarget::throw_if_failed("ID3D11Texture2D.QueryInterface(IDXGIResource) failed", result);

		HANDLE handle = 0;
		result = resource->GetSharedHandle(&handle);
		D3DTarget::throw_if_failed("IDXGIResource.GetSharedHandle failed", result);

		ComPtr<ID3D11Texture2D> texture_handle;
		result = device->OpenSharedResource(handle, __uuidof(ID3D11Texture2D), (void**)texture_handle.output_variable());
		D3DTarget::throw_if_failed("ID3D11Device.OpenSharedResource failed", result);

		handles.push_back(std::shared_ptr<DeviceHandles>(new DeviceHandles(device)));
		handles.back()->texture = texture_handle;
		return *handles.back();
	}

	D3D11_MAP D3DStagingTexture::to_d3d_map_type(BufferAccess access)
	{
		switch (access)
		{
		case access_read_only:
			return D3D11_MAP_READ;
		case access_write_only:
			return D3D11_MAP_WRITE;
		case access_write_discard:
			return D3D11_MAP_WRITE_DISCARD;
		case access_read_write:
			return D3D11_MAP_READ_WRITE;
		}
		throw Exception("Unsupported access type");
	}

	UINT D3DStagingTexture::to_d3d_cpu_access(StagingDirection direction)
	{
		switch (direction)
		{
		case StagingDirection::to_gpu:
			return D3D11_CPU_ACCESS_WRITE;
		case StagingDirection::from_gpu:
			return D3D11_CPU_ACCESS_READ;
		}
		throw Exception("Unsupported pixel buffer direction");
	}
}
