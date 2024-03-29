#include "rule_table.h"

#define SUBNET_DOT_NUMBER 3
#define DECIMAL_BASIS 10 // For strtol.
#define MAX_BYTE_VAL 255
#define MAX_MASK_LEN 32
#define MAX_USHORST_VAL 65535
#define ANY_SETTING 0 // The value we'll fill into ips, masks, ports and mask lengths in case thier set within the address space was defined by any.
#define ABOVE_1023	1024 // The value of a port member in a rule_t in case the rule_t represents a rule where a matching port number is every port above 1023.
#define MAX_RULES 50 // Maximal amount of rules in a rule table.
#define MAX_RULE_NAME_LEN 20
#define MAX_SUBNET_LEN 18
#define MAX_PORT_LEN 5
#define ANY_STR "any"
#define ABOVE_1023_STR ">1023"

// I intentionally don't put this in fw.h, beause rule_table.c and logs.c pass different arries to main_deserialize_field, so some of this should be different in the two c files.
#define DIRECTIONS_NUM 3 // The number of possible values for the direction member of a rule_t.
#define PROTS_NUM 4 // The number of possible values for the prot member of a rulet_t.
#define ACK_NUM 3 // The number of possible values for the ack member of a rulet_t.
#define ACTIONS_NUM 4 // The number of possible non-file-terminating specifiers for the action of a rule in a rule table configuration file.
#define END_ACTIONS_NUM 2 // The number of possible file-terminating specifiers for the action of a rule in a rule table configuration file.
#define SUBNET_SECTIONS_NUM 5 // The number of decimal numbers within a subnet specification.
#define MASK_FROM_SIZE(mask_size) ((mask_size == 0) ? 0 : ~((1LU << (MAX_MASK_LEN - (mask_size))) - 1))
#define DIRECTION_STRS (char*[DIRECTIONS_NUM]){"in", "out", "any"} // The direction specifications that can appear in a rule table configuration file.
#define DIRECTION_VALS (unsigned int[DIRECTIONS_NUM]){DIRECTION_IN, DIRECTION_OUT, DIRECTION_ANY} // The possible values of a direction member of a rule_t.
#define PROT_STRS (char*[PROTS_NUM]){"TCP", "UDP", "ICMP", "any"} // The protocol specifications that can appear in a rule table configuration file.
#define PROT_VALS (unsigned int[PROTS_NUM]){PROT_TCP, PROT_UDP, PROT_ICMP, PROT_ANY} // The legal values of a protocol member of a rule_t.
#define ACK_STRS (char*[ACK_NUM]){"yes", "no", "any"} // The ack configuration that can appear in a rule table configuration file.
#define ACK_VALS (unsigned int[ACK_NUM]){ACK_YES, ACK_NO, ACK_ANY} // The possible values of an ack member of a rule_t.
#define ACTION_STRS (char*[ACTIONS_NUM]){"accept\r\n", "accept\n", "drop\r\n", "drop\n"} // The possible strings for an accepetion/dropping verdict configuration within a rule table configuration file that doesn't appear at the end of the file.
#define ACTION_VALS (unsigned int[ACTIONS_NUM]){NF_ACCEPT, NF_ACCEPT, NF_DROP, NF_DROP} // The possible values of the action member of a rule_t, respectively to the string that appear in ACTION_STRS.
#define END_ACTION_STRS (char*[END_ACTIONS_NUM]){"accept", "drop"} // The possible strings for an accepetion/dropping verdict configuration within a rule table configuration file that appear at the end of the file.
#define END_ACTION_VALS (unsigned int[END_ACTIONS_NUM]){NF_ACCEPT, NF_DROP} // The possible values of the action member of a rule_t, respectively to the string that appear in END_ACTION_STRS.

/*
	Serves as a refactorization of the procedure of parsing a subnet out of a line of the rules file fed up to the kernel module.

	Parameters:
	- ip: src_ip or a dst_ip taken out of the rule_t we're filling up.
	- prefix_size: The prefix_size member corresponding to ip.
	- prefix_mask: The prefix_mask member corresponding to ip
	
	Returns: 0 in case of success (which is equivalent to the subnet parsed having correct syntax.), else, 1.
*/
int parse_subnet(unsigned int* ip, unsigned char *prefix_size, unsigned int *prefix_mask)
{
	// The code assumes the line was parsed beforehand using strtok till the beggining of the subnet part.

	char* token; // Will be used to store the currently parsed token.
	unsigned int temp_int; // Will be used to store values out of strtol.
	char* saveptr; // Will be used to backtrack tokenization to the begginning of the subnet specification, we need this to distinguish between "any" and an actual subnet syntax.
	char* end; // Will be used to infer the end of a number within the token.

	// Dealing with "any"
	token = strtok(NULL, " ");
	MAIN_SIMPLE_ERR_CHECK(token == NULL)
	if (strcmp(token, "any") == SRTCMP_OF_EQ)
	{
		*ip = ANY_SETTING;
		*prefix_mask = ANY_SETTING;
		*prefix_size = ANY_SETTING;
		return EXIT_SUCCESS;
	}

	// Dealing with actual subnet specification syntax.
	for (size_t i = 0; i < SUBNET_SECTIONS_NUM; i++)
	{
		if (i == 0)
		{
			token = strtok_r(token, ".", &saveptr);
		}
		else if (i < SUBNET_DOT_NUMBER)
		{
			token = strtok_r(NULL, ".", &saveptr);
		}
		else if(i == SUBNET_DOT_NUMBER)
			token = strtok_r(NULL, "/", &saveptr);
		else
			token = strtok_r(NULL, "", &saveptr);
		
		MAIN_SIMPLE_ERR_CHECK(token == NULL)

		MAIN_SIMPLE_ERR_CHECK(token[0] == ' ' || token[0] == '+' || token[0] == '-')

		temp_int = strtol(token, &end, DECIMAL_BASIS);
		MAIN_SIMPLE_ERR_CHECK(end != &token[strlen(token)]) // From end == &token[strlen(token)] and (token[0] != ' ' && token[0] != '-' && token[0] != '+') we can infer that the token is a number without a sign specification.

		if (i <= MAIN_MAX_INDX_OF_BYTE_IN_ADDRESS)
		{
			MAIN_SIMPLE_ERR_CHECK(MAX_BYTE_VAL < temp_int)
			((char*)ip)[MAIN_MAX_INDX_OF_BYTE_IN_ADDRESS - i] = (char)temp_int; // We're parsing the ip by bytes, so we must write it by bytes.
		}
		else
		{
			MAIN_SIMPLE_ERR_CHECK(MAX_MASK_LEN < temp_int);
			*prefix_size = (unsigned char) temp_int;
		}
	}

	*prefix_mask = MASK_FROM_SIZE(*prefix_size);
	
	return EXIT_SUCCESS;
}

/*
	Parses the port section of a firewall rules configuration file's line and puts the data parsed inside a port member of the relevant rule_t.

	Parameters:
	- port: The port member of the relevant rule_t, namely, src_port or dst_port.

	Returns: 0 on success (which is equivalent to the section parsed having correct syntax.), else, returns 1.
*/
int parse_port(unsigned short* port)
{
	char* token;
	token = strtok(NULL, " ");
	MAIN_SIMPLE_ERR_CHECK(token == NULL)
	if(strcmp(ANY_STR, token) == SRTCMP_OF_EQ)
		*port = ANY_SETTING;
	else if(strcmp(ABOVE_1023_STR, token) == SRTCMP_OF_EQ)
	{
		*port = ABOVE_1023;
	}
	else
	{
		// The section should be a number.
		char* end;
		MAIN_SIMPLE_ERR_CHECK(token[0] == ' ' || token[0] == '+' || token[0] == '-')
		unsigned int temp_long = strtol(token, &end, DECIMAL_BASIS);
		MAIN_SIMPLE_ERR_CHECK(end != &token[strlen(token)] || temp_long > MAX_USHORST_VAL); // From end == &token[strlen(token)] and (token[0] != ' ' && token[0] != '-' && token[0] != '+') we can infer that the token is a number without a sign specification.
		*port = (unsigned short) temp_long;
	}

	return EXIT_SUCCESS;
}

/*
	The applicable values for the t parameter of parse_member.
*/
enum type{
	CHAR,
	INT
};

/*
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
*/
int parse_member(void *member, char* keywords[], unsigned int values[], int len, char* delimiters, enum type t)
{
	/*
		This function can be refactored even more, together with main_deseralize_field, if we'll mix them into one function,
		but right now, it's not worth the time.
	*/
	
	char *token = strtok(NULL, delimiters);
	MAIN_SIMPLE_ERR_CHECK(token == NULL)
	for(int i = 0; i < len; i++)
		if (strcmp(keywords[i], token) == SRTCMP_OF_EQ)
		{
			switch (t)
			{
				case CHAR:
					*((unsigned char*) member) = (unsigned char) values[i];
					break;
				case INT:
					*((unsigned int*) member) = values[i];
			}
			return EXIT_SUCCESS;
		}
	return EXIT_FAILURE;
}

/*
	Parses a line from the rules configuration file and loads the configuration specified in the line to rule.

	Parameters:
	- rule: The address of the rule_t that the function fills up.
	- line: The line that the function parses.
	- last: true iff line is the last line of the file parsed, used internally for error checking.

	Returns: 0 on success (which is equivalent to the line parsed having correct syntax.), 1 on failure.
*/
int rule_table_in_line(rule_t* rule, char* line, bool last)
{
	char* token;

	// Parse rule name.
	token = strtok(line, " ");
	MAIN_SIMPLE_ERR_CHECK(token == NULL)
	MAIN_SIMPLE_ERR_CHECK(strlen(token) > 20)
	strncpy(rule->rule_name, token, MAX_RULE_NAME_LEN);

	// Parse directions.
	MAIN_SIMPLE_ERR_CHECK(parse_member(&rule->direction, DIRECTION_STRS, DIRECTION_VALS, DIRECTIONS_NUM , " ", INT));
	
	// Parse subnets.
	MAIN_SIMPLE_ERR_CHECK(parse_subnet(&rule->src_ip, &rule->src_prefix_size, &rule->src_prefix_mask))
	MAIN_SIMPLE_ERR_CHECK(parse_subnet(&rule->dst_ip, &rule->dst_prefix_size, &rule->dst_prefix_mask))

	// Parse protocol.
	MAIN_SIMPLE_ERR_CHECK(parse_member(&rule->protocol, PROT_STRS, PROT_VALS, PROTS_NUM, " ", CHAR))

	// Parse ports.
	MAIN_SIMPLE_ERR_CHECK(parse_port(&rule->src_port))
	MAIN_SIMPLE_ERR_CHECK(parse_port(&rule->dst_port))

	// Parse ack.
	MAIN_SIMPLE_ERR_CHECK(parse_member(&rule->ack, ACK_STRS, ACK_VALS, ACK_NUM, " ", INT))

	if (!last)
	{
		MAIN_SIMPLE_ERR_CHECK(parse_member(&rule->action, ACTION_STRS, ACTION_VALS, ACTIONS_NUM, "", CHAR))
	}
	else
		MAIN_SIMPLE_ERR_CHECK(parse_member(&rule->action, END_ACTION_STRS, END_ACTION_VALS, END_ACTIONS_NUM, "", CHAR))
	
	return EXIT_SUCCESS;
}

/*
	Parses the rule table from the list l.

	Assumptions: l has all lines of the rules file as the keys of its nodes.

	Parameters:
	- table: Pointing to the pointer from main.c that will point to the base of an array of rule_ts representing the rule table we're configuring.
	- l: The list we'll parse the rule table from.

	Returns: 0 on success, else, returns 1.
*/
int rule_table_init(rule_t **table, list *l)
{
	MAIN_MSG_ERR_CHECK((*table = malloc(sizeof(rule_t) * l->size)) == NULL,,MAIN_MALLOC_ERR_MSG);
	
	MAIN_MSG_ERR_CHECK(l->size > MAX_RULES, free(*table), MAIN_FILE_FORMAT_ERR_MSG)

	list_node *node = l->head;
	for (size_t i = 0; i < l->size; i++)
	{
		MAIN_MSG_ERR_CHECK(rule_table_in_line(&(*table)[i], node->key, i == LIST_LAST_NODE_INDX(l)), free(*table), MAIN_FILE_FORMAT_ERR_MSG)
		node = node->next;
	}
	
	return EXIT_SUCCESS;
}

/*
	Deserializes the subnet given.

	Parameters:		
	- subnet: The string that we'll store the subnet specification syntax in.
	- ip: The ip that specifies the subnet as an unsigned int.
	- prefix_size: The size of the subnet mask.
*/
void  deserialize_subnet(char* subnet, unsigned int ip, unsigned char prefix_size)
{
	if ((ip == ANY_SETTING) && (prefix_size == ANY_SETTING))
	{
		strcpy(subnet, "any");
		return;
	}
	snprintf(subnet, MAX_SUBNET_LEN + MAIN_NULL_INCLUDED, "%d.%d.%d.%d/%d", ((unsigned char*) &ip)[MAIN_MAX_INDX_OF_BYTE_IN_ADDRESS], ((unsigned char*) &ip)[MAIN_MAX_INDX_OF_BYTE_IN_ADDRESS - 1], ((unsigned char*) &ip)[MAIN_MAX_INDX_OF_BYTE_IN_ADDRESS - 2], ((unsigned char*) &ip)[MAIN_MAX_INDX_OF_BYTE_IN_ADDRESS - 3], prefix_size);
}

/*
	Deserializes member, which represents a set of ports that a rule applies to.

	Parameters:
	- port: A pointer to the string that will store member deserialized.
	- member: The serialized value that specifies the set of ports that the deserialized rule_t applies to.
*/
void deserialize_port(char* port, unsigned short member)
{
	switch(member)
	{
		case ANY_SETTING:
			strcpy(port, ANY_STR);
			return;
		case ABOVE_1023:
			strcpy(port, ABOVE_1023_STR);
			return;
	}
	snprintf(port, MAX_PORT_LEN + MAIN_NULL_INCLUDED, "%hu", member);
}

/*
	Prints rule table.

	Parameters:
	- fptr: A pointer to the FILE you get by opening "/sys/class/fw/rules/rules", we assume it was not read from or written previously to the function run-time,
		and that it has read permissions.

	Returns: 0 on success, 1 on failure.
*/
int rule_table_print(FILE* fptr)
{
	size_t rule_num;
	clearerr(fptr); // We do that for the next check of getc's success.
	rule_num = getc(fptr);
	MAIN_MSG_ERR_CHECK(ferror(fptr),, MAIN_RULE_TABLE_READ_ATTRIBUTE_ERR_MSG)
	rule_t* temp = malloc(sizeof(rule_t));
	MAIN_MSG_ERR_CHECK(temp == NULL,, MAIN_MALLOC_ERR_MSG);
	char rule_name[MAX_RULE_NAME_LEN + MAIN_NULL_INCLUDED], src_subnet[MAX_SUBNET_LEN + MAIN_NULL_INCLUDED], dest_subnet[MAX_SUBNET_LEN + MAIN_NULL_INCLUDED], src_port[MAX_PORT_LEN + MAIN_NULL_INCLUDED], dst_port[MAX_PORT_LEN + MAIN_NULL_INCLUDED];
	rule_name[MAX_RULE_NAME_LEN] = '\0'; // We're making sure that rule_name will be NULL terminated throughout the function.
	char *direction, *ack, *action, *prot;

	for (size_t i = 0; i < rule_num; i++)
	{
		fread(temp, sizeof(rule_t), 1, fptr);
		MAIN_MSG_ERR_CHECK(ferror(fptr), free(temp), MAIN_RULE_TABLE_READ_ATTRIBUTE_ERR_MSG);
		strncpy(rule_name, temp->rule_name, MAX_RULE_NAME_LEN);
		main_deserialize_field(&direction, temp->direction, DIRECTION_STRS, DIRECTION_VALS, DIRECTIONS_NUM);
		deserialize_subnet(src_subnet, temp->src_ip, temp->src_prefix_size);
		deserialize_subnet(dest_subnet, temp->dst_ip, temp->dst_prefix_size);
		main_deserialize_field(&prot, temp->protocol, PROT_STRS, PROT_VALS, PROTS_NUM);
		deserialize_port(src_port, temp->src_port);
		deserialize_port(dst_port, temp->dst_port);
		main_deserialize_field(&ack, temp->ack, ACK_STRS, ACK_VALS, ACK_NUM);
		main_deserialize_field(&action, temp->action, END_ACTION_STRS, END_ACTION_VALS, END_ACTIONS_NUM);

		printf("%s %s %s %s %s %s %s %s %s\n", rule_name, direction, src_subnet, dest_subnet, prot, src_port, dst_port, ack, action);
	}
	free(temp);

	return EXIT_SUCCESS;
}
