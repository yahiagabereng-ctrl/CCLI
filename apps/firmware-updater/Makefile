include $(TOPDIR)/rules.mk

PKG_NAME:=firmware-updater
PKG_VERSION:=1.1.0
PKG_RELEASE:=8

include $(INCLUDE_DIR)/package.mk

define Package/firmware-updater
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Modular signed firmware/package updater
  DEPENDS:=+jsonfilter +openssl-util +tpm2-tools
endef

define Package/firmware-updater/description
 Common signed APK update engine with TPM integration and a USB transport.
 HTTPS transport can be added later without changing the update engine.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	cp ./src/firmware_updater.cpp $(PKG_BUILD_DIR)/firmware_updater.cpp
endef

define Build/Compile
	$(TARGET_CXX) $(TARGET_CXXFLAGS) -std=c++17 -O2 -Wall -Wextra -Wpedantic -static-libstdc++ $(PKG_BUILD_DIR)/firmware_updater.cpp -o $(PKG_BUILD_DIR)/firmware-updater $(TARGET_LDFLAGS)
endef

define Package/firmware-updater/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_DIR) $(1)/etc/init.d $(1)/etc/config $(1)/etc/firmware-updater
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/firmware-updater $(1)/usr/sbin/firmware-updater
	$(INSTALL_BIN) ./files/firmware-updater-provision-trust $(1)/usr/sbin/firmware-updater-provision-trust
	$(INSTALL_BIN) ./files/firmware-updater.init $(1)/etc/init.d/firmware-updater
	$(INSTALL_DATA) ./files/firmware-updater.config $(1)/etc/config/firmware-updater
endef

define Package/firmware-updater/postinst
#!/bin/sh
[ -n "$${IPKG_INSTROOT}" ] || {
	rm -f /usr/libexec/firmware-updater/core.sh /usr/libexec/firmware-updater/usb.sh
	rmdir /usr/libexec/firmware-updater 2>/dev/null || true
	/etc/init.d/tespro-usb-apk-update stop 2>/dev/null || true
	/etc/init.d/tespro-usb-apk-update disable 2>/dev/null || true
	/etc/init.d/firmware-updater enable
	/etc/init.d/firmware-updater restart
	# Ensure CCLI is restored even when this updater release is installed by an
	# older updater that did not remember a prior asynchronous CCLI shutdown.
	[ ! -x /etc/init.d/ccli ] || /etc/init.d/ccli start
}
exit 0
endef

$(eval $(call BuildPackage,firmware-updater))
