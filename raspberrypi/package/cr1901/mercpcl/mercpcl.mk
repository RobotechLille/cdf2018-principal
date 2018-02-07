################################################################################
#
# mercpcl
#
################################################################################

MERCPCL_VERSION = ce3d815f76f8f5d7254e5e17a90e30d9affacfea
MERCPCL_SITE = $(call github,cr1901,mercpcl,$(MERCPCL_VERSION))
MERCPCL_DEPENDENCIES = libftdi libusb
MERCPCL_LICENSE = GPLv3
MERCPCL_LICENSE_FILES = LICENSE

$(eval $(cmake-package))
