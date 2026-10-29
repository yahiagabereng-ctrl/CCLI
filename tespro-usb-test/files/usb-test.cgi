#!/bin/sh

printf 'Content-Type: text/plain\r\nCache-Control: no-store\r\n\r\n'
/usr/sbin/usb-test --status
