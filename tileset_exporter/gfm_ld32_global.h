/**
 * GFM (proprietary) tiled plugin
 * 
 * Based on the "CSV Tiled Plugin"
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GFM_LD32_GLOBAL_H
#define GFM_LD32_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GFM_LD32_LIBRARY)
#  define GFMLD32SHARED_EXPORT Q_DECL_EXPORT
#else
#  define GFMLD32SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // GFM_LD32_GLOBAL_H

