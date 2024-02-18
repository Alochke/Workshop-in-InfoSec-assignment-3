from scapy.all import IP, ICMP, TCP, send

# Craft the IP packet
ip_packet = IP(src="127.0.0.1", dst="10.1.1.1")

# Craft the ICMP packet (ping request)
tcp_packet = TCP(sport=0, dport=23)

# Combine the IP and ICMP packets
packet = ip_packet / tcp_packet

# Send the packet
send(packet)
