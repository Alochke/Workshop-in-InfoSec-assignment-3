from scapy.all import IP, ICMP, TCP, send

# Craft the IP packet
ip_packet = IP(src="10.1.1.1", dst="10.1.1.2")

# Craft the ICMP packet (ping request)
tcp_packet = TCP(sport=23, dport=1024, flags= "A")

# Combine the IP and ICMP packets
packet = ip_packet / tcp_packet

# Send the packet
send(packet)
