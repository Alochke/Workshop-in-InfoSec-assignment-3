from scapy.all import IP, ICMP, send

# Craft the IP packet
ip_packet = IP(src="127.0.0.1", dst="10.1.1.1")

# Craft the ICMP packet (ping request)
icmp_packet = ICMP()

# Combine the IP and ICMP packets
packet = ip_packet / icmp_packet

# Send the packet
send(packet)
