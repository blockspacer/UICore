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
#include "UICore/UI/Controller/window_manager.h"
#include "UICore/UI/TopLevel/top_level_window.h"
#include "UICore/UI/UIThread/ui_thread.h"
#include "UICore/Display/Window/display_window_description.h"
#include "UICore/Display/Window/display_window.h"
#include "UICore/Display/2D/canvas.h"
#include "UICore/Display/System/run_loop.h"
#include "UICore/Display/Image/pixel_buffer.h"
#include "UICore/Display/ImageFormats/image_file.h"
#include "UICore/Core/IOData/path_help.h"
#include <map>

namespace uicore
{
#if 0
	class WindowManagerImpl
	{
	public:
		bool exit_on_last_close = true;
		std::map<WindowController *, std::shared_ptr<WindowController>> windows;

		static WindowManagerImpl *instance()
		{
			static WindowManagerImpl impl;
			return &impl;
		}

		static Sizef auto_content_size(const std::shared_ptr<View> &root_view, const std::shared_ptr<Canvas> &canvas)
		{
			auto css_width = root_view->style_cascade().computed_value("width");
			auto css_height = root_view->style_cascade().computed_value("height");
			auto css_min_width = root_view->style_cascade().computed_value("min-width");
			auto css_min_height = root_view->style_cascade().computed_value("min-height");
			auto css_max_width = root_view->style_cascade().computed_value("max-width");
			auto css_max_height = root_view->style_cascade().computed_value("max-height");

			float width = 0.0f;
			if (css_width.is_keyword("auto"))
				width = root_view->preferred_width(canvas);
			else
				width = css_width.number();

			if (css_min_width.is_length())
				width = std::max(width, css_min_width.number());
			if (css_max_width.is_length())
				width = std::min(width, css_max_width.number());

			float height = 0.0f;
			if (css_height.is_keyword("auto"))
				height = root_view->preferred_height(canvas, width);
			else
				height = css_height.number();

			if (css_min_height.is_length())
				height = std::max(height, css_min_height.number());
			if (css_max_height.is_length())
				height = std::min(height, css_max_height.number());

			return Sizef(width, height);
		}
	};

	/////////////////////////////////////////////////////////////////////////

	class WindowControllerImpl
	{
	public:
		std::string title;
		Sizef initial_size;
		bool frame_geometry = true;
		bool resizable = true;
		std::vector<std::string> icon_images;
		std::shared_ptr<View> root_view = std::make_shared<View>();

		std::shared_ptr<TopLevelWindow> window;
		std::weak_ptr<View> modal_owner;

		SlotContainer slots;
	};

	/////////////////////////////////////////////////////////////////////////

	void WindowManager::set_exit_on_last_close(bool enable)
	{
		WindowManagerImpl::instance()->exit_on_last_close = enable;
	}

	void WindowManager::present_main(const std::shared_ptr<WindowController> &controller)
	{
		DisplayWindowDescription desc;
		desc.set_main_window();
		desc.set_visible(false);
		desc.set_title(controller->title());
		desc.set_allow_resize(controller->impl->resizable);

		if (controller->impl->initial_size != Sizef())
			desc.set_size(controller->impl->initial_size, !controller->impl->frame_geometry);

		controller->impl->window = std::make_shared<TopLevelWindow>(desc);
		controller->impl->window->set_root_view(controller->root_view());

		std::shared_ptr<DisplayWindow> display_window = controller->impl->window->display_window();
		controller->impl->slots.connect(display_window->sig_window_close(), bind_member(controller.get(), &WindowController::dismiss));

		WindowManagerImpl::instance()->windows[controller.get()] = controller;

		if (controller->impl->initial_size == Sizef())
		{
			std::shared_ptr<Canvas> canvas = controller->root_view()->canvas();
			Sizef content_size = WindowManagerImpl::auto_content_size(controller->root_view(), canvas);
			Rectf content_box(content_size);
			Rectf margin_box = ViewGeometry::from_content_box(controller->root_view()->style_cascade(), content_box).margin_box();
			display_window->set_size(margin_box.width(), margin_box.height(), true);
		}

		if (!controller->impl->icon_images.empty())
		{
			display_window->set_large_icon(ImageFile::load(FilePath::combine(UIThread::resource_path(), controller->impl->icon_images.front())));
			display_window->set_small_icon(ImageFile::load(FilePath::combine(UIThread::resource_path(), controller->impl->icon_images.back())));
		}

		controller->impl->window->show(WindowShowType::show);
	}

	void WindowManager::present_modal(View *owner, const std::shared_ptr<WindowController> &controller)
	{
		Pointf screen_pos = owner->to_screen_pos(owner->geometry().content_box().center());

		DisplayWindowDescription desc;
		desc.set_dialog_window();
		desc.set_visible(false);

		std::shared_ptr<DisplayWindow> owner_display_window = owner->view_tree()->display_window();
		if (owner_display_window)
			desc.set_owner_window(owner_display_window);
		desc.set_title(controller->title());
		desc.show_minimize_button(false);
		desc.show_maximize_button(false);
		desc.set_allow_resize(controller->impl->resizable);

		if (controller->impl->initial_size != Sizef())
			desc.set_size(controller->impl->initial_size, !controller->impl->frame_geometry);

		controller->impl->modal_owner = owner->shared_from_this();
		controller->impl->window = std::make_shared<TopLevelWindow>(desc);
		controller->impl->window->set_root_view(controller->root_view());

		std::shared_ptr<DisplayWindow> controller_display_window = controller->impl->window->display_window();
		if (controller_display_window)
			controller->impl->slots.connect(controller_display_window->sig_window_close(), bind_member(controller.get(), &WindowController::dismiss));

		WindowManagerImpl::instance()->windows[controller.get()] = controller;

		if (controller->impl->initial_size == Sizef())
		{
			std::shared_ptr<Canvas> canvas = controller->root_view()->canvas();
			Sizef content_size = WindowManagerImpl::auto_content_size(controller->root_view(), canvas);
			Rectf content_box(screen_pos.x - content_size.width * 0.5f, screen_pos.y - content_size.height * 0.5f, screen_pos.x + content_size.width * 0.5f, screen_pos.y + content_size.height * 0.5f);
			Rectf margin_box = ViewGeometry::from_content_box(controller->root_view()->style_cascade(), content_box).margin_box();
			if (controller_display_window)
				controller_display_window->set_position(margin_box, true);
		}

		if (!controller->impl->icon_images.empty())
		{
			controller_display_window->set_large_icon(ImageFile::load(FilePath::combine(UIThread::resource_path(), controller->impl->icon_images.front())));
			controller_display_window->set_small_icon(ImageFile::load(FilePath::combine(UIThread::resource_path(), controller->impl->icon_images.back())));
		}

		controller->impl->window->show(WindowShowType::show);
		if (owner_display_window)
			owner_display_window->set_enabled(false);
	}

	void WindowManager::present_popup(View *owner, const Pointf &pos, const std::shared_ptr<WindowController> &controller)
	{
		Pointf screen_pos = owner->to_screen_pos(pos);

		DisplayWindowDescription desc;
		desc.set_popup_window();
		desc.set_visible(false);
		desc.set_topmost(true);
		desc.set_no_activate(true);
		desc.show_caption(false);
		desc.show_sysmenu(false);
		desc.show_minimize_button(false);
		desc.show_maximize_button(false);
		desc.set_allow_resize(false);

		if (controller->impl->initial_size != Sizef())
			desc.set_size(controller->impl->initial_size, !controller->impl->frame_geometry);

		controller->impl->window = std::make_shared<TopLevelWindow>(desc);
		controller->impl->window->set_root_view(controller->root_view());

		std::shared_ptr<DisplayWindow> owner_display_window = owner->view_tree()->display_window();
		if (owner_display_window)
			controller->impl->slots.connect(owner_display_window->sig_lost_focus(), bind_member(controller.get(), &WindowController::dismiss));

		WindowManagerImpl::instance()->windows[controller.get()] = controller;

		if (controller->impl->initial_size == Sizef())
		{
			std::shared_ptr<Canvas> canvas = controller->root_view()->canvas();
			Sizef content_size = WindowManagerImpl::auto_content_size(controller->root_view(), canvas);

			Rectf content_box(screen_pos, content_size);
			Rectf margin_box = ViewGeometry::from_content_box(controller->root_view()->style_cascade(), content_box).margin_box();

			std::shared_ptr<DisplayWindow> controller_display_window = controller->impl->window->display_window();
			if (controller_display_window)
				controller_display_window->set_position(margin_box, false);
		}

		controller->impl->window->show(WindowShowType::show_no_activate);
	}

	/////////////////////////////////////////////////////////////////////////

	WindowController::WindowController() : impl(new WindowControllerImpl)
	{
	}

	WindowController::~WindowController()
	{
	}

	const std::shared_ptr<View> &WindowController::root_view() const
	{
		return impl->root_view;
	}

	void WindowController::set_root_view(std::shared_ptr<View> root_view)
	{
		impl->root_view = root_view;
	}

	const std::string &WindowController::title() const
	{
		return impl->title;
	}

	void WindowController::set_title(const std::string &title)
	{
		impl->title = title;
		if (impl->window)
		{
			std::shared_ptr<DisplayWindow> display_window = impl->window->display_window();
			if (display_window)
				display_window->set_title(title);
		}
	}

	void WindowController::set_frame_size(const Sizef &size, bool resizable)
	{
		impl->initial_size = size;
		impl->frame_geometry = true;
		impl->resizable = resizable;
		if (impl->window)
		{
			std::shared_ptr<DisplayWindow> display_window = impl->window->display_window();
			if (display_window)
				display_window->set_size(size.width, size.height, false);
		}
	}

	void WindowController::set_content_size(const Sizef &size, bool resizable)
	{
		impl->initial_size = size;
		impl->frame_geometry = false;
		impl->resizable = resizable;
		if (impl->window)
		{
			std::shared_ptr<DisplayWindow> display_window = impl->window->display_window();
			if (display_window)
				display_window->set_size(size.width, size.height, true);
		}
	}

	void WindowController::set_icon(const std::vector<std::string> &icon_images)
	{
		impl->icon_images = icon_images;
		if (impl->window)
		{
			std::shared_ptr<DisplayWindow> display_window = impl->window->display_window();
			if (display_window && !icon_images.empty())
			{
				display_window->set_large_icon(ImageFile::load(FilePath::combine(UIThread::resource_path(), icon_images.front())));
				display_window->set_small_icon(ImageFile::load(FilePath::combine(UIThread::resource_path(), icon_images.back())));
			}
		}
	}

	void WindowController::dismiss()
	{
		auto modal_owner = impl->modal_owner.lock();
		if (modal_owner && modal_owner->view_tree())
		{
			std::shared_ptr<DisplayWindow> display_window = modal_owner->view_tree()->display_window();
			if (display_window)
			{
				display_window->set_enabled(true);
				if (impl->window->display_window()->has_focus())
					display_window->show(true); // activate parent to workaround bug in Windows in some situations
			}
		}

		// Reset fields before erase because 'this' might be destroyed if 'windows' had the last reference
		impl->window.reset();
		impl->modal_owner.reset();

		auto &windows = WindowManagerImpl::instance()->windows;
		auto it = windows.find(this);
		if (it != windows.end())
			windows.erase(it);

		if (WindowManagerImpl::instance()->exit_on_last_close && windows.empty())
			RunLoop::exit();
	}
#endif
}
