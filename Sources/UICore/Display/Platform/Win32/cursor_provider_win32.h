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

#include "UICore/Display/Window/cursor.h"
#include "UICore/Core/System/databuffer.h"
#include <vector>

namespace uicore
{
	class PixelBuffer;
	class Point;
	class Rect;
	class CursorDescription;

	class CursorProvider_Win32 : public Cursor
	{
	public:
		CursorProvider_Win32(const CursorDescription &cursor_description);
		~CursorProvider_Win32();

		HCURSOR handle;

	private:
		static HCURSOR create_cursor(const CursorDescription &cursor_description);
		static std::shared_ptr<DataBuffer> create_ico_file(const std::shared_ptr<PixelBuffer> &image);
		static std::shared_ptr<DataBuffer> create_cur_file(const std::shared_ptr<PixelBuffer> &image, const Rect &rect, const Point &hotspot);
		static std::shared_ptr<DataBuffer> create_ani_file(const CursorDescription &cursor_description);
		static std::shared_ptr<DataBuffer> create_ico_helper(const std::shared_ptr<PixelBuffer> &image, const Rect &rect, WORD type, const Point &hotspot);
		static std::shared_ptr<DataBuffer> create_ico_helper(const std::vector<std::shared_ptr<PixelBuffer>> &images, const std::vector<Rect> &rect, WORD type, const std::vector<Point> &hotspots);
		static void set_riff_header(char *data, const char *type, DWORD size);

		struct ANIHeader
		{
			DWORD cbSizeOf;              // Number of bytes in AniHeader (36 bytes)
			DWORD cFrames;               // Number of unique Icons in this cursor
			DWORD cSteps;                // Number of Blits before the animation cycles
			DWORD cx, cy;                // reserved, must be zero.
			DWORD cBitCount, cPlanes;    // reserved, must be zero.
			DWORD JifRate;               // Default Jiffies (1/60th of a second) if rate chunk not present.
			DWORD flags;                 // Animation Flag (see AF_ constants)
		};

		#define AF_ICON 0x0001L // Windows format icon/cursor animation

		struct ICONHEADER
		{
			WORD idReserved;
			WORD idType; // 1 = ICO, 2 = CUR
			WORD idCount;
		};

		struct IconDirectoryEntry
		{
			BYTE bWidth;
			BYTE bHeight;
			BYTE bColorCount;
			BYTE bReserved;
			union
			{
				WORD wPlanes;	// ICO format
				SHORT XHotspot; // CUR format
			};
			union
			{
				WORD wBitCount; // ICO format
				SHORT YHotspot; // CUR format
			};
			DWORD dwBytesInRes;
			DWORD dwImageOffset;
			/** WORD nID; // Mentioned by http://msdn2.microsoft.com/en-us/library/ms997538.aspx but not in other ICO docs.*/
		};

		struct ANIInfo
		{
			int length() const
			{
				// todo: dword align string lengths
				return 5 * 4 + title.length() + 1 + author.length() + 1;
			}

			void write(char *d)
			{
				memcpy(d, "INFOINAM", 8);
				*(DWORD *)(d + 8) = title.length() + 1;
				memcpy(d + 12, title.c_str(), title.length() + 1);
				int offset_art = 12 + title.length() + 1;
				memcpy(d + offset_art, "IART", 4);
				*(DWORD *)(d + offset_art + 4) = author.length() + 1;
				memcpy(d + offset_art + 8, author.c_str(), author.length() + 1);
			}

			std::string title;
			std::string author;
		};

		struct ANIFrames
		{
			int length() const
			{
				int s = 4 + 8 * icons.size();
				for (unsigned int i = 0; i < icons.size(); i++)
					s += icons[i]->size();
				return s;
			}

			void write(char *d)
			{
				memcpy(d, "fram", 4);
				int p = 4;
				for (unsigned int i = 0; i < icons.size(); i++)
				{
					memcpy(d + p, "icon", 4);
					*(DWORD *)(d + p + 4) = icons[i]->size();
					memcpy(d + p + 8, icons[i]->data(), icons[i]->size());
					p += 8 + icons[i]->size();
				}
			}

			std::vector<std::shared_ptr<DataBuffer>> icons;
		};

		enum { size_header = 6, size_direntry = 16 };
	};
}
