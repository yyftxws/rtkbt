# RELEASE NAME: 20171130_TV_ANDROID_4.4(Beta)
# RTKBT_API_VERSION=0.1.1.0

BOARD_HAVE_BLUETOOTH := true
BOARD_HAVE_BLUETOOTH_RTK := true
BOARD_HAVE_BLUETOOTH_RTK_COEX := true

#BOARD_HAVE_BLUETOOTH_RTK_ADDON := bee1 rtkbtAutoPairService rtkbtAutoPairUIDemo 
#BOARD_HAVE_BLUETOOTH_RTK_ADDON := bee1 rtkbtAutoPairService rtkbtAutoPairUIDemo vr_bee_hidraw_daemon
#BOARD_HAVE_BLUETOOTH_RTK_ADDON := bee2 rtkbtAutoPairService rtkbtAutoPairUIDemo 
#BOARD_HAVE_BLUETOOTH_RTK_ADDON := bee2 rtkbtAutoPairService rtkbtAutoPairUIDemo vr_bee_hidraw_daemon
BOARD_HAVE_BLUETOOTH_RTK_ADDON := basic

ifneq ($(BOARD_HAVE_BLUETOOTH_RTK_ADDON),)
$(foreach item,$(BOARD_HAVE_BLUETOOTH_RTK_ADDON),$(call inherit-product,$(LOCAL_PATH)/addon/$(item)/addon.mk))
#Firmware For Tv
include $(LOCAL_PATH)/Firmware/TV/TV_Firmware.mk
else
#Firmware For Tablet
include $(LOCAL_PATH)/Firmware/BT/BT_Firmware.mk
endif

PRODUCT_COPY_FILES += \
	$(LOCAL_PATH)/system/etc/bluetooth/rtkbt.conf:system/etc/bluetooth/rtkbt.conf \
	$(LOCAL_PATH)/system/etc/permissions/android.hardware.bluetooth_le.xml:system/etc/permissions/android.hardware.bluetooth_le.xml \
	$(LOCAL_PATH)/system/etc/permissions/android.hardware.bluetooth.xml:system/etc/permissions/android.hardware.bluetooth.xml \
	$(LOCAL_PATH)/system/lib/hw/audio.vr_bee_hidraw.default.so:system/lib/hw/audio.vr_bee_hidraw.default.so \
	$(LOCAL_PATH)/system/lib/rtkbt/autopair.so:system/lib/rtkbt/autopair.so \
	$(LOCAL_PATH)/system/lib/rtkbt/autopair_stack.so:system/lib/rtkbt/autopair_stack.so \
	$(LOCAL_PATH)/system/lib/rtkbt/fwlog.so:system/lib/rtkbt/fwlog.so \
	$(LOCAL_PATH)/system/lib/rtkbt/heartbeat.so:system/lib/rtkbt/heartbeat.so \
	$(LOCAL_PATH)/system/lib/rtkbt/test.so:system/lib/rtkbt/test.so \
	$(LOCAL_PATH)/system/lib/rtkbt/vhid.so:system/lib/rtkbt/vhid.so \
	$(LOCAL_PATH)/system/lib/rtkbt/vr_bee_hidraw.so:system/lib/rtkbt/vr_bee_hidraw.so \
	$(LOCAL_PATH)/system/usr/keylayout/rtkbt_virtual_hid.kl:system/usr/keylayout/rtkbt_virtual_hid.kl \


PRODUCT_PACKAGES += \
	Bluetooth \
	audio.a2dp.default
