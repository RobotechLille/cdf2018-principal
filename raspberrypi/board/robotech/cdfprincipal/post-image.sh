#!/bin/sh

>> "${BINARIES_DIR}/rpi-firmware/config.txt"
		cat << __EOF__ >> "${BINARIES_DIR}/rpi-firmware/config.txt"

# activates I2C connectivity
dtparam=i2c_arm=on,i2c_arm_baudrate=1000000
__EOF__


