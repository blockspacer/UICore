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
#include "UICore/Display/display_target.h"
#include "UICore/GL/opengl_context_description.h"
#include "UICore/GL/opengl_target.h"
#include "UICore/Core/Text/text.h"
#include "opengl_graphic_context_provider.h"
#include "opengl_target_provider.h"
#include "GL3/gl3_graphic_context.h"
#include "setup_gl_impl.h"
#include "setup_gl.h"

namespace uicore
{
	namespace
	{
		OpenGLContextDescription context_description;
	}

	bool OpenGLTarget::is_current()
	{
		return std::dynamic_pointer_cast<OpenGLTargetProvider>(DisplayTarget::current_target()) ? true : false;
	}

	OpenGLContextDescription OpenGLTarget::description()
	{
		SetupGL::start();
		return context_description;
	}

	void OpenGLTarget::set_description(OpenGLContextDescription &desc)
	{
		SetupGL::start();
		context_description = desc;
	}

	void OpenGLTarget::set_current()
	{
		SetupGL::start();
		if (!SetupGL_Impl::instance)
			throw Exception("uicoreGL has not been initialised");

		static std::shared_ptr<OpenGLTargetProvider> provider = std::make_shared<OpenGLTargetProvider>();
		DisplayTarget::set_current_target(provider);
	}

	void OpenGLTarget::opengl_version(const std::shared_ptr<GraphicContext> &gc, int &version_major, int &version_minor)
	{
		const OpenGLContextProvider *provider = dynamic_cast<const OpenGLContextProvider *>(gc.get());
		if (provider == nullptr)
			throw Exception("Graphic Context is not from a GL target");
		provider->get_opengl_version(version_major, version_minor);
	}

	void OpenGLTarget::opengl_version(const std::shared_ptr<GraphicContext> &gc, int &version_major, int &version_minor, int &version_release)
	{
		const OpenGLContextProvider *provider = dynamic_cast<const OpenGLContextProvider *>(gc.get());
		if (provider == nullptr)
			throw Exception("Graphic Context is not from a GL target");
		provider->get_opengl_version(version_major, version_minor, version_release);
	}

	void OpenGLTarget::opengl_shading_language_version(const std::shared_ptr<GraphicContext> &gc, int &version_major, int &version_minor)
	{
		const OpenGLContextProvider *provider = dynamic_cast<const OpenGLContextProvider *>(gc.get());
		if (provider == nullptr)
			throw Exception("Graphic Context is not from a GL target");
		provider->get_opengl_shading_language_version(version_major, version_minor);
	}

	std::string OpenGLTarget::renderer_string(const std::shared_ptr<GraphicContext> &gc)
	{
		set_active_context(gc);
		std::string renderer = (char*)glGetString(GL_RENDERER);
		return renderer;
	}

	std::string OpenGLTarget::vendor_string(const std::shared_ptr<GraphicContext> &gc)
	{
		set_active_context(gc);
		std::string vendor = (char*)glGetString(GL_VENDOR);
		return vendor;
	}

	std::vector<std::string> OpenGLTarget::extensions(const std::shared_ptr<GraphicContext> &gc)
	{
		set_active_context(gc);
		std::string extension_string = (char*)glGetString(GL_EXTENSIONS);
		std::vector<std::string> tmp = Text::split(extension_string, " ");
		std::vector<std::string> extensions;
		for (auto & elem : tmp)
			extensions.push_back(elem);
		return extensions;
	}

	void OpenGLTarget::set_active_context(const std::shared_ptr<GraphicContext> &gc)
	{
		const OpenGLContextProvider *provider = dynamic_cast<const OpenGLContextProvider *>(gc.get());
		if (provider == nullptr)
			throw Exception("Graphic Context is not from a GL target");
		OpenGL::set_active(provider);
	}
}
