#!/bin/sh
set -eu
umask 077
source=/etc/ccli/aggregator-backup-20261019
history=/mnt/mmcblk0/ccli-history-20261020
test "$(readlink -f "$source")" = /etc/ccli/aggregator-backup-20261019
test -d "$source" && test ! -L "$source"
test ! -e "$history" || { test -d "$history" && test ! -L "$history" && test -z "$(ls -A "$history")"; }
mount | grep '^/dev/mmcblk0 on /mnt/mmcblk0 type ext4 ' >/dev/null
mkdir -p -m 700 "$history"
chmod 700 "$history"
ls -ld "$history" | grep '^drwx------' >/dev/null
(cd "$source"; find . -type f -exec sha256sum '{}' ';') > "$history/aggregator.sha256"
cp -a "$source" "$history/aggregator-backup-20261019"
(cd "$history/aggregator-backup-20261019"; sha256sum -c "$history/aggregator.sha256") > "$history/copy-verified.txt"
# Keep a verified copy before moving the original archive off limited NOR.
# Both copies remain on the ext4 card, with the old path pointing at the copy.
test "$(readlink -f "$history")" = /mnt/mmcblk0/ccli-history-20261020
mv "$source" "$history/aggregator-original-20261019"
ln -s "$history/aggregator-backup-20261019" "$source"
(cd "$source"; sha256sum -c "$history/aggregator.sha256") > "$history/link-verified.txt"
echo HISTORICAL_BACKUP_PRESERVED_ON_EXT4_CARD_WITH_OLD_PATH_LINK
df -h /overlay /mnt/mmcblk0
du -sk /etc/ccli
