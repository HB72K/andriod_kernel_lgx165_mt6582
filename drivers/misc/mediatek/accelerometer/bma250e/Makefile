include $(srctree)/drivers/misc/mediatek/Makefile.custom

ifeq ($(findstring "e12",$(CONFIG_ARCH_MTK_PROJECT)),"e12")
obj-y	:=  bma250e.o
else
obj-y	:=  bma250.o
endif

