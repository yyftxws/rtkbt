ifeq ($(BOARD_HAVE_BLUETOOTH_RTK),true)
LOCAL_PATH := $(call my-dir)
# Common macros inherited by sub-makefiles
rtk_local_C_INCLUDES := \
        $(LOCAL_PATH)/include \
        $(LOCAL_PATH)/../bta/hh \
        $(LOCAL_PATH)/../bta/include \
        $(LOCAL_PATH)/../bta/sys \
        $(LOCAL_PATH)/../bta/dm \
        $(LOCAL_PATH)/../gki/common \
        $(LOCAL_PATH)/../gki/ulinux \
        $(LOCAL_PATH)/../include \
        $(LOCAL_PATH)/../stack/include \
        $(LOCAL_PATH)/../stack/l2cap \
        $(LOCAL_PATH)/../stack/a2dp \
        $(LOCAL_PATH)/../stack/btm \
        $(LOCAL_PATH)/../stack/avdt \
        $(LOCAL_PATH)/../hcis \
        $(LOCAL_PATH)/../hcis/include \
        $(LOCAL_PATH)/../hcis/patchram \
        $(LOCAL_PATH)/../udrv_rtk/include \
        $(LOCAL_PATH)/../btif_rtk/include \
        $(LOCAL_PATH)/../btif_rtk/co \
        $(LOCAL_PATH)/../hci/include\
        $(LOCAL_PATH)/../brcm/include \
        $(LOCAL_PATH)/../embdrv_rtk/sbc/encoder/include \
        $(LOCAL_PATH)/../utils/include \
        external/tinyxml2 \
        $(bdroid_C_INCLUDES)

rtk_local_CFLAGS += -DBUILDCFG $(bdroid_CFLAGS)  -Wno-error=maybe-uninitialized -Wno-error=uninitialized

include $(call all-subdir-makefiles)
endif
