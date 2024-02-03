#include "rule_table_parser.h"

#define SUBNET_SECTIONS_NUM 5
#define SUBNET_DOT_NUMBER 3
#define DECIMAL_BASIS 10 // For strtol.
#define MIN_BYTE_VAL 0
#define MAX_BYTE_VAL 255
#define MAXIMAL_INDX_OF_BYTE_IN_ADDRESS 3
#define BYTES_NUM_IN_ADDRESS 4
#define MAX_MASK_LEN 32
#define MIN_USHORT_VAL 0
#define MAX_USHORST_VAL 65535
#define ANY_SETTING 0
#define DIRECTIONS_NUM 3 // The number of possible values for the direction member of a rule_t.
#define PROTS_NUM 5 // The number of possible values for the prot member of a rulet_t.
#define ACK_NUM 3 // The number of possible values for the ack member of a rulet_t.
#define ACTIONS_NUM 4 // The number of possible non-file-terminating specifiers for the action of a rule in a rule table configuration file.
#define END_ACTIONS_NUM 2 // The number of possible file-terminating specifiers for the action of a rule in a rule table configuration file.
#define MAX_RULES 50 // Maximal amount of rules in a rule table.
#define LAST_NODE_INDX(list) (list->size - 1)
#define SIZE_PLUS_NULL(x) (strlen(x) + 1)

/*
	Parses FILE into a list where every node has a line of stream as it's token.
	Assumptions: stream wasn't read beforehand, l is empty.

	@l: Will have all of stream's lines stored as the keys of nodes.
	@stream: The address of the FILE will read the lines from.

	Returns: Amount of lines read on success, 1 upon Failure.
*/
int rule_table_parser_list_lines(list *l, FILE *stream)
{
    char *datapoint_line = NULL;
	size_t length = 0;
	int nread;

	while ((nread = getline(&datapoint_line, &length, stream)) != EOF)
	{
		char *temp = (char *)malloc(sizeof(char) * SIZE_PLUS_NULL(datapoint_line));
		
		MAIN_ERR_CHECK(temp == NULL, printf("%s", MAIN_MALLOC_ERR_MSG);)
		
		strcpy(temp, datapoint_line);
		MAIN_ERR_CHECK(list_insert_key(l, temp) == NULL, free(temp); printf("%s", MAIN_MALLOC_ERR_MSG);)
	}
	free(datapoint_line);
	return EXIT_SUCCESS;
}

/*
	Serves as a factorization of the process of parsing a subnet out of a line of the rules file fed up to the kernel module.

	@ip: src_ip or a dst_ip taken out of the rule_t we're filling up.
	@prefix_size: The prefix_size memmber corresponding to ip.
	@prefix_mask: The prefix_mask member corresponding to ip
	
	Returns: 0 in case of success (which is equivalent to the subnet parsed having correct syntax.), else, 1.
*/
int parse_subnet(unsigned int* ip, char* prefix_size, unsigned int* prefix_mask)
{
	// The code assumes the line was parsed beforehand using strtok till the beggining of the subnet part.

	char* token; // Will be used to store the currently parsed token.
	long temp_long; // Will be used to store values out of strtol.
	char* saveptr; // Will be used to backtrack tokenization to the begginning of the address specification, we need this to distinguish between "any" and an actual subnet.
	char* end; // Will be used to infer the end of a number within the token.

	// Dealing with "any"
	token = strtok(NULL, " ");
	MAIN_ERR_CHECK(token == NULL,)
	if (!strcmp(token, "any"))
	{
		ip = ANY_SETTING;
		prefix_mask = ANY_SETTING;
		prefix_mask = ANY_SETTING;
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
		
		MAIN_ERR_CHECK(token == NULL,)

		MAIN_ERR_CHECK(token[0] == ' ',)

		temp_long = strtol(token, &end, DECIMAL_BASIS);
		MAIN_ERR_CHECK(end != &token[strlen(token)],) // From end == &token[strlen(token)] and token[0] != ' ' we can infer that the token is a number.

		MAIN_ERR_CHECK(MIN_BYTE_VAL > temp_long,)
		if (i < BYTES_NUM_IN_ADDRESS)
		{
			MAIN_ERR_CHECK(MAX_BYTE_VAL < temp_long,)
			((char*)ip)[MAXIMAL_INDX_OF_BYTE_IN_ADDRESS - i] = (char)temp_long;
		}
		else
		{
			MAIN_ERR_CHECK(MAX_MASK_LEN < temp_long,);
			*prefix_size = (char) temp_long;
		}
	}
	temp_long = ~((1LU << (MAX_MASK_LEN - *prefix_size)) - 1);

	*prefix_mask = temp_long;
	
	return EXIT_SUCCESS;
}

/*
	Parses the port section of a fire wall rules configuration file line and puts the data parsed inside a port member of the relevent rule_t.

	@port: The port memeber of the relevent rule_t, namely, src_port or dst_port.

	Returns: 0 on succes (which is equivalent to the section parsed having correct syntax.),
		else, returns 1.
*/
int parse_port(unsigned short* port)
{
	char* token;
	token = strtok(NULL, " ");
	MAIN_ERR_CHECK(token == NULL,)
	if(!strcmp("any", token))
		*port = 0;
	else if(!strcmp(">1023", token))
	{
		*port = FW_PORT_ABOVE_1023;
	}
	else
	{
		// The section should be a number.
		char* end;
		MAIN_ERR_CHECK(token[0] == ' ',)
		long temp_long = strtol(token, &end, DECIMAL_BASIS);
		MAIN_ERR_CHECK(end != &token[strlen(token)] || MIN_BYTE_VAL > temp_long || temp_long > MAX_USHORST_VAL,); // From end == &token[strlen(token)] and token[0] != ' ' we can infer that the token is a number.
		*port = (short) temp_long;
	}

	return EXIT_SUCCESS;
}

/*
	The applicable values for the t parameter of parse_member.
*/
enum type{
	CHAR,
	LONG
};

/*
	A refactorization of the procedure of parsing a token from a rule line and
	giving the member field of the corresponding rule_t with the appropriate value.

	@member: The filled member of the rule_t corresponding to the rule parsed.
	@keywords: The keywords that can be found within the field being parsed, all should be NULL terminated strings.
		s.t. keywords[i] means that member should be given values[i].
	@values: The applicable values for member.
	@delimeters: The delimeters we'll use to parse the relevant member, concatenated within a string.
	@t: Will be used to determine the type of member.

	Returns: 0 on success (which is equivalent to the field parsed having correct syntax.), 1 on failure.
*/
int parse_member(void *member, char* keywords[], long values[], int len, char* delimiters, enum type t)
{
	char *token = strtok(NULL, delimiters);
	MAIN_ERR_CHECK(token == NULL,)
	for(int i = 0; i < len; i++)
		if (!strcmp(keywords[i], token))
		{
			switch (t)
			{
				case CHAR:
					*((char*) member) = (char) values[i];
					break;
				case LONG:
					*((long*) member) = values[i];
			}
			return EXIT_SUCCESS;
		}
	return EXIT_FAILURE;
}

/*
	Parses a line from the rules configuration file and feeds up the configuration specified in the line to rule.

	- rule (rule_t*): The rule that the funcction fills up.
	- char (char*): The line that the function parses.
	- last (last*): true iff line is the last line of the file parsed, used internally

	Returns: 0 on success (which is equivalent to the line parsed having correct syntax.), 1 on failure.
*/
int rule_table_parser_in_line(rule_t* rule, char* line, bool last){
	char* token;

	// Parse rule name.
	token = strtok(line, " ");
	MAIN_ERR_CHECK(token == NULL,)
	MAIN_ERR_CHECK(strlen(token) > 20,)
	strcpy(rule->rule_name, token);

	// Parse directions.
	MAIN_ERR_CHECK(parse_member(&rule->direction, (char*[DIRECTIONS_NUM]){"in", "out", "any"}, (long[DIRECTIONS_NUM]){DIRECTION_IN, DIRECTION_OUT, DIRECTION_ANY}, DIRECTIONS_NUM , " ", LONG),);
	
	// Parse subnets.
	MAIN_ERR_CHECK(parse_subnet(&rule->src_ip, &rule->src_prefix_size, &rule->src_prefix_mask),)
	MAIN_ERR_CHECK(parse_subnet(&rule->dst_ip, &rule->dst_prefix_size, &rule->dst_prefix_mask),)

	// Parse protocol.
	MAIN_ERR_CHECK(parse_member(&rule->protocol, (char*[PROTS_NUM]){"TCP", "UDP", "ICMP", "other", "any"}, (long[PROTS_NUM]){PROT_TCP, PROT_UDP, PROT_ICMP, PROT_OTHER, PROT_ANY}, PROTS_NUM, " ", CHAR),)

	// Parse ports.
	MAIN_ERR_CHECK(parse_port(&rule->src_port),)
	MAIN_ERR_CHECK(parse_port(&rule->dst_port),)

	// Parse ack.
	MAIN_ERR_CHECK(parse_member(&rule->ack, (char*[ACK_NUM]){"yes", "no", "any"}, (long[ACK_NUM]){ACK_YES, ACK_NO, ACK_ANY}, ACK_NUM, " ", LONG),)

	if (!last)
	{
		MAIN_ERR_CHECK(parse_member(&rule->action, (char*[ACTIONS_NUM]){"accept\r\n", "accept\n", "drop\r\n", "drop\n"}, (long[ACTIONS_NUM]){NF_ACCEPT, NF_ACCEPT, NF_DROP, NF_DROP}, ACTIONS_NUM, "", CHAR),)
	}
	else
		MAIN_ERR_CHECK(parse_member(&rule->action, (char*[END_ACTIONS_NUM]){"accept", "drop"}, (long[END_ACTIONS_NUM]){NF_ACCEPT, NF_DROP}, ACTIONS_NUM, "", CHAR),)
	
	return EXIT_SUCCESS;
}

/*
	Parses the rule table from the list l, which is assumed to have all lines of the rules file as the keys of its nodes.

	Parameters:
		- table (rule_t*): Pointing to the base of an array of rule_ts representing the rule table we're configuring.
		- l (list*): A list which is assumed to have all lines of the rules file as the keys of its nodes.

	Returns: 0 on success, else, returns 1.
*/
int rule_table_parser_in_init(rule_t *table, list *l)
{
	MAIN_ERR_CHECK((table = malloc(sizeof(rule_t) * l->size)) == NULL, printf("%s", MAIN_MALLOC_ERR_MSG););
	
	MAIN_ERR_CHECK(l->size > MAX_RULES, printf("%s", MAIN_FILE_FORMAT_ERR_MSG);)

	list_node *node = l->head;
	for (size_t i = 0; i < l->size; i++)
	{
		MAIN_ERR_CHECK(rule_table_parser_in_line(&table[i], node->key, i == LAST_NODE_INDX(l)), printf("%s", MAIN_FILE_FORMAT_ERR_MSG);)
		node = node->next;
	}
	
	return EXIT_SUCCESS;
}
