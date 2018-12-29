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

#pragma once

#include "UICore/Display/Window/display_window_provider.h"
#include "UICore/Display/Render/graphic_context.h"
#include "UICore/Display/Window/input_device.h"
#include "UICore/Display/Platform/Win32/win32_window.h"
#include <memory>
#include "UICore/GL/opengl_context_description.h"
#include "UICore/GL/opengl_wrap.h"

namespace uicore
{
	typedef BOOL(APIENTRY *ptr_wglSwapIntervalEXT)(int interval);

	class OpenGLContextDescription;

	class OpenGLWindowProvider : public DisplayWindowProvider
	{
	public:
		OpenGLWindowProvider(OpenGLContextDescription &opengl_desc, const DisplayWindowDescription &description);
		~OpenGLWindowProvider();

		bool is_double_buffered() const { return double_buffered; }
		Rect backing_geometry() const;
		Rect backing_viewport() const;
		bool is_fullscreen() const;
		bool has_focus() const;
		bool is_minimized() const;
		bool is_maximized() const;
		bool is_visible() const;
		std::string title() const;
		Size backing_minimum_size(bool client_area) const;
		Size backing_maximum_size(bool client_area) const;
		DisplayWindowHandle handle() const override { DisplayWindowHandle handle; handle.hwnd = win32_window.get_hwnd(); return handle; }
		HDC get_device_context() const { return device_context; }
		HGLRC get_opengl_context() const { return opengl_context; }
		const std::shared_ptr<GraphicContext> &gc() const { return _gc; }
		const std::shared_ptr<InputDevice> &keyboard() const override { return win32_window.get_keyboard(); }
		const std::shared_ptr<InputDevice> &mouse() const override { return win32_window.get_mouse(); }
		const std::vector<std::shared_ptr<InputDevice>> &game_controllers() const override { return win32_window.get_game_controllers(); }
		bool is_clipboard_text_available() const;
		bool is_clipboard_image_available() const;
		std::string clipboard_text() const;
		std::shared_ptr<PixelBuffer> clipboard_image() const;
		float pixel_ratio() const override;

		void make_current() const;
		Point backing_client_to_screen(const Point &client);
		Point backing_screen_to_client(const Point &screen);

		HGLRC get_share_context();
		void show_system_cursor();
		std::shared_ptr<DisplayCursor> create_cursor(const CursorDescription &cursor_description);
		void set_cursor(const std::shared_ptr<DisplayCursor> &cursor);
		void set_cursor(StandardCursor type);
		void set_cursor_handle(HCURSOR cursor);
		void hide_system_cursor();
		void set_title(const std::string &new_title);
		void set_backing_position(const Rect &pos, bool client_area);
		void set_backing_size(int width, int height, bool client_area);
		void set_backing_minimum_size(int width, int height, bool client_area);
		void set_backing_maximum_size(int width, int height, bool client_area);
		void set_enabled(bool enable);
		void minimize();
		void restore();
		void maximize();
		void toggle_fullscreen();
		void show(bool activate);
		void hide();
		void bring_to_front();

		/// \brief Flip OpenGL buffers.
		void backing_flip(int interval);

		/// \brief Capture/Release the mouse.
		void capture_mouse(bool capture);

		/// \brief Stores text in the clipboard.
		void set_clipboard_text(const std::string &text);

		void set_clipboard_image(const std::shared_ptr<PixelBuffer> &buf);

		/// \brief Invalidates a region of a screen, causing a repaint.
		void request_repaint();

		void set_large_icon(const std::shared_ptr<PixelBuffer> &image);
		void set_small_icon(const std::shared_ptr<PixelBuffer> &image);

		void backing_enable_alpha_channel(const Rect &blur_rect);
		void backing_extend_frame_into_client_area(int left, int top, int right, int bottom);

		ProcAddress *get_proc_address(const std::string& function_name) const;

	private:
		static BOOL CALLBACK enum_windows_callback_save(HWND hwnd, LPARAM lParam);
		static BOOL CALLBACK enum_windows_callback_restore(HWND hwnd, LPARAM lParam);
		void create_shadow_window(HWND wnd);
		void on_window_resized();
		void get_opengl_version(int &version_major, int &version_minor);

		std::shared_ptr<GraphicContext> _gc;
		Win32Window win32_window;

		/// \brief OpenGL rendering context for this window.
		HGLRC opengl_context = 0;

		/// \brief Device context for this window.
		HDC device_context = 0;
		HWND shadow_hwnd = 0;
		bool shadow_window = false;
		bool dwm_layered = false;
		bool fullscreen = false;

		ptr_wglSwapIntervalEXT wglSwapIntervalEXT = nullptr;
		int swap_interval = -1;

		OpenGLContextDescription opengl_desc;

		bool using_gl3 = true;
		bool double_buffered = true;
	};
}
