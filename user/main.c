#include "main.h"

#define MAX_ARGC 3
#define ARGC_FOR_NO_RULE_LOADING 2
#define NO_ARGS 1
#define FIRST_ARG 1
#define LAST_ARG 2
#define RULE_TABLE_ATTRIBUTE "/sys/class/fw/rules/rules"
#define SYSCALL_FAIL_RETURN -1

FILE *fptr; // This will be used in case we're loading rules from a file.
list *lst; // We'll use this list to structure the data.
rule_t *rule_table;
int fd; // This fd will be of use to us when we'll write the rule configutation to RULE_TABLE_ATTRIBUTE,
// I wanted to use a fd and not a FILE because I know on write sysfs expects the entire buffer to be passed during the first write,
// and a library function can spread the writing as it sees fit.
// Sources: https://docs.kernel.org/filesystems/sysfs.html

/*
    The next enum is for the cleanup function in main.c. Items represent the state of the module initialization the module is currently at.
*/
enum stage{
    FILE_OPENED,
    LIST_INIT,
    RULE_TABLE_INIT,
    MID_OPENING
};

/*
	Cleans the module.

	Parameters:
    - stg (stage): A designated enum's member that represents the stage of initialization the user interface is at.
*/
static void cleanup(enum stage stg)
{
    // We use the enum- stage, defined in main.c to choose action based on the state of the sysfs initialization the module is currently at. 
    switch (stg)
    {
        case FILE_OPENED:
            fclose(fptr);
        case MID_OPENING:
        case LIST_INIT:
            list_destroy(lst);
            if(stg == MID_OPENING)
                close(fd);
        case RULE_TABLE_INIT:
            free(rule_table);
    }
}

int main(int argc, char* argv[])
{
    MAIN_ERR_CHECK(argc > MAX_ARGC || argc <= NO_ARGS, printf("%s", MAIN_ARG_ERR_MSG);)
    
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
            printf("%s", MAIN_ARG_ERR_MSG);
    }
    else
    {
        MAIN_ERR_CHECK(strcmp(argv[FIRST_ARG], "load_rules"), printf("%s", MAIN_ARG_ERR_MSG);)
        
        fptr = fopen(argv[LAST_ARG], "r");
        MAIN_ERR_CHECK(fptr == NULL, printf("%s", MAIN_FILE_OPEN_ERR_MSG);)

        MAIN_ERR_CHECK(list_init(&lst), cleanup(FILE_OPENED); printf("%s", MAIN_MALLOC_ERR_MSG);)
        
        MAIN_ERR_CHECK(rule_table_list_lines(lst, fptr), cleanup(LIST_INIT);)

        list_reverse(lst);

        MAIN_ERR_CHECK(rule_table_in_init(&rule_table, lst),)

        fclose(fptr);
        fd = open(RULE_TABLE_ATTRIBUTE, O_WRONLY);

        MAIN_ERR_CHECK(fd == SYSCALL_FAIL_RETURN, cleanup(MID_OPENING); printf("%s", MAIN_RULE_TABLE_OPENNING_ATTRIBUTE_ERR_MSG);)
        
        MAIN_ERR_CHECK(write(fd, rule_table, (lst->size) * sizeof(rule_t)) == SYSCALL_FAIL_RETURN, cleanup(MID_OPENING); printf("%s", MAIN_RULE_TABLE_WRITING_ATTRIBUTE_ERR_MSG);)

        cleanup(MID_OPENING);
    }

    return EXIT_SUCCESS;
}