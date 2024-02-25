============================= main.c =============================

    This c file contains the main function, surprisingly.
    In this function things are pretty straightforward, the arguments are checked, and if those aren't correct, an error message is printed, else, the right device is opened and its file descriptor/FILE* is passed to a function that is imported from a .c file, but we skip that step for clearing the logs, because that is as simple as just writing to the device anything.
    The only quirk in the main function is the way it implements rule loading, it does so by utilizing a linked list API implemented in list.c, which parses the lines and then passes the list to a function implemented in rule_table.c to create a buffer of serialized data that the main function writes to a firewall's char device. 

    main.c also stores a function called main_deseralize_field just because this function is used in rule_table.c and logs.c, and it seems like overkill- to create a new .c file just for that function, and also because main.c file is naturally the only one above log.c and rule_table.c in any reasonable hierarchy.

    int main_deseralize_field(char **field, unsigned int member, char* keywords[], unsigned int values[], size_t len)
        Deserializes the member- member, of the deserialized rule_t or log_row_t, and points the pointer pointed by field to the deserialized data.
        Decides the correct value by the next assumption,
        keyword[i] is the correct value for *field iff values[i] == member.

        Parameters:
		- field: The pointer it points to will point to the deserialized data after the function is run.
		- member: The member to deserialize.
		- keywords: An array of applicable values for *field.
		- values: An array of the possible values of member.
		- len: The length of keywords and values.

        Returns: 0 in case there is an 0<=i<=(len - 1) s.t. member == values[i], in any other case, will return 1.

    static void cleanup(enum stage stg)
        Cleans the user interface.

        Parameters:
        - stg (stage): A designated enum's member that represents the stage of initialization the user interface is at.

============================= list.c =============================

    This c file implements a linked list interface.
    It's used for parsing the rules txt into rows and then reversing the list, so the list nodes will be in an order that beholds that the first node will store the first line in the parsed txt file, the second node will store the second an so on...
    One can argue that this makes the program overly complex, but using the interface was just a matter of copy-pasting because this is recycled code from a previous project I had, so reusing it saved precious time.

    int list_init(list **l)
        Initializes l.

		Parameters:
        - l: A pointer to a pointer that will point to the initialized list.

    list_node *list_insert(list *l, list_node *node)
        Inserts node into l from the head-end of l.

		Parameters:
        - l: The address of the list node will be inserted into.
        - node: The address of the inserted node.

        Returns: node's address, or NULL if the initialization failed.

    list_node *list_insert_key(list *l, char *key)
        Inserts a node that will be initialized with key as its key, to the head-end of l.

		Parameters:
        - l: The address of the list we'll insert the key into.
        - key: The string that node->key will point to.

        Returns: A pointer with the inserted node's address, or NULL if the initialization failed.

    void list_destroy(list *l)
        Frees l and all of its nodes.

		Parameters:
        - l: A pointer to the freed list.

    void list_reverse(list *l)
        Reverses *l.

		Parameters:
        - l: A pointer to the reversed list.

    int list_parse_lines(list *l, FILE *stream)
        Parses stream into a list where every node has a line of stream as its key.
        
        Assumptions: stream wasn't read beforehand, and *l is empty.

        Parameters:
        - l: Address of the list that will have all of stream's lines stored as the keys of its nodes.
        - stream: The address of the FILE we'll read the lines from.

        Returns: 0 on success, 1 upon Failure.

============================= logs.c =============================

    This implements the interaction of the program with the logs,
    which is really not that complex. It's just one function, logs_read, that is responsible for reading the logs, when given a file descriptor of /dev/fw_log.
    
	It's true that we also may need to write to a log's sysfs attribute, but that involves such a simple procedure that it's not worth the trouble of creating a function; In that procedure, we just write to a file.

    int logs_read(int fd)
        Reads the logs from a file descriptor and after that prints the logs.
        
        Assumptions: fd is a read-permitted file descriptor of /dev/fw_log, that has its offset set to 0.

        Parameters:
        - fd: A file descriptor that's supposed to be a read-permitted file descriptor of /dev/fw_log, and have its offset set to 0.

        Returns: 1 on success, 0 on failure.

============================= rule_table.c =============================

    This c file is responsible for serializing a rule table's data when the program loads a new table to the kernel module.
    The c file does so by iterating over the nodes of a linked list that is provided by the main function where every node's struct has a string member that contains a line of the configuration file.
    This c file is also responsible for deserializing and reading, a serialized rule table from the kernel module and then printing it to standard output.

    int parse_subnet(unsigned int* ip, unsigned char *prefix_size, unsigned int *prefix_mask)
        Serves as a refactorization of the procedure of parsing a subnet out of a line of the rules file fed up to the kernel module.

        Parameters:
        - ip: src_ip or a dst_ip taken out of the rule_t we're filling up.
        - prefix_size: The prefix_size member corresponding to ip.
        - prefix_mask: The prefix_mask member corresponding to ip
        
        Returns: 0 in case of success (which is equivalent to the subnet parsed having correct syntax.), else, 1.

    int parse_port(unsigned short* port)
        Parses the port section of a firewall rules configuration file's line and puts the data parsed inside a port member of the relevant rule_t.

        Parameters:
        - port: The port member of the relevant rule_t, namely, src_port or dst_port.

        Returns: 0 on success (which is equivalent to the section parsed having correct syntax.), else, returns 1.

    int parse_member(void *member, char* keywords[], unsigned int values[], int len, char* delimiters, enum type t)
        A refactorization of the procedure of parsing a token from a rule line and
        giving a member of the corresponding rule_t the appropriate value.

        Parameters:
        - member: The address of the filled member of the rule_t that corresponds to the rule parsed.
        - keywords: The keywords that can be found within the field being parsed, all should be NULL terminated strings.
            s.t. keywords[i] means that member should be given the value values[i].
        - values: The applicable values for member.
        - len: The length of values and keywords.
        - delimiters: A pointer to a null-terminated string where the delimiters we use to parse the member are concatenated.
        - t: Will be used to determine the type of member.

        Returns: 0 on success (which is equivalent to the field parsed having correct syntax.), 1 on failure.

    int rule_table_in_line(rule_t* rule, char* line, bool last)
        Parses a line from the rules configuration file and loads the configuration specified in the line to rule.

        Parameters:
        - rule: The address of the rule_t that the function fills up.
        - line: The line that the function parses.
        - last: true iff line is the last line of the file parsed, used internally for error checking.

        Returns: 0 on success (which is equivalent to the line parsed having correct syntax.), 1 on failure.

    int rule_table_init(rule_t **table, list *l)
        Parses the rule table from the list l.

		Assumptions: l has all lines of the rules file as the keys of its nodes.

        Parameters:
        - table: Pointing to the pointer from main.c that will point to the base of an array of rule_ts representing the rule table we're configuring.
        - l: The list we'll parse the rule table from.

        Returns: 0 on success, else, returns 1.

    void  deserialize_subnet(char* subnet, unsigned int ip, unsigned char prefix_size)
        Deserializes the subnet given.

        Parameters:     
        - subnet: The string that we'll store the subnet specification syntax in.
        - ip: The ip that specifies the subnet as an unsigned int.
        - prefix_size: The size of the subnet mask.

    void deseralize_port(char* port, unsigned short member)
        Deserializes member, which represents a set of ports that a rule applies to.

        Parameters:
        - port: A pointer to the string that will store member deserialized.
        - member: The serialized value that specifies the set of ports that the deserialized rule_t applies to.

    int rule_table_print(FILE* fptr)
        Prints rule table.

        Parameters:
        - fptr: A pointer to the FILE you get by opening "/sys/class/fw/rules/rules", we assume it was not read from or written previously to the function run-time, and that it has read permissions.

        Returns: 0 on success, 1 on failure.
    