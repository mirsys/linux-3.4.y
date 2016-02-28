#
# Copyright (C) 2012, 2015 ARM Limited. All rights reserved.
# 
# This program is free software and is provided to you under the terms of the GNU General Public License version 2
# as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
# 
# A copy of the licence is included with the program, and can also be obtained from Free Software
# 
#

TARGET			= mali_drm
obj-$(CONFIG_DRM_MALI)	:= $(TARGET).o

######################################################################
# Source Cods
$(TARGET)-objs	+=

# Driver Main
$(TARGET)-objs	+= mali/mali_drv.o
$(TARGET)-objs	+= mali/mali_mm.o

######################################################################

EXTRA_CFLAGS += -I./include

######################################################################
