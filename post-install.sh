#!/bin/sh

objectFiles=`find . -name "*.o"`
ssh root@192.168.1.209 "/etc/init.d/phiaquarium stop" &&
echo "Copying object files ..." && 
scp $objectFiles root@192.168.1.209:/root/phiaquarium_debug/ && 
echo "Copying phiaquarium binary ..." &&
scp phiaquarium root@192.168.1.209:/usr/bin/ && 
echo "Copy on target : done" && 
ssh root@192.168.1.209 "/etc/init.d/phiaquarium start"