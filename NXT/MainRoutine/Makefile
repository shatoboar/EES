# Target specific macros
nxtOSEK = /usr/local/mindstorms/nxtOSEK
TARGET = mainRoutine3
TARGET_SOURCES = \
	mainRoutine3.c \
	duties.c \
	motor_helper.c \
	mainTask.c
TOPPERS_OSEK_OIL_SOURCE = ./mainRoutine3.oil
ROOT=$(nxtOSEK)

# Don't modify below part
O_PATH ?= build
include $(nxtOSEK)/ecrobot/ecrobot.mak
