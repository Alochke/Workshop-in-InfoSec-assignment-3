from scapy.all import IP, UDP, ICMP, TCP, sendp, Ether, send, conf

# Craft the IP packet
ip_packet = IP(dst = "10.1.2.15")

# Craft the ICMP packet (ping request)
transort= TCP(sport = 1024, dport = 43534)

# Combine the IP and ICMP packets
packet = ip_packet / transort

# Send the packet
send(packet)