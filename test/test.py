from scapy.all import IP, UDP, ICMP, TCP, sendp, Ether, send, conf

# Craft the IP packet
ip_packet = IP(dst = "10.1.2.15")

# Craft the ICMP packet (ping request)
transort= TCP(sport = 23, dport = 1025, flags = "A")

# Combine the IP and ICMP packets
packet = ip_packet / transort

# Send the packet
send(packet)