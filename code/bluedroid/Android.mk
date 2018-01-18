LOCAL_PATH := $(call my-dir)


# RealTek Bluetooth private configuration table
ifeq ($(BOARD_HAVE_BLUETOOTH_RTK),true)
rtkbt_bdroid_C_INCLUDES += $(LOCAL_PATH)/realtek/include
rtkbt_bdroid_C_INCLUDES += $(LOCAL_PATH)/bta/hh
rtkbt_bdroid_C_INCLUDES += $(LOCAL_PATH)/bta/dm
ifeq ($(BOARD_HAVE_BLUETOOTH_RTK_COEX),true)
rtkbt_bdroid_CFLAGS += -DBLUETOOTH_RTK_COEX
endif
rtkbt_bdroid_CFLAGS += -DBLUETOOTH_RTK_API
rtkbt_bdroid_CFLAGS += -DBLUETOOTH_RTK

ifeq ($(PLATFORM_VERSION), 4.3)
rtkbt_bdroid_CFLAGS += -DANDROID_43
endif

endif

ifeq ($(BOARD_HAVE_BLUETOOTH_RTK),true)
	bdroid_C_INCLUDES := $(rtkbt_bdroid_C_INCLUDES)
	bdroid_CFLAGS += -DHAS_BDROID_BUILDCFG  $(rtkbt_bdroid_CFLAGS)
else
# Setup bdroid local make variables for handling configuration
ifneq ($(BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR),)
  bdroid_C_INCLUDES := $(BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR)
  bdroid_CFLAGS := -DHAS_BDROID_BUILDCFG $(rtkbt_bdroid_CFLAGS)
else
  bdroid_C_INCLUDES :=
  bdroid_CFLAGS := -DHAS_NO_BDROID_BUILDCFG $(rtkbt_bdroid_CFLAGS)
endif
endif
include $(call all-subdir-makefiles)

# Cleanup our locals
bdroid_C_INCLUDES :=
bdroid_CFLAGS :=
