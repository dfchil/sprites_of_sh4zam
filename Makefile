TARGETNAME = sha4zamsprites
BUILDDIR=build
OBJS := $(shell find . -name '*.c' -not -path "./.git/*" |sed -e 's,\.\(.*\).c,$(BUILDDIR)\1.o,g')

KOS_CSTD := -std=c23
CC=kos-cc



DTTEXTURES:=$(shell find assets/textures -name '*.png'| sed -e 's,assets/textures/\(.*\)/\([a-z_A-Z0-9]*\).png,$(BUILDDIR)/pvrtex/\1/\2.dt,g')


LDLIBS 	:= -lm -lm -lkosutils


INCLUDES= -I$(KOS_BASE)/include \
		-I$(KOS_BASE)/kernel/arch/dreamcast/include \
		-I$(KOS_BASE)/addons/include \
		-I$(KOS_BASE)/../kos-ports/include \
		-I$(KOS_BASE)/utils \
		-I$(shell pwd)/include \
		-I$(shell pwd)/build	 \


KOS_CFLAGS+=\
		$(KOS_CSTD) \
		$(INCLUDES) \
		-D__DREAMCAST__  \
		-D_arch_dreamcast -D_arch_sub_pristine \
		-fstack-protector-all \
		-flto=auto \
		-DFRAME_POINTERS \
		-Wall -Wextra  \
		-fno-strict-aliasing \
		-fomit-frame-pointer \
		-fbuiltin -ffast-math -ffp-contract=fast \
		-ml \
		-matomic-model=soft-imask \
		-ffunction-sections -fdata-sections -ftls-model=local-exec \
		-m4-single-only 

all: rm-elf ${TARGETNAME}.elf

include $(KOS_BASE)/Makefile.rules

clean:
	-rm -f ${TARGETNAME}.elf ${TARGETNAME}.cdi $(OBJS) $(DTTEXTURES)

rm-elf:
	-rm -f ${TARGETNAME}.elf


$(BUILDDIR)/%.o: %.c Makefile ${LHEADERS} $(DTTEXTURES)
	@mkdir -p $(shell dirname $@)
	$(CC) $(KOS_CFLAGS) $(LDLIBS) -c $< -o $@


${TARGETNAME}.elf: $(OBJS) 
	$(CC) $(KOS_CFLAGS) -o $@  $(LDLIBS) $(OBJS) 

TEXDIR_PAL4=$(BUILDDIR)/pvrtex/pal4
$(TEXDIR_PAL4):
	mkdir -p $@
$(TEXDIR_PAL4)/%.dt: assets/textures/pal4/%.png $(TEXDIR_PAL4)
	pvrtex -f PAL4BPP -c --max-color 16 -i $< -o $@

TEXDIR_PAL8=$(BUILDDIR)/pvrtex/pal8
$(TEXDIR_PAL8):
	mkdir -p $@
$(TEXDIR_PAL8)/%.dt: assets/textures/pal8/%.png $(TEXDIR_PAL8)
	pvrtex -f PAL8BPP -c --max-color 256 -i $< -o $@

TEXDIR_RGB565_VQ_TW=$(BUILDDIR)/pvrtex/rgb565_vq_tw
$(TEXDIR_RGB565_VQ_TW):
	mkdir -p $@
$(TEXDIR_RGB565_VQ_TW)/%.dt: assets/textures/rgb565_vq_tw/%.png $(TEXDIR_RGB565_VQ_TW)
	pvrtex -f RGB565 -c -i $< -o $@

TEXDIR_ARGB1555_VQ_TW=$(BUILDDIR)/pvrtex/argb1555_vq_tw
$(TEXDIR_ARGB1555_VQ_TW):
	mkdir -p $@
$(TEXDIR_ARGB1555_VQ_TW)/%.dt: assets/textures/argb1555_vq_tw/%.png $(TEXDIR_ARGB1555_VQ_TW)
	pvrtex -f ARGB1555 -c -i $< -o $@


run: $(TARGET)
	$(KOS_LOADER) $(TARGET)

dist:
	rm -f $(OBJS)
	$(KOS_STRIP) $(TARGET)
