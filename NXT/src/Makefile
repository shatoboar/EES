# Target specific macros
nxtOSEK = /usr/local/mindstorms/nxtOSEK
TARGET = Main
TARGET_SOURCES = \
	mainRoutine.c \
	duties.c \
	motor_helper.c \
	bluetooth_module.c
TOPPERS_OSEK_OIL_SOURCE = ./mainRoutine.oil
ROOT=$(nxtOSEK)

# Don't modify below part
O_PATH ?= build
include $(nxtOSEK)/ecrobot/ecrobot.mak
