include $(TOPDIR)/rules.mk

PKG_NAME:=ccli-watchdog
PKG_VERSION:=0.1.0
PKG_RELEASE:=55
PKG_MAINTAINER:=HiTEKS CCLI
PKG_LICENSE:=Proprietary

CCLI_SRC_DIR?=$(CURDIR)/../../apps/ccli

include $(INCLUDE_DIR)/package.mk

define Build/Prepare
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) $(CCLI_SRC_DIR)/watchdog/ccli_watchdog.c $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) -std=c11 -D_POSIX_C_SOURCE=200809L \
		$(PKG_BUILD_DIR)/ccli_watchdog.c -o $(PKG_BUILD_DIR)/ccli-watchdog $(TARGET_LDFLAGS)
endef

define Package/ccli-watchdog
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=CCLI independent watchdog supervisor
  DEPENDS:=+ccli
endef

define Package/ccli-watchdog/description
  Independent monotonic health supervisor for CCLI on TesPro TG544.
  It requests CCLI safe abort and relies on procd for respawn; hardware WDT is not enabled.
endef

define Package/ccli-watchdog/postinst
#!/bin/sh
if [ -z "$${IPKG_INSTROOT}" ]; then
	chown root:root /etc/init.d/ccli-watchdog 2>/dev/null || true
	/etc/init.d/ccli-watchdog enable 2>/dev/null || true
	/etc/init.d/ccli-watchdog restart 2>/dev/null || /etc/init.d/ccli-watchdog start 2>/dev/null || true
fi
exit 0
endef

define Package/ccli-watchdog/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ccli-watchdog $(1)/usr/sbin/ccli-watchdog
	$(INSTALL_BIN) ./files/ccli-watchdog.init $(1)/etc/init.d/ccli-watchdog
endef

$(eval $(call BuildPackage,ccli-watchdog))
