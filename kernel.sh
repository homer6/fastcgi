sysctl net.core.netdev_max_backlog
sysctl net.ipv4.tcp_max_syn_backlog
sysctl net.core.somaxconn
ifconfig eth0 txqueuelen
sysctl net.ipv4.tcp_tw_recycle
sysctl net.ipv4.tcp_tw_reuse
echo "---------------------------"
sudo sysctl net.core.netdev_max_backlog=5000
sudo sysctl net.ipv4.tcp_max_syn_backlog=5000
sudo sysctl net.core.somaxconn=20048
sudo ifconfig eth0 txqueuelen 20000
sudo sysctl net.ipv4.tcp_tw_recycle=1
sudo sysctl net.ipv4.tcp_tw_reuse=1 

