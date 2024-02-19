from scapy.all import IP, ICMP, TCP, sendp, Ether, send

# Craft the IP packet
ip_packet = IP(dst = "10.1.2.14")

# Craft the ICMP packet (ping request)
transort= TCP(sport = 1024, dport = 23, flags = "A")

# Combine the IP and ICMP packets
packet = ip_packet / transort

# Send the packet
send(packet)
