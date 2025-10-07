#include <errno.h>
#include <sh4zamsprites/tex_loader.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sh4zam/shz_sh4zam.h>
#include <malloc.h>


int pvrtex_load_blob(const void* data, dttex_info_t* texinfo) {
    memcpy(&texinfo->hdr, data, sizeof(dt_header_t));
    if (texinfo->hdr.fourcc[0] != 'D' || texinfo->hdr.fourcc[1] != 'c' ||
        texinfo->hdr.fourcc[2] != 'T' || texinfo->hdr.fourcc[3] != 'x') {
        printf("Error: not valid DcTx data\n");
        return 0;
    }
    size_t tdatasize =
        texinfo->hdr.chunk_size - ((1 + texinfo->hdr.header_size) << 5);

    texinfo->flags.compressed = fDtIsCompressed(&texinfo->hdr);
    texinfo->flags.mipmapped = fDtIsMipmapped(&texinfo->hdr);
    texinfo->flags.palettised = fDtIsPalettized(&texinfo->hdr);
    texinfo->flags.num_palette_colors = fDtGetColorsUsed(&texinfo->hdr);

    if (texinfo->flags.palettised) {
        texinfo->flags.palette_format = texinfo->flags.num_palette_colors == 16
                                            ? PVR_PAL_ARGB4444
                                            : PVR_PAL_ARGB8888;
    } else {
        texinfo->flags.palette_format = 0;
    }

    texinfo->flags.strided = fDtIsStrided(&texinfo->hdr);
    texinfo->flags.twiddled = fDtIsTwiddled(&texinfo->hdr);
    texinfo->width = fDtGetPvrWidth(&texinfo->hdr);
    texinfo->height = fDtGetPvrHeight(&texinfo->hdr);

    texinfo->pvrformat = texinfo->hdr.pvr_type & 0xFFC00000;

    texinfo->ptr = pvr_mem_malloc(tdatasize);
    if (texinfo->ptr == NULL) {
        printf("Error: pvr_mem_malloc failed\n");
        return 0;
    }
    pvr_txr_load(data + sizeof(dt_header_t), texinfo->ptr, tdatasize);
    return 1;
}

int pvrtex_load_file(const char* filename, dttex_info_t* texinfo) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Error opening file %s: %s\n", filename, strerror(errno));
        return 0;
    }

    if (fread(texinfo, sizeof(dt_header_t), 1, file) != 1) {
        printf("Error reading header from file %s\n", filename);
        fclose(file);
        return 0;
    }

    if (texinfo->hdr.fourcc[0] != 'D' || texinfo->hdr.fourcc[1] != 'c' ||
        texinfo->hdr.fourcc[2] != 'T' || texinfo->hdr.fourcc[3] != 'x') {
        printf("Error: not valid DcTx data in file %s\n", filename);
        fclose(file);
        return 0;
    }

    size_t tdatasize =
        texinfo->hdr.chunk_size - ((1 + texinfo->hdr.header_size) << 5);

    void* buffer = memalign(32, tdatasize + sizeof(dt_header_t));
    if (!buffer) {
        printf("Error allocating memory for texture data from file %s\n",
               filename);
        fclose(file);
        return 0;
    }
    memcpy(buffer, texinfo, sizeof(dt_header_t));
    if (fread(buffer + sizeof(dt_header_t), tdatasize , 1, file) != 1) {
        printf("Error reading texture data from file %s\n", filename);
        free(buffer);
        fclose(file);
        return 0;
    }
    fclose(file);
    int result = pvrtex_load_blob(buffer, texinfo);
    free(buffer);
    return result;
}

int pvrtex_load_palette_blob(const void* raw_data, int fmt, size_t offset) {
    struct {
        char fourcc[4];
        size_t colors;
    } palette_hdr;
    memcpy(&palette_hdr, raw_data, sizeof(palette_hdr));
    uint32_t* colors = (uint32_t*)((char*)raw_data + sizeof(palette_hdr));

    pvr_set_pal_format(fmt);
    for (size_t i = 0; i < palette_hdr.colors; i++) {
        uint32_t color = colors[i];  // format 0xAARRGGBB
        switch (fmt) {
            case PVR_PAL_ARGB8888:
                break;
            case PVR_PAL_ARGB4444:
                color =
                    ((color & 0xF0000000) >> 16 | (color & 0x00F00000) >> 12) |
                    ((color & 0x0000F000) >> 8) | ((color & 0x000000F0) >> 4);
                break;
            case PVR_PAL_RGB565:
                color = ((color & 0x00F80000) >> 8) |
                        ((color & 0x0000FC00) >> 5) |
                        ((color & 0x000000F8) >> 3);
                break;
            case PVR_PAL_ARGB1555:
                color =
                    ((color & 0x80000000) >> 16) | ((color & 0x00F80000) >> 9) |
                    ((color & 0x0000F800) >> 6) | ((color & 0x000000F8) >> 3);
                break;
            default:
                break;
        }
        pvr_set_pal_entry(i + offset, color);
    }
    return 1;
}

int pvrtex_load_palette_file(const char* filename, int fmt, size_t offset) {
    int success = 1;
    FILE* file = NULL;
    void* raw_data = NULL;
    do {
        file = fopen(filename, "rb");
        if (!file) {
            printf("Error opening palette file %s: %s\n", filename,
                   strerror(errno));
            success = 0;
            break;
        }
        struct {
            char fourcc[4];
            size_t colors;
        } palette_hdr;
        if (fread(&palette_hdr, sizeof(palette_hdr), 1, file) != 1) {
            printf("Error reading palette header from file %s\n", filename);
            success = 0;
            break;
        }
        void* raw_data =
            memalign(32, palette_hdr.colors * sizeof(uint32_t) + sizeof(palette_hdr));
        if (!raw_data) {
            printf("Error allocating memory for palette colors from file %s\n",
                   filename);
            success = 0;
            break;
        }
        memcpy(raw_data, &palette_hdr, sizeof(palette_hdr));
        if (fread((char*)raw_data + sizeof(palette_hdr),
                  palette_hdr.colors * sizeof(uint32_t), 1, file) != 1) {
            printf("Error reading palette colors from file %s\n", filename);
            free(raw_data);
        }
        fclose(file);
        success = pvrtex_load_palette_blob(raw_data, fmt, offset);
    } while (0);

    if (file != NULL) {
        fclose(file);
    }
    if (raw_data != NULL) {
        free(raw_data);
    }
    return success;
}

int pvrtex_unload(dttex_info_t* texinfo) {
    if (texinfo->ptr != NULL) {
        pvr_mem_free(texinfo->ptr);
        texinfo->ptr = NULL;
        return 1;
    }
    return 0;
}
