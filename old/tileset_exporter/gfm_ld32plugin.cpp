/**
 * GFM_ld32 (proprietary) tiled plugin
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

#include "gfm_ld32plugin.h"

#include "map.h"
#include "mapobject.h"
#include "objectgroup.h"
#include "tile.h"
#include "tilelayer.h"

#include <QFile>

#if QT_VERSION >= 0x050100
#define HAS_QSAVEFILE_SUPPORT
#endif

#ifdef HAS_QSAVEFILE_SUPPORT
#include <QSaveFile>
#endif

using namespace Tiled;
using namespace Gfm_ld32;

#ifdef HAS_QSAVEFILE_SUPPORT
static void writeTilemap(QSaveFile &file, QSaveFile &headerFile, const TileLayer *tileLayer);
static void writeWalls(QSaveFile &file, QSaveFile &headerFile, const ObjectGroup *objs);
static void writeStones(QSaveFile &file, QSaveFile &headerFile, const ObjectGroup *objs);
static void writeSpikes(QSaveFile &file, QSaveFile &headerFile, const ObjectGroup *objs);
#else
static void writeTilemap(QFile &file, QFile &headerFile, const TileLayer *tileLayer);
static void writeWalls(QFile &file, QFile &headerFile, const ObjectGroup *objs);
static void writeStones(QFile &file, QFile &headerFile, const ObjectGroup *objs);
static void writeSpikes(QFile &file, QFile &headerFile, const ObjectGroup *objs);
#endif

Gfm_ld32Plugin::Gfm_ld32Plugin()
{
}

bool Gfm_ld32Plugin::write(const Map *map, const QString &fileName)
{
    int foundTileLayer;
    QString headerName = QString(fileName);
    headerName.remove(headerName.length()-1, 1);
    headerName.append("h");
#ifdef HAS_QSAVEFILE_SUPPORT
    QSaveFile file(fileName);
    QSaveFile headerFile(headerName);
#else
    QFile file(fileName);
    QFile headerFile(headerName);
#endif
    
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        mError = tr("Could not open file for writing.");
        return false;
    }
    if (!headerFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        mError = tr("Could not open file for writing.");
        return false;
    }
    
    headerFile.write("#include <GFraMe/GFraMe_error.h>\n");
    headerFile.write("#include <GFraMe/GFraMe_object.h>\n\n");
    headerFile.write("#include \"sprite.h\"\n\n");
    
    file.write("#include <GFraMe/GFraMe_error.h>\n");
    file.write("#include <GFraMe/GFraMe_hitbox.h>\n");
    file.write("#include <GFraMe/GFraMe_object.h>\n\n");
    file.write("#include <stdlib.h>\n");
    file.write("#include <string.h>\n\n");
    file.write("#include \"global.h\"\n");
    file.write("#include \"sprite.h\"\n\n");
    
    foundTileLayer = 0;
    
    // Write every layer
    foreach (const Layer *layer, map->layers()) {
        if (!layer->isVisible())
            continue;
        
        // Write a tilemap
        if (layer->layerType() == Layer::TileLayerType) {
            const TileLayer *tileLayer;
            
            if (foundTileLayer == 1) {
                mError = tr("Found more than one tilemap!");
                return false;
            }
            
            foundTileLayer = 1;
            
            tileLayer = static_cast<const TileLayer*>(layer);
            
            writeTilemap(file, headerFile, tileLayer);
        }
        else if (layer->layerType() == Layer::ObjectGroupType) {
            const ObjectGroup *objectGroup;
            
            objectGroup = static_cast<const ObjectGroup*>(layer);
            
            if (objectGroup->name() == "walls") {
                writeWalls(file, headerFile, objectGroup);
            }
            else if (objectGroup->name() == "stones") {
                writeStones(file, headerFile, objectGroup);
            }
            else if (objectGroup->name() == "spikes") {
                writeSpikes(file, headerFile, objectGroup);
            }
            else {
                mError = tr("Found a non-parsable object layer!");
                return false;
            }
        }
    }

    if (file.error() != QFile::NoError) {
        mError = file.errorString();
        return false;
    }
    if (headerFile.error() != QFile::NoError) {
        mError = file.errorString();
        return false;
    }

#ifdef HAS_QSAVEFILE_SUPPORT
    if (!file.commit()) {
        mError = file.errorString();
        return false;
    }
    if (!headerFile.commit()) {
        mError = file.errorString();
        return false;
    }
#endif

    file.close();
    headerFile.close();
    return true;
}

QString Gfm_ld32Plugin::nameFilter() const
{
    return tr("GFM_LD32 parsed C files (*.c)");
}

QString Gfm_ld32Plugin::errorString() const
{
    return mError;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(Gfm, Gfm_ld32Plugin)
#endif

#define getInt(var) \
    QByteArray::number((int)(var))

#ifdef HAS_QSAVEFILE_SUPPORT
static void writeTilemap(QSaveFile &file, QSaveFile &headerFile, const TileLayer *tileLayer) {
#else
static void writeTilemap(QFile &file, QFile &headerFile, const TileLayer *tileLayer) {
#endif
    QStringList list = file.fileName().split("/");
    QString name = list.at(list.size()-1);
    name.remove(name.length() -2, 2);
    
    // Create the tilemap's header
    headerFile.write("/** Generated tilemap */\nextern char ");
    headerFile.write(name.toLatin1());
    headerFile.write("_tilemap[];\n");
    headerFile.write("/** Generated tilemap's width */\nextern int ");
    headerFile.write(name.toLatin1());
    headerFile.write("_width;\n/** Generated tilemap's height */\nextern int ");
        headerFile.write(name.toLatin1());
    headerFile.write("_height;\n");
    
    // Create the buffer
    file.write("/** Generated tilemap */\n");
    file.write("char ");
    file.write(name.toLatin1());
    file.write("_tilemap[] = \n");
    file.write("{\n");
    // Write the buffer data
    for (int y = 0; y < tileLayer->height(); y++) {
        file.write("  ");
        for (int x = 0; x < tileLayer->width(); x++) {
            const Cell &cell = tileLayer->cellAt(x, y);
            const Tile *tile = cell.tile;
            const int id = tile ? tile->id() : -1;
            file.write(QByteArray::number(id));
            file.write(",", 1);
        }
        
        file.write("\n", 1);
    }
    // Close the buffer
    file.write("};\n\n");
    
    // Write the tilemap's dimensions
    file.write("/** Generated tilemap's width */\n");
    file.write("int ");
    file.write(name.toLatin1());
    file.write("_width = ");
    file.write(QByteArray::number(tileLayer->width()));
    file.write(";\n");
    file.write("/** Generated tilemap's height */\n");
    file.write("int ");
    file.write(name.toLatin1());
    file.write("_height = ");
    file.write(QByteArray::number(tileLayer->height()));
    file.write(";\n\n");
}

#ifdef HAS_QSAVEFILE_SUPPORT
static void writeWalls(QSaveFile &file, QSaveFile &headerFile, const ObjectGroup *objs) {
#else
static void writeWalls(QFile &file, QFile &headerFile, const ObjectGroup *objs) {
#endif
    int len = objs->objects().length();
    int i;
    
    QStringList list = file.fileName().split("/");
    QString name = list.at(list.size()-1);
    name.remove(name.length() -2, 2);

    headerFile.write("/** Get all this map's walls into a GFraMe_object buffer */\n");
    headerFile.write("int ");
    headerFile.write(name.toLatin1());
    headerFile.write("_getWalls(GFraMe_object **ppObjs, int *pLen, int *pUsed);\n");
    
    file.write("/** Get all this map's walls into a GFraMe_object buffer */\n");
    file.write("int ");
    file.write(name.toLatin1());
    file.write("_getWalls(GFraMe_object **ppObjs, int *pLen, int *pUsed) {\n");
    file.write("    int len;\n");
    file.write("    int i;\n    \n");
    
    file.write("    len = "); file.write(getInt(len)); file.write(";\n    \n");
    
    file.write("    if (!ppObjs)\n        return 1;\n");
    file.write("    if (!pLen)\n        return 1;\n");
    file.write("    if (!pUsed)\n        return 1;\n    \n");
    file.write("    if (*pLen < len) {\n");
    file.write("        *ppObjs = (GFraMe_object*)realloc(*ppObjs, sizeof(GFraMe_object)*len);\n");
    file.write("        if (!(*ppObjs))\n            return 1;\n");
    file.write("        *pLen = len;\n");
    file.write("    }\n");
    file.write("    *pUsed = len;\n    \n");
    
    file.write("    i = 0;\n");
    file.write("    while (i < *pLen)\n");
    file.write("        memset(&((*ppObjs)[i++]), 0, sizeof(GFraMe_object));\n    \n");
    
    // Write every object in this layer
    i = 0;
    file.write("    i = 0;\n");
    foreach (const MapObject *obj, objs->objects()) {
        if (!obj->isVisible())
            continue;
        
        file.write("    GFraMe_object_clear(&((*ppObjs)[i]));\n");
        file.write("    GFraMe_object_set_x(&((*ppObjs)[i]), ");file.write(getInt(obj->x()));file.write(");\n");
        file.write("    GFraMe_object_set_y(&((*ppObjs)[i]), ");file.write(getInt(obj->y()));file.write(");\n");
        file.write("    GFraMe_hitbox_set(&((*ppObjs)[i].hitbox), GFraMe_hitbox_upper_left, 0/*x*/, 0/*y*/, ");file.write(getInt(obj->width()));file.write(", ");file.write(getInt(obj->height()));file.write(");\n    \n");
        file.write("    i++;\n");
        
        i++;
    }
    
    file.write("    return 0;\n");
    file.write("}\n");
}

#ifdef HAS_QSAVEFILE_SUPPORT
static void writeStones(QSaveFile &file, QSaveFile &headerFile, const ObjectGroup *objs) {
#else
static void writeStones(QFile &file, QFile &headerFile, const ObjectGroup *objs) {
#endif
    int len = objs->objects().length();
    int i;
    
    QStringList list = file.fileName().split("/");
    QString name = list.at(list.size()-1);
    name.remove(name.length() -2, 2);

    headerFile.write("/** Get all this map's walls into a GFraMe_object buffer */\n");
    headerFile.write("int ");
    headerFile.write(name.toLatin1());
    headerFile.write("_getStones(sprite ***pppSprs, int *pLen, int *pUsed);\n");
    
    file.write("/** Get all this map's walls into a GFraMe_object buffer */\n");
    file.write("int ");
    file.write(name.toLatin1());
    file.write("_getStones(sprite ***pppSprs, int *pLen, int *pUsed) {\n");
    file.write("    int i, len, rv;\n");
    file.write("    sprite *pSpr;\n    \n");
    
    file.write("    len = "); file.write(getInt(len)); file.write(";\n    \n");
    
    file.write("    if (!pppSprs)\n        return 1;\n");
    file.write("    if (!pLen)\n        return 1;\n");
    file.write("    if (!pUsed)\n        return 1;\n    \n");
    file.write("    if (*pLen < len) {\n");
    file.write("        *pppSprs = (sprite**)realloc(*pppSprs, sizeof(sprite*)*len);\n");
    file.write("        if (!(*pppSprs))\n            return 1;\n");
    file.write("        *pLen = len;\n");
    file.write("    }\n");
    file.write("    *pUsed = len;\n    \n");
    
    file.write("    memset(*pppSprs, 0, sizeof(sprite*)*len);\n    \n");
    
    // Write every object in this layer
    i = 0;
    file.write("    i = 0;\n");
    foreach (const MapObject *obj, objs->objects()) {
        if (!obj->isVisible())
            continue;
        
        file.write("    pSpr = 0;\n");
        file.write("    rv = spr_getNew(&pSpr);\n");
        file.write("    ASSERT_NR(rv == 0);\n");
        file.write("    (*pppSprs)[i] = pSpr;\n");
        file.write("    rv = spr_init(pSpr,\n");
        file.write("             /*x*/"); file.write(getInt(obj->x())); file.write(",\n");
        file.write("             /*y*/"); file.write(getInt(obj->y())); file.write(",\n");
        file.write("          /*offX*/0,\n");
        file.write("          /*offY*/0,\n");
        file.write("         /*width*/8,\n");
        file.write("        /*height*/8,\n");
        file.write("   /*hitboxWidth*/"); file.write(getInt(obj->width())); file.write(",\n");
        file.write("  /*hitboxHeight*/"); file.write(getInt(obj->height())); file.write(",\n");
        if (obj->type() == "red") {
            file.write("      /*animData*/_sprRedStoneData,\n");
            file.write("       /*animLen*/_sprRedStoneAnimLen,\n");
            file.write("          /*type*/SPR_RED_STONE\n");
        }
        else if (obj->type() == "orange") {
            file.write("      /*animData*/_sprOrangeStoneData,\n");
            file.write("       /*animLen*/_sprOrangeStoneAnimLen,\n");
            file.write("          /*type*/SPR_ORANGE_STONE\n");
        }
        else if (obj->type() == "yellow") {
            file.write("      /*animData*/_sprYellowStoneData,\n");
            file.write("       /*animLen*/_sprYellowStoneAnimLen,\n");
            file.write("          /*type*/SPR_YELLOW_STONE\n");
        }
        else if (obj->type() == "green") {
            file.write("      /*animData*/_sprGreenStoneData,\n");
            file.write("       /*animLen*/_sprGreenStoneAnimLen,\n");
            file.write("          /*type*/SPR_GREEN_STONE\n");
        }
        else if (obj->type() == "cyan") {
            file.write("      /*animData*/_sprCyanStoneData,\n");
            file.write("       /*animLen*/_sprCyanStoneAnimLen,\n");
            file.write("          /*type*/SPR_CYAN_STONE\n");
        }
        else if (obj->type() == "blue") {
            file.write("      /*animData*/_sprBlueStoneData,\n");
            file.write("       /*animLen*/_sprBlueStoneAnimLen,\n");
            file.write("          /*type*/SPR_BLUE_STONE\n");
        }
        else if (obj->type() == "purple") {
            file.write("      /*animData*/_sprPurpleStoneData,\n");
            file.write("       /*animLen*/_sprPurpleStoneAnimLen,\n");
            file.write("          /*type*/SPR_PURPLE_STONE\n");
        }
        file.write("    );\n");
        file.write("    ASSERT_NR(rv == 0);\n");
        file.write("    i++;\n");
        
        i++;
    }
    
    file.write("    rv = 0;\n");
    file.write("__ret:\n");
    file.write("    return rv;\n");
    file.write("}\n");
}

#ifdef HAS_QSAVEFILE_SUPPORT
static void writeSpikes(QSaveFile &file, QSaveFile &headerFile, const ObjectGroup *objs) {
#else
static void writeSpikes(QFile &file, QFile &headerFile, const ObjectGroup *objs) {
#endif
    int len = objs->objects().length();
    int i;
    
    QStringList list = file.fileName().split("/");
    QString name = list.at(list.size()-1);
    name.remove(name.length() -2, 2);

    headerFile.write("/** Get all this map's walls into a GFraMe_object buffer */\n");
    headerFile.write("int ");
    headerFile.write(name.toLatin1());
    headerFile.write("_getSpikes(sprite ***pppSprs, int *pLen, int *pUsed);\n");
    
    file.write("/** Get all this map's walls into a GFraMe_object buffer */\n");
    file.write("int ");
    file.write(name.toLatin1());
    file.write("_getSpikes(sprite ***pppSprs, int *pLen, int *pUsed) {\n");
    file.write("    int i, len, rv;\n");
    file.write("    sprite *pSpr;\n    \n");
    
    file.write("    len = "); file.write(getInt(len)); file.write(";\n    \n");
    
    file.write("    if (!pppSprs)\n        return 1;\n");
    file.write("    if (!pLen)\n        return 1;\n");
    file.write("    if (!pUsed)\n        return 1;\n    \n");
    file.write("    if (*pLen < len) {\n");
    file.write("        *pppSprs = (sprite**)realloc(*pppSprs, sizeof(sprite*)*len);\n");
    file.write("        if (!(*pppSprs))\n            return 1;\n");
    file.write("        *pLen = len;\n");
    file.write("    }\n");
    file.write("    *pUsed = len;\n    \n");
    
    file.write("    memset(*pppSprs, 0, sizeof(sprite*)*len);\n    \n");
    
    // Write every object in this layer
    i = 0;
    file.write("    i = 0;\n");
    foreach (const MapObject *obj, objs->objects()) {
        if (!obj->isVisible())
            continue;
        
        file.write("    pSpr = 0;\n");
        file.write("    rv = spr_getNew(&pSpr);\n");
        file.write("    ASSERT_NR(rv == 0);\n");
        file.write("    (*pppSprs)[i] = pSpr;\n");
        file.write("    rv = spr_init(pSpr,\n");
        file.write("             /*x*/"); file.write(getInt(obj->x())); file.write(",\n");
        file.write("             /*y*/"); file.write(getInt(obj->y())); file.write(",\n");
        file.write("          /*offX*/0,\n");
        file.write("          /*offY*/0,\n");
        file.write("         /*width*/0,\n");
        file.write("        /*height*/0,\n");
        file.write("   /*hitboxWidth*/"); file.write(getInt(obj->width())); file.write(",\n");
        file.write("  /*hitboxHeight*/"); file.write(getInt(obj->height())); file.write(",\n");
        file.write("      /*animData*/0,\n");
        file.write("       /*animLen*/0,\n");
        file.write("          /*type*/SPR_SPIKE\n");
        file.write("    );\n");
        file.write("    ASSERT_NR(rv == 0);\n");
        file.write("    i++;\n");
        
        i++;
    }
    
    file.write("    rv = 0;\n");
    file.write("__ret:\n");
    file.write("    return rv;\n");
    file.write("}\n");
}

