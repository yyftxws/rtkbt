LOCAL_PATH:= $(call my-dir)

#
# Bluetooth HW module
#

include $(CLEAR_VARS)

# HAL layer
LOCAL_SRC_FILES:= \
	../btif_rtk/src/bluetooth.c

# platform specific
LOCAL_SRC_FILES+= \
	bte_main.c \
	bte_init.c \
	bte_version.c \
	bte_logmsg.c \
	bte_conf.c

# BTIF
LOCAL_SRC_FILES += \
    ../btif_rtk/src/btif_core.c \
    ../btif_rtk/src/btif_dm.c \
    ../btif_rtk/src/btif_storage.c \
    ../btif_rtk/src/btif_util.c \
    ../btif_rtk/src/btif_sm.c \
    ../btif_rtk/src/btif_hf.c \
    ../btif_rtk/src/btif_av.c \
    ../btif_rtk/src/btif_rc.c \
    ../btif_rtk/src/btif_media_task.c \
    ../btif_rtk/src/btif_hh.c \
    ../btif_rtk/src/btif_hl.c \
    ../btif_rtk/src/btif_sock.c \
    ../btif_rtk/src/btif_sock_rfc.c \
    ../btif_rtk/src/btif_sock_thread.c \
    ../btif_rtk/src/btif_sock_sdp.c \
    ../btif_rtk/src/btif_sock_util.c \
    ../btif_rtk/src/btif_pan.c \
    ../btif_rtk/src/btif_gatt.c \
    ../btif_rtk/src/btif_gatt_client.c \
    ../btif_rtk/src/btif_gatt_server.c \
    ../btif_rtk/src/btif_gatt_util.c \
    ../btif_rtk/src/btif_gatt_test.c \
    ../btif_rtk/src/btif_config.c \
    ../btif_rtk/src/btif_config_util.cpp \
    ../btif_rtk/src/btif_profile_queue.c

# callouts
LOCAL_SRC_FILES+= \
    ../btif_rtk/co/bta_sys_co.c \
    ../btif_rtk/co/bta_fs_co.c \
    ../btif_rtk/co/bta_ag_co.c \
    ../btif_rtk/co/bta_dm_co.c \
    ../btif_rtk/co/bta_av_co.c \
    ../btif_rtk/co/bta_hh_co.c \
    ../btif_rtk/co/bta_hl_co.c \
    ../btif_rtk/co/bta_pan_co.c \
    ../btif_rtk/co/bta_gattc_co.c \
    ../btif_rtk/co/bta_gatts_co.c \

# sbc encoder
LOCAL_SRC_FILES+= \
	../embdrv_rtk/sbc/encoder/srce/sbc_analysis.c \
	../embdrv_rtk/sbc/encoder/srce/sbc_dct.c \
	../embdrv_rtk/sbc/encoder/srce/sbc_dct_coeffs.c \
	../embdrv_rtk/sbc/encoder/srce/sbc_enc_bit_alloc_mono.c \
	../embdrv_rtk/sbc/encoder/srce/sbc_enc_bit_alloc_ste.c \
	../embdrv_rtk/sbc/encoder/srce/sbc_enc_coeffs.c \
	../embdrv_rtk/sbc/encoder/srce/sbc_encoder.c \
	../embdrv_rtk/sbc/encoder/srce/sbc_packing.c \

LOCAL_SRC_FILES+= \
	../udrv_rtk/ulinux/uipc.c

LOCAL_C_INCLUDES+= . \
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
	$(LOCAL_PATH)/../audio_a2dp_hw \
	$(LOCAL_PATH)/../utils/include \
	$(bdroid_C_INCLUDES) \
	external/tinyxml2
ifeq ($(BOARD_HAVE_BLUETOOTH_RTK),true)
LOCAL_C_INCLUDES+= . \
    $(LOCAL_PATH)/../realtek/include \
    $(LOCAL_PATH)/../bta/hh
endif
ifeq ($(BOARD_HAVE_BLUETOOTH_RTK_COEX),true)
LOCAL_C_INCLUDES+= . \
    $(LOCAL_PATH)/../hci/include
endif
LOCAL_CFLAGS += -DBUILDCFG $(bdroid_CFLAGS) -Werror -Wno-error=maybe-uninitialized -Wno-error=uninitialized -Wno-error=unused-parameter

ifeq ($(TARGET_PRODUCT), full_crespo)
     LOCAL_CFLAGS += -DTARGET_CRESPO
endif
ifeq ($(TARGET_PRODUCT), full_crespo4g)
     LOCAL_CFLAGS += -DTARGET_CRESPO
endif
ifeq ($(TARGET_PRODUCT), full_maguro)
     LOCAL_CFLAGS += -DTARGET_MAGURO
endif

ifeq ($(BOARD_HAVE_BLUETOOTH_RTK_COEX),true)
	LOCAL_CFLAGS += -DBLUETOOTH_RTK_COEX
endif

# Fix this
#ifeq ($(TARGET_VARIANT), eng)
#     LOCAL_CFLAGS += -O2 # and other production release flags
#else
#     LOCAL_CFLAGS +=
#endif

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libdl \
    liblog \
    libpower \
    libbt-hci-rtk \
    libbt-utils-rtk

#LOCAL_WHOLE_STATIC_LIBRARIES := libbt-brcm_gki libbt-brcm_stack libbt-brcm_bta

LOCAL_STATIC_LIBRARIES := libbt-brcm_gki-rtk libbt-brcm_bta-rtk libbt-brcm_stack-rtk libtinyxml2
ifeq ($(BOARD_HAVE_BLUETOOTH_RTK),true)
	LOCAL_STATIC_LIBRARIES += \
		libbt-rtkbt
endif

LOCAL_MODULE := bluetooth.rtk
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_REQUIRED_MODULES := libbt-hci-rtk libbt-vendor_usb libbt-vendor_uart bt_stack.conf bt_did.conf auto_pair_devlist.conf

include $(BUILD_SHARED_LIBRARY)
