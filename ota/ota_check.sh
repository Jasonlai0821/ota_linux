#!/bin/sh
#
# Start 3308 linux service....
#

case "$1" in
	start)
		# ueventd
		/usr/bin/ota &
		;;
	stop)
		printf "stop ota finished"
        ;;
	*)
        echo "Usage: $0 {start|stop}"
        exit 1
        ;;
esac
sleep 1
exit 0
