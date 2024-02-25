A stateless firewall for Ubuntu 16.04.7 LTS,

The firewall provides the utilities of loading a static rule table into the module, logging the packets that went through the stateless packet filtering,
showing the currently loaded rule table to the user, and of course, stateless packet filtering.

Only IPv4 forwarding-ment packets, which are also TCP, UDP or ICMP are going through the filters, when there aren't any rules loaded, all of these packets will be dropped. That means packets that are sent to or sent by a process of the firewall machine won't be affected by the program.

============================= The kernel Module =============================
    
    All code related to the module can be found in the module sub-directory.
    You can compile and install the module by using the next commands when the module is your pwd:
        make
        sudo insmod firewall.ko
    To remove the module you can use the next command:
        sudo rmmod firewall
    
    The module will add the following files to your system:
    - /dev/rules
    - /dev/fw_log
    - Some unnamed symlinks at /dev/char.
    - The directory /sys/class/fw and the files within it.
    (Compilation files will be added within the module directory but all of those can be cleaned up if you choose to clean them.)

    When a packet doesn't match any rule- it's dropped.
    The module also drops every Christmas tree packet.

    You can refer to README.md in the module sub-directory for dry documentation of the code of the kernel module and the functions defined within the code's files.

============================= The User Interface =============================
    
    All code related to the module is found within the user sub-directory.
    You can compile the user interface by using the next command when module is your pwd:
        make
    This will add an executable named main to the sub-directory, you can use it by running:
    sudo ./main <command>
    When a user is your pwd.
    <command> can be one of:
    - show_rules
    - show_log
    - clear_log
    and you can also call:
    sudo ./main load_rules <path_to_rules_file>

    "show_rules" will show you the currently loaded rule table, nothing will be printed in case there isn't any rule table currently loaded.
    Every row in the output will describe a rule and the format of each row will be:
    <rule name> <direction> <source subnet> <destination subnet> <protocol> <source port> <destination port> <ack> <action>
    <direction> can be one of:
        - in
        - out
        - any
    "in" means that only packets that are routed to the nic enp0s8, which is assumed to be the nic facing the internal network will be affected by the rule,
    "out" means that only packets that are routed to the nic enp0s9, which is assumed to be the nic facing the outwards network will be affected by the rule,
    "any" means a packet that the rule applies to can be routed to any nic.
    <source subnet> and <destination subnet> represent the subnets that the rule applies to, those can be one of:
    - <ip>/<mask length>
    - any
    For a rule to apply to a packet, the source address should be within the subnet specified by <source subnet> and the destination address should be within the subnet specified by <destination subnet>.
    "any" means the rule applies to any source address or any destination address, that depends on the place where it appears, of course.
    <protocol> can be one of:
        - TCP
        - UDP
        - ICMP
        - any
    and specifies the transfer protocol that the rule applies to, obviously.
    <source port> and <destination port> specify the source and destination transfer protocol ports that a rule applies to, 
    respectively, and can be one of:
        - any
        - An integer within the range 0-65535
        - >1023
    any means that packets can be of any port, integers other than 0 and 1024 mean that only ports that are equal to the integer can be matched to the rule, 0 means any port fall within the domain affected by the rule, 1024 means any port larger than 1023 is in the this domain, ">1023" has the same meaning. The reason for this design decision is the next line in the file fw.h that was given to us with the assignment,
    "unsigned short dst_port;           // number of port or 0 for any or port 1023 for any port number > 1023"
    and a discussion about the assignment, that led to the conclusion that it should be "port 1024" and not "port 1023" in this comment.
    <source port> and <destination port> don't have any significance for ICMP packets.
    <ack> doesn't have any significance to packets that are not TCP can be one of
        - yes
        - no
        - any
    If it's "yes", the rule applies only to TCP packets that have their ack flag turned on,
    if it's "no", the rule applies only to TCP packets that have their ack flag turned off,
    and if it's "any", the rule applies to a TCP packet whether the flag is on or off.
    <action> can be one of:
        - accept
        - drop
    "accept" means that every packet that is within the rule's domain of enforcement will be carried on, and "drop" means that it will be dropped.
    The rule specified by line 0 has the highest precedence, the rule after it is second in precedence and so on...

    "sudo ./main load_rules <path_to_rules_file>" configures the rule table.
    <path_to_rules_file> should be the path to a txt file that contains the configuration of a rule table.
    The format of the file should be exactly as the format printed by "sudo ./main show_rules", except, it should not end with a new line.

    "show_log" prints the logs,
    Those will provide data about every packet that passes through the filter.
    Its source and destination IPs, its source and destination ports (which will be 0 for ICMP packets), its transfer protocol, the action taken by the firewall about this packet, the reason for this action, which will be a rule's index in case the reason is a rule, how many packets with those parameters equal passed through the filter and the last time one of those did so.

    "clear log" resets the logs.

    You can refer to README.md in the user sub-directory for dry documentation of the code of the user interface and the functions defined within the code's files.
