#include "main.h"

#define MAX_ARGC 3
#define ARGC_FOR_NO_RULE_LOADING 2
#define NO_ARGS 1
#define ONLY_ARG_INDX 1
#define LAST_ARG 2
#define ARG_ERR_MSG "Wrong arguments, you've entered.\n The correct arguments are:\n•  show_rules\n•load_rules <path_to_rules_file>\n•	show_log\n•	clear_log\n"
#define FILE_ERR_MSG "Wrong file path, you've given.\n"
#define MAIN_INIT_ERR_CHECK(condition, stage, msg) MAIN_ERR_CHECK(condition, cleanup(stage); printf("%s", msg);, EXIT_FAILURE)

FILE *fptr; // This will be used in case we're loading rules from a file.
linked_list *lst; // We'll use this list to structure the data.

/*
    The next enum is for the cleanup function in main.c. Items represent the state of the module initialization the module is currently at.
*/
enum stage{
    FIRST,
    FILE_OPENED,
    LIST_FILLED
};

/*
	Cleans the module.

	Parameters:
    - stg (stage): A designated enum's member that represents the stage of initialization the module is at.
*/
static void cleanup(enum stage stg)
{
    // We use the enum- stage, defined in main.c to choose action based on the state of the sysfs initialization the module is currently at. 
    switch (stg)
    {
        case FIRST:
            break;
        case FILE_OPENED:
            fclose(fptr);
        case LIST_FILLED:
            list_destroy(l)
    }
}

int main(int argc, char* argv[])
{
    MAIN_INIT_ERR_CHECK(argc > MAX_ARGC || argc == NO_ARGS, ARG_ERR_MSG)
    
    if (argc == ARGC_FOR_NO_RULE_LOADING)
    {
        if (!strcmp(argv[ONLY_ARG_INDX], "show_rules"))
        {
            /* code */
        }
        else if (!strcmp(argv[ONLY_ARG_INDX], "show_log"))
        {
            /* code */
        }
        else if (!strcmp(argv[ONLY_ARG_INDX], "clear_log"))
        {
            /* code */
        }
    }
    else
    {
        MAIN_INIT_ERR_CHECK(!strcmp(argv[0], "load_rules"), ARG_ERR_MSG)
        
        fptr = fopen(arg[LAST_ARG], "r");
        MAIN_INIT_ERR_CHECK(fptr == NULL, FILE_ERR_MSG);

        list_init(lst);
        int rule_num = rule_table_parser_in_line_linked_list(lst, fptr);
        
    }

    return EXIT_SUCCESS;
}