from scapy.all import IP, ICMP, TCP, UDP, sendp, Ether, send

# Craft the IP packet
ip_packet = IP(dst = "10.1.2.3")

# Combine the IP and ICMP packets
packet = ip_packet

# Send the packet
send(packet)
