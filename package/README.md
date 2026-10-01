# CCLI OpenWrt feed

Link this directory into an OpenWrt SDK or buildroot as a local feed:

```bash
echo "src-link ccli /path/to/CCLI/package" >> feeds.conf
./scripts/feeds update ccli
./scripts/feeds install ccli
```

Packages:

| Package | Description |
|---------|-------------|
| `ccli` | PF2 / CEI 0-16 application — see `ccli/README.md` |

Source application: `../apps/ccli/`
