from scapy.all import IP, ICMP, TCP, sendp, Ether, send

# Craft the IP packet
ip_packet = IP(dst = "10.1.1.14")

# Craft the ICMP packet (ping request)
transort= TCP(sport = 23, dport = 1024, flags = "A")

# Combine the IP and ICMP packets
packet = ip_packet / transort

# Send the packet
send(packet)
