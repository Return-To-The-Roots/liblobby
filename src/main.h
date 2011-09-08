// $Id: main.h 7521 2011-09-08 20:45:55Z FloSoft $
//
// Copyright (c) 2005 - 2011 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Return To The Roots.
//
// Return To The Roots is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Return To The Roots is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>.
#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#pragma once

///////////////////////////////////////////////////////////////////////////////
// System-Header

#define _CRTDBG_MAP_ALLOC
#define WIN32_LEAN_AND_MEAN

#ifdef _WIN32
	#include <windows.h>
	
	#ifndef __CYGWIN__
		#include <conio.h>
	#endif
	
	#include <ws2tcpip.h>

#ifdef _MSC_VER
	#include <crtdbg.h>
#else
    #include <assert.h>
#endif
#else
	#include <arpa/inet.h>
	#include <assert.h>
#endif // !_WIN32

#ifdef _WIN32

#ifdef _MSC_VER
	#define getch _getch
	#ifndef snprintf
		#define snprintf _snprintf
	#endif
	#define assert _ASSERT
#endif

#endif // _WIN32

#include <errno.h>

#include <cstdlib>
#include <cstdarg>
#include <cstring>

#include <map>
#include <string>
#include <vector>
#include <sstream>

///////////////////////////////////////////////////////////////////////////////
// Eigene Header
#include <build_paths.h>
#include "libutil.h"
#include "mygettext.h"
#include "liblobby.h"

const char *GetWindowTitle();
const char *GetWindowVersion();
const char *GetWindowRevision();

#endif // MAIN_H_INCLUDED
