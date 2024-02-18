import socket
import struct

# IP header fields
version_ihl = 5      # IPv4, 5-word header
dscp_ecn = 0         # Default DSCP and ECN
total_length = 20    # Total length of the IP packet
identification = 123 # Identification
flags_offset = 0     # Flags and Fragment Offset
ttl = 64             # Time to Live
protocol = 132       # Protocol number (replace with your desired protocol)
source_address = '10.1.1.1'
destination_address = '10.1.1.2'

# IP header
ip_header = struct.pack('!BBHHHBBH4s4s',
                        (version_ihl << 4) | dscp_ecn,
                        total_length,
                        identification,
                        flags_offset,
                        ttl,
                        protocol,
                        0,  # Checksum (set to 0 for now)
                        0,  # Source IP address (filled later)
                        0)  # Destination IP address (filled later)

# Calculate IP header checksum
checksum = 0
for i in range(0, len(ip_header), 2):
    checksum += (ip_header[i] << 8) + ip_header[i + 1]

checksum = (checksum >> 16) + (checksum & 0xFFFF)
checksum = ~checksum & 0xFFFF

# Insert the checksum into the IP header
ip_header = ip_header[:10] + struct.pack('!H', checksum) + ip_header[12:]

# Convert source and destination IP addresses to binary format
source_address = socket.inet_aton(source_address)
destination_address = socket.inet_aton(destination_address)

# Complete the IP header
ip_header = ip_header[:12] + source_address + destination_address + ip_header[24:]

# Payload (replace with your actual payload)
payload = b"Hello, custom protocol!"

# Create the raw socket
raw_socket = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_RAW)

# Combine the IP header and payload
packet = ip_header + payload

# Send the raw packet
raw_socket.sendto(packet, (destination_address, 0))

# Close the raw socket
raw_socket.close()
