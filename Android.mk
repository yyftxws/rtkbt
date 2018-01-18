ifeq ($(BOARD_HAVE_BLUETOOTH_RTK),true)
LOCAL_PATH := $(call my-dir)
$(info $(shell rm -rf !{$(OUT)/obj/*/*tinyxml* $(OUT)/obj/*/*bt*}  $(OUT)/obj/*/*luetooth* $(OUT)/obj/*/*hci* $(OUT)/*/*a2dp*  ))
include $(call all-subdir-makefiles)
#$(info $(shell rm -rf !{$(OUT)/obj/*/*tinyxml* $(OUT)/obj/*/*bt*}  $(OUT)/obj/*/*luetooth* $(OUT)/obj/*/*hci* $(OUT)/*/*a2dp*  ))
endif
