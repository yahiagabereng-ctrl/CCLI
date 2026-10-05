include $(TOPDIR)/rules.mk

PKG_NAME:=ccli
# Keep in sync with apps/ccli/VERSION (run scripts/sync-ccli-version.ps1)
PKG_VERSION:=0.1.0
PKG_RELEASE:=29

PKG_MAINTAINER:=HiTEKS CCLI
PKG_LICENSE:=Proprietary

PKG_BUILD_PARALLEL:=1
PKG_BUILD_DEPENDS:=cmake/host

# Local source tree (repo apps/ccli). Override when packaging from another layout:
#   make package/ccli/compile CCLI_SRC_DIR=/path/to/apps/ccli
CCLI_SRC_DIR?=$(CURDIR)/../../apps/ccli

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

# TesPro MT798X (mediatek/filogic) — platform_tg500, ubus dido_v2 for DIO.
CCLI_BUILD_PLATFORM:=tg500
CCLI_FORCE_GPIOD:=OFF
CCLI_RUNTIME_DEPS:=+libmodbus

CMAKE_INSTALL:=1
PKG_USE_NINJA:=1
CMAKE_SOURCE_SUBDIR:=.

CMAKE_OPTIONS += \
	-DBUILD_PLATFORM:STRING=$(CCLI_BUILD_PLATFORM) \
	-DCCLI_BUILD_TESTS:BOOL=OFF \
	-DCCLI_FORCE_LIBGPIOD:BOOL=$(CCLI_FORCE_GPIOD) \
	-DCCLI_FORCE_LIBMODBUS:BOOL=ON \
	-DCMAKE_CXX_STANDARD=17

define Build/Download
endef

define Build/Prepare
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) $(CCLI_SRC_DIR)/* $(PKG_BUILD_DIR)/
endef

define Package/ccli
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=CCLI Central Plant Controller (PF2 / CEI 0-16)
  DEPENDS:=$(CCLI_RUNTIME_DEPS)
  URL:=https://github.com/hiteks/ccli
endef

define Package/ccli/description
  HiTEKS CCLI user-space firmware: PF2 FSM, Modbus, IEC 61850 MMS (Eth_A), IEC 60870-5-104 (Eth_B).
  Platform HAL: platform_tg500 on TesPro TG544 / TR500 / TG-524 (MT798X).
endef

define Package/ccli/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/ccli $(1)/usr/sbin/ccli

	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/ccli.init $(1)/etc/init.d/ccli

	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/ccli.config $(1)/etc/config/ccli

	$(INSTALL_DIR) $(1)/etc/ccli
	$(INSTALL_CONF) $(CCLI_SRC_DIR)/config/lab_tr400_phase1_regulation.yaml $(1)/etc/ccli/lab.yaml
	$(INSTALL_DIR) $(1)/etc/ccli/tls
	$(CP) $(CCLI_SRC_DIR)/config/tls/* $(1)/etc/ccli/tls/

	$(INSTALL_DIR) $(1)/usr/share/ccli/config
	$(CP) $(PKG_BUILD_DIR)/config/modbus $(1)/usr/share/ccli/config/
	$(CP) $(PKG_BUILD_DIR)/config/icd $(1)/usr/share/ccli/config/

	$(INSTALL_DIR) $(1)/www/ccli
	$(CP) $(CURDIR)/../../lab/tg544-openwrt/www/ccli/* $(1)/www/ccli/

	$(INSTALL_DIR) $(1)/www/cgi-bin
	$(INSTALL_BIN) $(CURDIR)/../../lab/tg544-openwrt/cgi-bin/ccli-bench $(1)/www/cgi-bin/ccli-bench

	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_BIN) ./files/99-ccli-enable $(1)/etc/uci-defaults/99-ccli-enable
endef

$(eval $(call BuildPackage,ccli))
