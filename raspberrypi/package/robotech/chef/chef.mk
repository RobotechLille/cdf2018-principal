################################################################################
#
# Programme chef - Coupe de France de Robotique
#
################################################################################
CHEF_VERSION = 1.0
CHEF_SITE = $(BR2_EXTERNAL_CDF_PATH)/package/robotech/chef/code
CHEF_SITE_METHOD = local
CHEF_DEPENDENCIES = wiringpi

define CHEF_BUILD_CMDS
	$(MAKE) -C $(@D) clean
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)
endef

define CHEF_INSTALL_TARGET_CMDS
	$(INSTALL) -d -m 0755 $(TARGET_DIR)/opt/chef/bin
	$(INSTALL) -d -m 0755 $(TARGET_DIR)/opt/chef/com
	$(INSTALL) -D -m 0755 $(@D)/bin/* $(TARGET_DIR)/opt/chef/bin
	$(INSTALL) -D -m 0755 $(@D)/run.sh $(TARGET_DIR)/opt/chef
endef

$(eval $(generic-package))
