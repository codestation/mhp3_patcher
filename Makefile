TARGET = mhfupatcher
OBJS = main.o
LIBS =
BUILD_PRX=1
PSP_FW_VERSION = 500

CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)
ENCRYPT=1
EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = "Parche MHP3rd v1.4"
PSP_EBOOT_ICON= "patcher-logo2.png"
#PSP_EBOOT_UNKPNG= "patcher-fondo.png"

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
