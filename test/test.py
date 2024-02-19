from scapy.all import IP, ICMP, TCP, sendp, Ether, send

# Craft the IP packet
ip_packet = IP(src = "10.1.1.1", dst = "10.1.2.3")

# Craft the ICMP packet (ping request)
transort= TCP()

# Combine the IP and ICMP packets
packet = ip_packet / transort

# Send the packet
send(packet)
