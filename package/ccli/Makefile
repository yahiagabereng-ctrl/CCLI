include $(TOPDIR)/rules.mk

PKG_NAME:=ccli
# Keep in sync with apps/ccli/VERSION (run scripts/sync-ccli-version.ps1)
PKG_VERSION:=0.1.0
PKG_RELEASE:=76

PKG_MAINTAINER:=HiTEKS CCLI
PKG_LICENSE:=Proprietary

PKG_BUILD_PARALLEL:=1
PKG_BUILD_DEPENDS:=cmake/host

# Local source tree (repo apps/ccli). Product line (program/main.c):
#   make package/ccli/compile CCLI_SRC_DIR=$(CURDIR)/../../product/apps/ccli
CCLI_SRC_DIR?=$(CURDIR)/../../apps/ccli
CCLI_REPO_DIR?=$(abspath $(CCLI_SRC_DIR)/../..)

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

# TesPro MT798X (mediatek/filogic) — platform_tg500, ubus dido_v2 for DIO.
CCLI_BUILD_PLATFORM:=tg500
CCLI_FORCE_GPIOD:=OFF
CCLI_RUNTIME_DEPS:=+libmodbus +curl +openssl-util

CMAKE_INSTALL:=1
PKG_USE_NINJA:=1
CMAKE_SOURCE_SUBDIR:=.

CMAKE_OPTIONS += \
	-DBUILD_PLATFORM:STRING=$(CCLI_BUILD_PLATFORM) \
	-DCCLI_BUILD_TESTS:BOOL=OFF \
	-DCCLI_FORCE_LIBGPIOD:BOOL=$(CCLI_FORCE_GPIOD) \
	-DCCLI_FORCE_LIBMODBUS:BOOL=ON \
	-DCCLI_WITH_LIB60870:BOOL=ON \
	-DCCLI_REQUIRE_PROTOCOL_DEPENDENCIES:BOOL=ON \
	-DCMAKE_POLICY_VERSION_MINIMUM:STRING=3.5 \
	-DCMAKE_CXX_STANDARD=17

define Build/Download
endef

define Build/Prepare
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)
	tar -C "$(CCLI_SRC_DIR)" --exclude='./build*' --exclude='.git' --exclude='./config/tls' --exclude='./config/EJBCA RBAC' --exclude='*.local.json' --exclude='__pycache__' -cf - . | tar -C "$(PKG_BUILD_DIR)" -xf -
	$(CP) $(CCLI_REPO_DIR)/scripts/generate_mms_certificate_auth.py $(PKG_BUILD_DIR)/cmake/
	$(CP) $(CCLI_REPO_DIR)/scripts/generate_mms_e2e.py $(PKG_BUILD_DIR)/cmake/
	$(CP) $(CCLI_REPO_DIR)/lab/conformance/CCI_RBAC_TLS_BASELINE.json $(PKG_BUILD_DIR)/cmake/
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

# Preserve files owned by older APKs before apk removes their obsolete payload.
# The init script inhibits implicit starts until restoration and verification.
define Package/ccli/preinst
#!/bin/sh
set -eu
[ -n "$${IPKG_INSTROOT:-}" ] && exit 0
umask 077
pending=/etc/ccli-upgrade.pending
[ ! -e "$$pending" ] || { echo "CCLI: unfinished upgrade preservation at $$pending" >&2; exit 1; }
backup=$$(mktemp -d /etc/ccli-upgrade.XXXXXX)
[ ! -d /etc/ccli ] || cp -a /etc/ccli "$$backup/ccli"
[ ! -f /etc/config/ccli ] || cp -p /etc/config/ccli "$$backup/uci-ccli"
printf '%s\n' "$$backup" > "$$pending"
chmod 600 "$$pending"
echo "CCLI: live configuration and credentials saved in $$backup"
endef

define Package/ccli/postinst
#!/bin/sh
set -eu
if [ -z "$${IPKG_INSTROOT:-}" ]; then
	if [ -f /etc/ccli-upgrade.pending ]; then
		backup=$$(cat /etc/ccli-upgrade.pending)
		case "$$backup" in /etc/ccli-upgrade.??????) ;; *) echo "Invalid CCLI backup path" >&2; exit 1;; esac
		[ -d "$$backup" ] && [ ! -L "$$backup" ] || exit 1
		if [ -d "$$backup/ccli" ]; then
			mkdir -p /etc/ccli
			cp -a "$$backup/ccli/." /etc/ccli/
		fi
		[ ! -f "$$backup/uci-ccli" ] || cp -p "$$backup/uci-ccli" /etc/config/ccli
		: > /etc/ccli-upgrade.hold
		chmod 600 /etc/ccli-upgrade.hold
		rm -f /etc/ccli-upgrade.pending
		echo "CCLI: configuration restored; backup retained at $$backup"
	fi
	# Ensure the init script is visible to LuCI's rc backend even when an APK
	# built on a bind-mounted Windows/WSL tree preserves a non-root owner.
	chown root:root /usr/sbin/ccli /etc/init.d/ccli /etc/uci-defaults/99-ccli-enable 2>/dev/null || true
	chown -R root:root /usr/share/ccli/config 2>/dev/null || true
	echo "CCLI: verify configuration, remove /etc/ccli-upgrade.hold and start CCLI explicitly."
fi
exit 0
endef

define Package/ccli/conffiles
/etc/config/ccli
/etc/config/ccli-crl
/etc/ccli/lab.yaml
endef

define Package/ccli/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/ccli $(1)/usr/sbin/ccli
	$(INSTALL_BIN) ./files/ccli-crl-fetch $(1)/usr/sbin/ccli-crl-fetch
	$(INSTALL_BIN) ./files/ccli-crl-refresh $(1)/usr/sbin/ccli-crl-refresh
	$(INSTALL_BIN) ./files/ccli-crl-worker $(1)/usr/sbin/ccli-crl-worker

	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/ccli.init $(1)/etc/init.d/ccli
	$(INSTALL_BIN) ./files/ccli-crl.init $(1)/etc/init.d/ccli-crl

	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/ccli.config $(1)/etc/config/ccli
	$(INSTALL_CONF) ./files/ccli-crl.config $(1)/etc/config/ccli-crl

	$(INSTALL_DIR) $(1)/etc/ccli
	$(INSTALL_CONF) $(CCLI_SRC_DIR)/config/lab_tespro_192_168_0_1_23ln.yaml $(1)/etc/ccli/lab.yaml

	$(INSTALL_DIR) $(1)/etc/ccli/icd
	$(INSTALL_CONF) $(CCLI_SRC_DIR)/config/icd/product/product_tr400_eth_a_all_mandatory.cfg $(1)/etc/ccli/icd/
	$(INSTALL_CONF) $(CCLI_SRC_DIR)/config/icd/product/product_tr400_eth_a_all_mandatory.cid $(1)/etc/ccli/icd/

	$(INSTALL_DIR) $(1)/usr/share/ccli/config
	$(CP) $(PKG_BUILD_DIR)/config/modbus $(1)/usr/share/ccli/config/
	$(CP) $(PKG_BUILD_DIR)/config/icd $(1)/usr/share/ccli/config/
	$(CP) $(PKG_BUILD_DIR)/config/rbac $(1)/usr/share/ccli/config/

	$(INSTALL_DIR) $(1)/www/ccli
	$(CP) $(CCLI_REPO_DIR)/lab/tg544-openwrt/www/ccli/* $(1)/www/ccli/

	$(INSTALL_DIR) $(1)/www/cgi-bin
	$(INSTALL_BIN) $(CCLI_REPO_DIR)/lab/tg544-openwrt/cgi-bin/ccli-bench $(1)/www/cgi-bin/ccli-bench

	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_BIN) ./files/99-ccli-enable $(1)/etc/uci-defaults/99-ccli-enable
endef

$(eval $(call BuildPackage,ccli))
