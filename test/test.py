from scapy.all import IP, UDP, ICMP, TCP, sendp, Ether, send

# Craft the IP packet
ip_packet = IP(dst = "10.1.1.1")

# Craft the ICMP packet (ping request)
transort= TCP(dport = 47, flags = "A")

# Combine the IP and ICMP packets
packet = ip_packet / transort

# Send the packet
send(packet)