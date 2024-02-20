#include "main.h"

#define MAX_ARGC 3
#define ARGC_FOR_NO_RULE_LOADING 2
#define MIN_ARGC 2
#define FIRST_ARG 1
#define LAST_ARG 2
#define RULE_TABLE_ATTRIBUTE "/sys/class/fw/rules/rules"
#define LOGS_READ_DEV "/dev/fw_log"
#define LOGS_ATTRIBUTE "/sys/class/fw/log/reset"
#define MIN_STORE 1 // The minimal amount of bytes we need to write into a sysfs attribute in order for the store function to take action.
#define ERR_CHECK_INIT(condition, state) MAIN_ERR_CHECK(condition, cleanup(state);)
#define ERR_CHECK_INIT_MSG(condition, state, msg) MAIN_ERR_CHECK(condition, cleanup(state); fprintf(stderr ,"%s\n", msg);)

FILE *fptr; // This will be used in case we're loading rules from a file.
list *lst; // We'll use this list to structure the data, when loading rules from a file.
rule_t *rule_table; // Will be used to store data we're writing and reading from/to a rule table.
int fd;
// I wanted to use a fd and not a FILE because I know on write sysfs expects the entire buffer to be passed during the first write,
// and a non-syscall writing function can spread the writing as it sees fit.
// Source: https://docs.kernel.org/filesystems/sysfs.html
// I also prefered to use it in other cases for more control and more predictability of the execution.

/*
    I put the next function here because both rule_table.c and logs.c should use it and it seems like an overkill to put it in a .c of its own,
    plus, main.c file is naturaly the only one above log.c and rule_table.c in any hierarchy.
*/

/*
	Deseralizes the member- member of the deseralized rule_t or log_row_t, and points the pointer pointed by field to the deseralized data.
	Decides the correct value by the next assumption,
	keyword[i] is the correct value for *field iff values[i] == member.

    Parameters:
	- field: The pointer it points to will point to the deseralized data after the function is run.
	- member: The member to deseralize.
	- keywords: An array of applicable values for *field.
	- values: An array of the possible values of member.
	- len: The length of keywords and values.

    Returns: 0 in case there is an 0<=i<=(len - 1) s.t. member == values[i], in any other case, will return 1.
*/
int main_deseralize_field(char **field, unsigned int member, char* keywords[], unsigned int values[], size_t len)
{
	for (size_t i = 0; i < len; i++)
	{
		if (values[i] == member)
		{
			*field = keywords[i];
            return EXIT_SUCCESS;
		}
	}
    return EXIT_FAILURE;
}

/*
    The next enum is for the cleanup function in main.c. Items represent states at which the program can be during exeution and should be cleaned up differently.
*/
enum stage{
    FIRST,
    FILE_OPENED,
    LIST_INIT,
    POST_LIST_INIT,
    RULE_TABLE_INIT,
    MID_OPENING,
    RESET_LOGS
};

/*
	Cleans the user interface.

	Parameters:
    - stg (stage): A designated enum's member that represents the stage of initialization the user interface is at.
*/
static void cleanup(enum stage stg)
{
    // We use the enum- stage, defined in main.c to choose action based on the state of the sysfs initialization the module is currently at. 
    switch (stg)
    {
        case FIRST:
            break;
        case RESET_LOGS:
        case MID_OPENING:
            close(fd);
            if(stg == RESET_LOGS)
                break;
        case RULE_TABLE_INIT:
            free(rule_table);
        case POST_LIST_INIT:
        case LIST_INIT:
            list_destroy(lst);
            if(stg > LIST_INIT)
                break;
        case FILE_OPENED:
            fclose(fptr);
    }
}

int main(int argc, char* argv[])
{
    ERR_CHECK_INIT_MSG(argc > MAX_ARGC || argc < MIN_ARGC, FIRST, MAIN_ARG_ERR_MSG)
    
    if (argc == ARGC_FOR_NO_RULE_LOADING)
    {
        if (strcmp(argv[FIRST_ARG], "show_rules") == SRTCMP_OF_EQ)
        {
            fptr = fopen(RULE_TABLE_ATTRIBUTE, "r");
            ERR_CHECK_INIT_MSG(fptr == NULL, FIRST, MAIN_RULE_TABLE_OPEN_ATTRIBUTE_ERR_MSG)
            rule_table_out_print(fptr);
            fclose(fptr);
        }
        else if (strcmp(argv[FIRST_ARG], "show_log") == SRTCMP_OF_EQ)
        {
            fd = open(LOGS_READ_DEV, O_RDONLY);
            ERR_CHECK_INIT_MSG(fd == MAIN_SYSCALL_FAIL_RETURN, FIRST, MAIN_LOGS_CHAR_DEV_OPEN_ERR_MSG)
            read_logs(fd);
            close(fd);
        }
        else if (strcmp(argv[FIRST_ARG], "clear_log") == SRTCMP_OF_EQ)
        {
            fd = open(LOGS_ATTRIBUTE, O_WRONLY);
            ERR_CHECK_INIT_MSG(fd == MAIN_SYSCALL_FAIL_RETURN, FIRST, MAIN_LOGS_ATTRIBUTE_OPEN_ERR_MSG)
            ERR_CHECK_INIT_MSG(write(fd, "A very functional and important text", MIN_STORE) == MAIN_SYSCALL_FAIL_RETURN, RESET_LOGS, MAIN_LOGS_ATTRIBUTE_WRITE_ERR_MSG) // This is a meme, we could give any buffer to write and it won't change the functionality. See, we're being professional here -_-.
            cleanup(RESET_LOGS);
        }
        else
        {
            fprintf(stderr, "%s\n", MAIN_ARG_ERR_MSG);
            return EXIT_FAILURE;
        }
    }
    else
    {
        ERR_CHECK_INIT_MSG(strcmp(argv[FIRST_ARG], "load_rules") != SRTCMP_OF_EQ, FIRST, MAIN_ARG_ERR_MSG)
        
        fptr = fopen(argv[LAST_ARG], "r");
        ERR_CHECK_INIT_MSG(fptr == NULL, FIRST, MAIN_FILE_OPEN_ERR_MSG)

        ERR_CHECK_INIT_MSG(list_init(&lst), FILE_OPENED, MAIN_MALLOC_ERR_MSG)
        
        ERR_CHECK_INIT_MSG(rule_table_list_lines(lst, fptr), LIST_INIT, MAIN_MALLOC_ERR_MSG)

        fclose(fptr);

        list_reverse(lst);

        ERR_CHECK_INIT(rule_table_in_init(&rule_table, lst), POST_LIST_INIT)

        fd = open(RULE_TABLE_ATTRIBUTE, O_WRONLY);

        ERR_CHECK_INIT_MSG(fd == MAIN_SYSCALL_FAIL_RETURN, RULE_TABLE_INIT, MAIN_RULE_TABLE_OPEN_ATTRIBUTE_ERR_MSG)
        
        ERR_CHECK_INIT_MSG(write(fd, rule_table, (lst->size) * sizeof(rule_t)) == MAIN_SYSCALL_FAIL_RETURN, MID_OPENING, MAIN_RULE_TABLE_WRITE_ATTRIBUTE_ERR_MSG)

        cleanup(MID_OPENING);
    }

    return EXIT_SUCCESS;
}