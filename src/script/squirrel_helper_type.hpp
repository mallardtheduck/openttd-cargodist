/* $Id$ */

/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file squirrel_helper_type.hpp Helper structs for converting Squirrel data structures to C++. */

#ifndef SQUIRREL_HELPER_TYPE_HPP
#define SQUIRREL_HELPER_TYPE_HPP

struct Array {
	int32 size;
	int32 array[];
};

#endif /* SQUIRREL_HELPER_TYPE_HPP */
