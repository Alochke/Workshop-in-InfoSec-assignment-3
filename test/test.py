from scapy.all import IP, ICMP, TCP, sendp, Ether, send

# Craft the IP packet
ip_packet = IP(dst = "10.1.1.1")

# Craft the ICMP packet (ping request)
tcp_packet = TCP()

# Combine the IP and ICMP packets
packet = ip_packet / tcp_packet

# Send the packet
send(packet)
