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
#include "d3d_rasterizer_state.h"
#include "UICore/D3D/d3d_target.h"

namespace uicore
{
	D3DRasterizerState::D3DRasterizerState(const ComPtr<ID3D11Device> &device, const RasterizerStateDescription &desc)
	{
		float factor = 0.0f;
		float units = 0.0f;
		desc.polygon_offset(factor, units);

		D3D11_RASTERIZER_DESC d3d_desc;
		d3d_desc.FrontCounterClockwise = (desc.front_face() == face_counter_clockwise) ? TRUE : FALSE;
		d3d_desc.DepthBias = factor;
		d3d_desc.SlopeScaledDepthBias = units;
		d3d_desc.DepthBiasClamp = FLT_MAX;
		d3d_desc.DepthClipEnable = TRUE;
		d3d_desc.ScissorEnable = desc.enable_scissor() ? TRUE : FALSE;
		d3d_desc.MultisampleEnable = FALSE;
		d3d_desc.AntialiasedLineEnable = desc.enable_line_antialiasing() ? TRUE : FALSE;

		if (desc.culled())
		{
			switch (desc.face_cull_mode())
			{
			case cull_front: d3d_desc.CullMode = D3D11_CULL_FRONT; break;
			case cull_back: d3d_desc.CullMode = D3D11_CULL_BACK; break;
			case cull_front_and_back: d3d_desc.CullMode = D3D11_CULL_NONE; break;
			}
		}
		else
		{
			d3d_desc.CullMode = D3D11_CULL_NONE;
		}

		switch (desc.face_fill_mode())
		{
		case fill_point: throw Exception("Point fill mode not supported by D3D target");
		case fill_line: d3d_desc.FillMode = D3D11_FILL_WIREFRAME; break;
		case fill_polygon: d3d_desc.FillMode = D3D11_FILL_SOLID; break;
		}

		HRESULT result = device->CreateRasterizerState(&d3d_desc, state.output_variable());
		D3DTarget::throw_if_failed("D3D11Device.CreateRasterizerState failed", result);
	}
}
