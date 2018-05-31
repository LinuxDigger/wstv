#!/bin/sh

STORAGE_PATH=/etc/conf.d/jovision/network


default_interface()
{
	echo "No interface.cfg, build it now"
	echo "#iface is the name of the dev, such as interface0/ppp0/ra0">/etc/network/interface.cfg
	echo "iface=eth0">>/etc/network/interface.cfg
	echo "#inet=static/dhcp/ppp/wifi">>/etc/network/interface.cfg
	echo "inet=dhcp">>/etc/network/interface.cfg
	echo "">>/etc/network/interface.cfg
	echo "ip=192.8.8.8">>/etc/network/interface.cfg
	echo "netmask=255.255.255.0">>/etc/network/interface.cfg
	echo "gateway=192.8.8.1">>/etc/network/interface.cfg
	echo "dns=8.8.8.8">>/etc/network/interface.cfg
}

[ -d $STORAGE_PATH ] || mkdir -p $STORAGE_PATH
[ -d /etc/network ] || mkdir /etc/network
[ -d /etc/ppp/ ] || mkdir /etc/ppp/
[ -f $STORAGE_PATH/pppoe.conf ] || cp /etc/ppp/* $STORAGE_PATH
[ -d /var/run ] || mkdir /var/run

if ! mountpoint /etc/network
then
        mount $STORAGE_PATH/ /etc/network
        mount $STORAGE_PATH/ /etc/ppp
fi

[ -f /etc/network/interface.cfg ] || default_interface

##################################################
# interface setting now


ifconfig eth0 up
ifconfig lo up

#ɾ��Ĭ��·�ɣ���������IPʱ��������ܶ�Ĭ��·��
route del default
route del 255.255.255.255

. /etc/network/interface.cfg
echo interface info here...
echo iface   = $iface
echo inet    = $inet
echo ip      = $ip
echo netmask = $netmask
echo gateway = $gateway
echo dns     = $dns

#��ӹ㲥·��
add_broadcast_route()
{
	route add -net 255.255.255.255 netmask 255.255.255.255 dev $iface
}

killall udhcpc
killall wpa_supplicant

ifconfig eth0:0 169.254.0.1
echo "eth0 preparing..." > /var/run/jvnetstatus
#set interface ip
case $inet in
	dhcp)
		echo iface $iface udhcpc 
		udhcpc -i $iface -q && echo "`ifconfig eth0`" > /var/run/jvnetstatus && add_broadcast_route &
		;;
	static)
		echo iface $iface static 
		ifconfig $iface $ip netmask $netmask
		route add default gw $gateway
		echo "nameserver $dns">/etc/resolv.conf
		echo "`ifconfig eth0`" > /var/run/jvnetstatus
		echo "interface, static ip..."
		add_broadcast_route
		;;
	ppp)
		echo iface $iface pppoe
		#ifconfig eth0 $ip netmask $netmask
		#For hisi 3507, �������һ��eth0 down, eth0 up�Ķ���������PPPOE���£��������Եȣ��ù���ȥ��
		sleep 1
		pppoe-start
		add_broadcast_route
		;;
	wifi)
		#echo iface $iface wifi
		##ifconfig eth0 down
		#ifconfig $iface up
		#/wifi/wpa_supplicant -B -i$iface -c /etc/network/wpa_supplicant.conf -Dwext
		##�����dhcpҪ��̨���з����������ʱ�ò���ip��ַ��Զ������ȥ�ˡ�����
		#udhcpc -i $iface -q&
		#sleep 2
		#add_broadcast_route
		;;
	*)
		echo inet error: $inet
		default_interface
		add_broadcast_route
		;;
esac
