from scapy.all import IP, UDP, ICMP, TCP, sendp, Ether, send, conf

conf.route.add(net= ”10.1.1.1/24” , gw="10.1.2.3")

# Craft the IP packet
ip_packet = IP(src = "10.1.2.6")

# Craft the ICMP packet (ping request)
transort= TCP(dport = 47)

# Combine the IP and ICMP packets
packet = ip_packet / transort

# Send the packet
send(packet)