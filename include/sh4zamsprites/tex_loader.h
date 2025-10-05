#ifndef RENDER_PVR_TEXTURE_TEX_LOADER_H
#define RENDER_PVR_TEXTURE_TEX_LOADER_H

#include <dc/pvr.h>
#include <pvrtex/file_dctex.h>
#include <stdint.h>

typedef fDtHeader dt_header_t;

typedef struct {
  dt_header_t hdr;
  uint32_t pvrformat;
  union flags {
    uint32_t raw;
    struct {
      uint32_t palettised : 1;
      uint32_t twiddled : 1;
      uint32_t compressed : 1;
      uint32_t strided : 1;
      uint32_t mipmapped : 1;
      uint32_t loaded : 1;
      uint32_t palette_position : 10;
      uint32_t palette_format : 2;
      uint32_t num_palette_colors : 9;
      uint32_t : 5;
    };
  } flags;
  uint16_t width;
  uint16_t height;
  pvr_ptr_t ptr;
} dttex_info_t;


/**
 * @brief Load a texture from a file
 * @param filename The name of the file to load
 * @param texinfo The texture info structure to fill
 * @return int 1 on success, 0 on failure
 */
int pvrtex_load_file(const char *filename, dttex_info_t *texinfo);

/**
 * @brief Load a palette from a file
 * @param raw_data The raw data of the palette
 * @param fmt The format of the palette
 * @param offset The offset to load the palette
 * @return int 1 on success, 0 on failure
 * @note Valid format defines are:

 * - PVR_PAL_ARGB8888: 32-bit ARGB

 * - PVR_PAL_ARGB4444: 16-bit ARGB

 * - PVR_PAL_RGB565: 16-bit RGB

 * - PVR_PAL_ARGB1555: 16-bit ARGB
 */
int pvrtex_load_palette_blob(void* raw_data, int fmt, size_t offset);


/**
 * @brief Load a palette from a file
 * @param filename The name of the file to load
 * @param fmt The format of the palette
 * @param offset The offset to load the palette
 * @return int 1 on success, 0 on failure
 * @note Valid format defines are:

 * - PVR_PAL_ARGB8888: 32-bit ARGB

 * - PVR_PAL_ARGB4444: 16-bit ARGB

 * - PVR_PAL_RGB565: 16-bit RGB

 * - PVR_PAL_ARGB1555: 16-bit ARGB
 */
int pvrtex_load_palette_file(const char *filename, int fmt, size_t offset);

/**
 * @brief Load a texture from a file
 *
 * @param texinfo The texture info structure, followed in memory by the texture data
 * @return int 1 on success, 0 on failure
 */
int pvrtex_load_blob(dttex_info_t* texinfo);

/**
 * @brief Unload a texture from memory
 * @param texinfo The texture texinfo struct
 * @return int 1 on success, 0 on failure
 */
int pvrtex_unload(dttex_info_t *texinfo);

#endif // RENDER_PVR_TEXTURE_TEX_LOADER_H
