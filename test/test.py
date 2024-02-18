from scapy.all import IP, ICMP, TCP, sendp, Ether, send

# Craft the IP packet
ip_packet = IP(dst = "10.1.2.2")

# Craft the ICMP packet (ping request)
transort= UDP()

# Combine the IP and ICMP packets
packet = ip_packet / transort

# Send the packet
send(packet)
