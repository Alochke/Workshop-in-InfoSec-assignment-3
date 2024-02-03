#include "main.h"

#define MAX_ARGC 3
#define ARGC_FOR_NO_RULE_LOADING 2
#define NO_ARGS 1
#define FIRST_ARG 1
#define LAST_ARG 2
#define RULE_TABLE_ATTRIBUTE "/sys/class/fw/rules/rules"
#define SYSCALL_FAIL_RETURN -1
#define ERR_CHECK_INIT(condition, state) MAIN_ERR_CHECK(condition, cleanup(state);)
#define ERR_CHECK_INIT_MSG(condition, state, msg) MAIN_ERR_CHECK(condition, cleanup(state); printf("%s\n", msg);)

FILE *fptr; // This will be used in case we're loading rules from a file.
list *lst; // We'll use this list to structure the data.
rule_t *rule_table; // Will be used to store data we're writing and reading from/to a rule table.
int fd; // This fd will be of use to us when we'll write the rule configutation to RULE_TABLE_ATTRIBUTE,
// I wanted to use a fd and not a FILE because I know on write sysfs expects the entire buffer to be passed during the first write,
// and a non-syscall writing function can spread the writing as it sees fit.
// Source: https://docs.kernel.org/filesystems/sysfs.html

/*
    The next enum is for the cleanup function in main.c. Items represent the state of the module initialization the module is currently at.
*/
enum stage{
    FIRST,
    FILE_OPENED,
    LIST_INIT,
    POST_LIST_INIT,
    RULE_TABLE_INIT,
    MID_OPENING
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
        case MID_OPENING:
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
    ERR_CHECK_INIT_MSG(argc > MAX_ARGC || argc <= NO_ARGS, FIRST, MAIN_ARG_ERR_MSG)
    
    if (argc == ARGC_FOR_NO_RULE_LOADING)
    {
        if (!strcmp(argv[FIRST_ARG], "show_rules"))
        {
            /* code */
        }
        else if (!strcmp(argv[FIRST_ARG], "show_log"))
        {
            /* code */
        }
        else if (!strcmp(argv[FIRST_ARG], "clear_log"))
        {
            /* code */
        }
        else
        {
            printf("%s", MAIN_ARG_ERR_MSG);
            return EXIT_FAILURE;
        }
    }
    else
    {
        ERR_CHECK_INIT_MSG(strcmp(argv[FIRST_ARG], "load_rules"), FIRST, MAIN_ARG_ERR_MSG)
        
        fptr = fopen(argv[LAST_ARG], "r");
        ERR_CHECK_INIT_MSG(fptr == NULL, FIRST, MAIN_FILE_OPEN_ERR_MSG)

        ERR_CHECK_INIT_MSG(list_init(&lst), FILE_OPENED, MAIN_MALLOC_ERR_MSG)
        
        ERR_CHECK_INIT_MSG(rule_table_list_lines(lst, fptr), LIST_INIT, MAIN_MALLOC_ERR_MSG)

        fclose(fptr);

        list_reverse(lst);

        ERR_CHECK_INIT(rule_table_in_init(&rule_table, lst), POST_LIST_INIT)

        fd = open(RULE_TABLE_ATTRIBUTE, O_WRONLY);

        ERR_CHECK_INIT_MSG(fd == SYSCALL_FAIL_RETURN, POST_LIST_INIT, MAIN_RULE_TABLE_OPENNING_ATTRIBUTE_ERR_MSG)
        
        ERR_CHECK_INIT_MSG(write(fd, rule_table, (lst->size) * sizeof(rule_t)) == SYSCALL_FAIL_RETURN, MID_OPENING, MAIN_RULE_TABLE_WRITING_ATTRIBUTE_ERR_MSG)

        cleanup(MID_OPENING);
    }

    return EXIT_SUCCESS;
}