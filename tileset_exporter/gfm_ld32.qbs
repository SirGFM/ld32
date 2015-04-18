import qbs 1.0

TiledPlugin {
    cpp.defines: ["GFM_LD32_LIBRARY"]

    files: [
        "gfm_ld32_global.h",
        "gfm_ld32plugin.cpp",
        "gfm_ld32plugin.h",
    ]
}
