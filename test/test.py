from scapy.all import IP, UDP, ICMP, TCP, sendp, Ether, send, conf

# Craft the IP packet
ip_packet = IP(dst = "10.1.1.6")

# Craft the ICMP packet (ping request)
transort= TCP(dport = 47)

# Combine the IP and ICMP packets
packet = ip_packet / transort

# Send the packet
send(packet)