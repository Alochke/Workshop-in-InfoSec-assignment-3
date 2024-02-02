#include "rule_table_parser.h"

#define SUBNET_SECTIONS_NUM 5
#define SUBNET_DOT_NUMBER 3
#define DECIMAL_BASIS 10 // For strtol.
#define MIN_BYTE_VAL 0
#define MAX_BYTE_VAL 255
#define NUMBER_OF_BYTES_IN_IP 4
#define MAX_MASK_LEN 32
#define MIN_USHORT_VAL 0
#define MAX_USHORST_VAL 65535
#define ANY_SETTING 0

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
		char *temp = (char *)malloc(sizeof(char) * nread);
		MAIN_ERR_CHECK(temp == NULL, printf("%s", MAIN_MALLOC_ERR_MSG);)
		
		strncpy(temp, datapoint_line, nread);
		MAIN_ERR_CHECK(list_insert_key(l, temp) == NULL, free(temp); printf("%s", MAIN_MALLOC_ERR_MSG);)
	}
	free(datapoint_line);
	return EXIT_SUCCESS;
}

/*
	Serves as a factorization of the process of parsing a subnet out of a line of the rules file fed up to the kernel module.

	@prev: The begging of the supposed-to-be subnet specification section of the currently loaded rule.
	@ip: src_ip or a dst_ip taken out of the rule_t we're filling up.
	@prefix_size: The prefix_size memmber corresponding to ip.
	@prefix_mask: The prefix_mask member corresponding to ip
	
	Returns: 0 in case of success (which is equivalent to the subnet parsed having correct syntax.), else, 1.
*/
int parse_subnet(char* prev, unsigned int* ip, char* prefix_size, unsigned int* prefix_mask)
{
	// The code assumes the line was parsed beforehand using strtok till the beggining of the subnet part.

	char* token; // Will be used to store the currently parsed token.
	long temp_long; // Will be used to store values out of strtol.
	char* saveptr; // Will be used to backtrack tokenization to the begginning of the address specification, we need this to distinguish between "any" and an actual subnet.

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
			token = strtok_r(prev, ".", &saveptr);
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

		char* end;
		temp_long = strtol(token, &end, DECIMAL_BASIS);
		MAIN_ERR_CHECK(*end == '\0',)

		MAIN_ERR_CHECK(MIN_BYTE_VAL > temp_long,)
		if (i < NUMBER_OF_BYTES_IN_IP)
		{
			MAIN_ERR_CHECK(MAX_BYTE_VAL < temp_long,)
			((char*) ip)[NUMBER_OF_BYTES_IN_IP- i] = (char) temp_long;
		}
		else
		{
			MAIN_ERR_CHECK(MAX_MASK_LEN < temp_long,);
			*prefix_size = (char) temp_long;
		}
	}
	temp_long = ~(1LU << (MAX_MASK_LEN - *prefix_size));

	*prefix_mask = htonl(temp_long);
	
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
		*port = htons((short) FW_PORT_ABOVE_1023);
	}
	else
	{
		// The section should be a number.
		char* end;
		long temp_long = strtol(token, &end, DECIMAL_BASIS);
		MAIN_ERR_CHECK(*end == '\0' || MIN_BYTE_VAL > temp_long || temp_long > MAX_USHORST_VAL,);
		*port = htons((short) temp_long);
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
int parse_member(void *member, char* keywords[], void *values[], int len, char* delimiters, enum type t)
{
	char *token = strtok(NULL, delimiters);
	MAIN_ERR_CHECK(token == NULL,)
	for(int i = 0; i < len; i++)
		if (!strcmp(keywords[i], token))
		{
			switch (t)
			{
				case CHAR:
					*((char*) member) = *((char*) values[i]);
					break;
				case LONG:
					*((long*) member) = *((long*) values[i]);
			}
			return EXIT_SUCCESS;
		}
	return EXIT_FAILURE;
}

/*
	Parses a line from the rules configuration file and feeds up the configuration specified in the line to rule.

	@rule: The rule that the funcction fills up.
	@char: The line that the function parses.

	Returns: 0 on success (which is equivalent to the line parsed having correct syntax.), 1 on failure.
*/
int rule_table_parser_in_line(rule_t* rule, char* line){
	char* token;

	// Parse rule name.
	token = strtok(line, " ");
	MAIN_ERR_CHECK(token == NULL,)
	MAIN_ERR_CHECK(strlen(token) > 20,)
	strcpy(rule->rule_name, token);

	// Parse directions.
	MAIN_ERR_CHECK(parse_member(&rule->direction, (char*[FW_DIRECTIONS_NUM]){"in", "out", "any"}, (void*[FW_DIRECTIONS_NUM]){DIRECTION_IN, DIRECTION_OUT, DIRECTION_ANY}, FW_DIRECTIONS_NUM , " ", LONG),);
	
	// Parse subnets.
	MAIN_ERR_CHECK(parse_subnet(token, &rule->src_ip, &rule->src_prefix_size, &rule->src_prefix_mask),)
	MAIN_ERR_CHECK(parse_subnet(token, &rule->dst_ip, &rule->dst_prefix_size, &rule->dst_prefix_mask),)

	// Parse protocol.
	MAIN_ERR_CHECK(parse_member(&rule->protocol, (char*[FW_PROTS_NUM]){"TCP", "UDP", "ICMP", "other", "any"}, (void*[FW_PROTS_NUM]){htonl(PROT_TCP), htonl(PROT_UDP), htonl(PROT_ICMP), htonl(PROT_OTHER), htonl(PROT_ANY)}, FW_PROTS_NUM, " ", CHAR),)

	MAIN_ERR_CHECK(parse_port(&rule->src_port),)
	MAIN_ERR_CHECK(parse_port(&rule->dst_port),)

	// Parse ack.
	MAIN_ERR_CHECK(parse_member(&rule->ack, (char*[FW_ACK_NUM]){"yes", "no", "any"}, (void*[FW_ACK_NUM]){ACK_YES, ACK_NO, ACK_ANY}, FW_ACK_NUM, " ", LONG),)

	MAIN_ERR_CHECK(parse_member(&rule_t->action, (char*[FW_ACTIONS_NUM]){"accept, drop"}, (void*[FW_ACTIONS_NUM]){NF_ACCEPT, NF_DROP}, FW_ACTIONS_NUM, "", CHAR),)
	
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
	MAIN_ERR_CHECK((table == malloc(sizeof(rule_t) * l->size)) == NULL, printf("%s", MAIN_FILE_FORMAT_ERR_MSG););
	
	list_node *node = l->head;
	for (size_t i = 0; i < l->size; i++)
	{
		MAIN_ERR_CHECK(rule_table_parser_in_line(&table[i], node->key), printf("%s", MAIN_FILE_FORMAT_ERR_MSG);)
	}
	
	return EXIT_SUCCESS;
}
