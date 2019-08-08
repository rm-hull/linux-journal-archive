/*
 * dll_test.c : Test program for double link library
 *
 * Carl J. Nobile
 * Created: December 25, 1996
 * Updated: 06/15/97
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef	DEBUG
#  include "dll_dbg.h"		/* For debugging only */
#else
#  include "linklist.h"    /* For production */
#endif

#define Boolean	DLL_Boolean
#define FALSE		DLL_FALSE
#define TRUE		DLL_TRUE

/* Structures */
typedef struct name_addr
	{
	char name[30];
	char street[40];
	char city[22];
	char state[3];
	char zip[11];
	} NameAddr;

NameAddr put_addr, get_addr, find_addr;

/* Prototypes */
void delete_current_record(List *list);
int  direction_menu(void);
void display(NameAddr *set);
void display_all(List *list);
void enter(List *list);
void input(char *, char *, size_t);
int  field_menu(void);
int  find_name(NameAddr *record, NameAddr *match);
int  find_street(NameAddr *record, NameAddr *match);
int  find_city(NameAddr *record, NameAddr *match);
int  find_state(NameAddr *record, NameAddr *match);
int  find_zip(NameAddr *record, NameAddr *match);
void get_prior_record(List *list);
void get_next_record(List *list);
void get_first_record(List *list);
void get_last_record(List *list);
void load(List *list);
int  main_menu(void);
void number_of_records(List *list);
int  origin_menu(void);
void save(List *list);
void search(List *list);
int  search_menu(void);
int  sort_menu(void);
int  sort_name(NameAddr *record, NameAddr *match);
int  sort_state(NameAddr *record, NameAddr *match);
int  sort_zip(NameAddr *record, NameAddr *match);
void update(List *list);
int  update_select(void);
Boolean get_current_record(List *list);
Boolean verify_criteria(List *list, char *pField);


void main(void)
	{
	List *addr_list = NULL;
	DLL_Return ExitCode;

	if(DLL_CreateList(&addr_list) == NULL)
		{
		fputs("Fatal memory error", stderr);
		exit(EXIT_FAILURE);
		}

	if((ExitCode = DLL_InitializeList(addr_list, sizeof(NameAddr)))
	 != DLL_NORMAL)
		{
		(void)(ExitCode == DLL_ZERO_INFO
		 && fputs("Size of address record is zero.\n\n", stderr));
		(void)(ExitCode == DLL_NULL_LIST
		 && fputs("addr_list points to a NULL.\n\n", stderr));
		exit(EXIT_FAILURE);
		}

	printf("%s\n", DLL_Version());

	for(;;)
		{
		switch(main_menu())
			{
			case 1:
				enter(addr_list);
				break;
			case 2:
				update(addr_list);
				break;
			case 3:
				search(addr_list);
				break;
			case 4:
				display_all(addr_list);
				break;
			case 5:
				get_current_record(addr_list);
				break;
			case 6:
				delete_current_record(addr_list);
				break;
			case 7: 
				get_prior_record(addr_list);
				break;
			case 8:
				get_next_record(addr_list);
				break;
			case 9:
				get_first_record(addr_list);
				break;
			case 10:
				get_last_record(addr_list);
				break;
			case 11:
				save(addr_list);
				break;
			case 12:
				load(addr_list);
				break;
			case 13:
				number_of_records(addr_list);
				break;
			case 14:
				DLL_DestroyList(&addr_list);

#if	DEBUG
				if(addr_list != NULL)
					{
					fputs("Error: List was not destroyed.\n", stderr);
					exit(EXIT_FAILURE);
					}
#endif

				exit(EXIT_SUCCESS);
			}
		}
	}


/*
 * main_menu() : Select a menu option.
 */
int main_menu(void)
	{
	char s[80];
	int  c;

	fputs(" (1) Add an address             ", stdout);
	fputs(" (2) Update an address\n", stdout);
	fputs(" (3) Search for a record        ", stdout);
	fputs(" (4) List all records\n", stdout);
	fputs(" (5) Get current record         ", stdout);
	fputs(" (6) Delete current record\n", stdout);
	fputs(" (7) Get prior record           ", stdout);
	fputs(" (8) Get next record\n", stdout);
	fputs(" (9) Get first record           ", stdout);
	fputs("(10) Get last record\n", stdout);
	fputs("(11) Save to a file             ", stdout);
	fputs("(12) Load from a file\n", stdout);
	fputs("(13) Number of records in list  ", stdout);
	fputs("(14) Quit\n", stdout);

	do
		{
		fputs("\nEnter your choice: ", stdout);
		gets(s);
		c = atoi(s);
		}
	while(c < 0 || c > 14);

	fputs("\n", stdout);
	return(c);
	}


/*
 * enter() : Enter names and addresses.
 */
void enter(List *list)
	{
	int (*pfun)() = NULL;

	memset(&put_addr, '\0', sizeof(put_addr));

	switch(sort_menu())
		{
		case 1:
			pfun = sort_name;
			break;
		case 2:
			pfun = sort_state;
			break;
		case 3:
			pfun = sort_zip;
			break;
		case 4:
			pfun = NULL;
			break;
		}

	for(;;)
		{
		fputs("\nEnter a new item, or press <Enter> to return to menu.\n",
		 stdout);
		input("Enter name: ", put_addr.name, sizeof(put_addr.name));

		if(!put_addr.name[0])
			break;

		input("Enter Street: ", put_addr.street, sizeof(put_addr.street));
		input("Enter City: ", put_addr.city, sizeof(put_addr.city));
		input("Enter State: ", put_addr.state, sizeof(put_addr.state));
		input("Enter Zip: ", put_addr.zip, sizeof(put_addr.zip));

		if(DLL_AddRecord(list, &put_addr, pfun) == DLL_MEM_ERROR)
			{
			fputs("Fatal memory error", stderr);
			exit(EXIT_FAILURE);
			}
#if	DEBUG
		fputs("DEBUG MODE *** DATA DIRECTLY FROM LINK LIST ***\n", stderr);
		display(list->tail->info);
#endif
		}
	}


/*
 * sort_menu() :
 */
int sort_menu(void)
	{
	char s[81];
	int  c;

	fputs("Sort options\n", stdout);
	fputs("------------\n", stdout);
	fputs("(1) By Name\n", stdout);
	fputs("(2) By State\n", stdout);
	fputs("(3) By Zip\n", stdout);
	fputs("(4) No Sort\n", stdout);

	do
		{
		fputs("\nEnter your choice: ", stdout);
		gets(s);
		c = atoi(s);
		}
	while(c < 0 || c > 4);

	fputs("\n", stdout);
	return(c);
	}


/*
 * update() : Updates record(s) either current or a group
 */
void update(List *list)
	{
	char str[2];
	DLL_Return ExitCode;

	if((ExitCode = DLL_GetCurrentRecord(list, &get_addr)) != DLL_NORMAL)
		{
		(void)(ExitCode == DLL_NULL_LIST && fputs("List is empty.\n\n", stderr));
		return;
		}

	memcpy(&put_addr, &get_addr, sizeof(NameAddr));

	for(;;)
		{
		display(&put_addr);

		switch(update_select())
			{
			case 1:
				input("Enter Name: ", put_addr.name, sizeof(put_addr.name));
				break;
			case 2:
				input("Enter Address: ", put_addr.street, sizeof(put_addr.street));
				break;
			case 3:
				input("Enter City: ", put_addr.city, sizeof(put_addr.city));
				break;
			case 4:
				input("Enter State: ", put_addr.state, sizeof(put_addr.state));
				break;
			case 5:
				input("Enter Zip: ", put_addr.zip, sizeof(put_addr.zip));
				break;
			case 6:
				return;
			}

		strcpy(str, "N");
		input("Update another field? [y|n] ", str, sizeof(str));

		if(*str != 'y' && *str != 'Y')
			break;
		}

	if(DLL_UpdateCurrentRecord(list, &put_addr) != DLL_NORMAL)
		fputs("List is empty.\n\n", stderr);

	fputs("Record was updated.\n\n", stdout);
	get_current_record(list);
	}


/*
 * update_select()
 */
int update_select(void)
	{
	char s[80];
	int  c;

	fputs("(1) Name\n", stdout);
	fputs("(2) Street\n", stdout);
	fputs("(3) City\n", stdout);
	fputs("(4) State\n", stdout);
	fputs("(5) Zip\n", stdout);
	fputs("(6) No Update\n", stdout);

	do
		{
		fputs("\nEnter field to update choice: ", stdout);
		gets(s);
		c = atoi(s);
		}
	while(c < 0 || c > 6);

	fputs("\n", stdout);
	return(c);
	}


/*
 * input() : Prompt for input and get string.
 */
void input(char *prompt, char *s, size_t count)
	{
	char str[256];

	memset(str, '\0', sizeof(str));

	for(;;)
		{
		fputs(prompt, stdout);
		gets(str);

		if(strlen(str) < count)
			break;

		fputs("\nToo Long.\n\n", stderr);
		continue;
		}

	strcpy(s, str);
	}


/*
 * sort_name() : Find an address based on the persons name
 */
int sort_name(NameAddr *record, NameAddr *match)
	{
#ifdef	DEBUG
	fputs("DEBUG MODE *** DATA FROM sort_name() ***\n", stderr);
	fprintf(stderr, "record->name: %s\n match->name: %s\n",
	 record->name, match->name);
#endif

	return(strcmp(record->name, match->name));
	}


/*
 * sort_state() : Find an address based on the persons state
 */
int sort_state(NameAddr *record, NameAddr *match)
	{
#ifdef	DEBUG
	fputs("DEBUG MODE *** DATA FROM sort_state() ***\n", stderr);
	fprintf(stderr, "record->state: %s\n match->state: %s\n",
	 record->state, match->state);
#endif

	return(strcmp(record->state, match->state));
	}


/*
 * sort_zip() : Find an address based on the persons zip
 */
int sort_zip(NameAddr *record, NameAddr *match)
	{
#ifdef	DEBUG
	fputs("DEBUG MODE *** DATA FROM sort_zip() ***\n", stderr);
	fprintf(stderr, "record->zip: %s\n match->zip: %s\n",
	 record->zip, match->zip);
#endif

	return(strcmp(record->zip, match->zip));
	}


/*
 * display() : Prints fields of each address.
 */
void display(NameAddr *set)
	{
	printf("%s\n", set->name);
	printf("%s\n", set->street);
	printf("%s ", set->city);
	printf("%s  ", set->state);
	printf("%s\n", set->zip);
	fputs("\n", stdout);
	}


/*
 * search() : Search for a name in the list.
 */
void search(List *list)
	{
  	int (*pfun)() = NULL;
	Boolean done = FALSE, field = FALSE;
	char *pField = NULL;

	for(; !done;)
		{
		switch(search_menu())
			{
			case 1:		/* Choose search origin */
				switch(origin_menu())
					{
					case 1:		/* Originate at head */
						DLL_SetSearchModes(list, DLL_HEAD, DLL_DOWN);
						break;
					case 2:		/* Originate at current */
						DLL_SetSearchModes(list, DLL_CURRENT, DLL_DIRECTION_DEFAULT);
						break;
					case 3:		/* Originate at tail */
						DLL_SetSearchModes(list, DLL_TAIL, DLL_UP);
						break;
					}
				continue;
			case 2:		/* Choose search direction */
				switch(direction_menu())
					{
					case 1:		/* Direction down */
						DLL_SetSearchModes(list, DLL_ORIGIN_DEFAULT, DLL_DOWN);
						break;
					case 2:		/* Direction up */
						DLL_SetSearchModes(list, DLL_ORIGIN_DEFAULT, DLL_UP);
						break;
					}
				continue;
			case 3:		/* Choose search field */
				switch(field_menu())
					{
					case 1:		/* Name */
						input("Enter name fragment: ", find_addr.name,
						 sizeof(find_addr.name));
						pField = find_addr.name;
						pfun = find_name;
							break;
					case 2:		/* Street */
						input("Enter street fragment: ", find_addr.street,
						 sizeof(find_addr.street));
						pField = find_addr.street;
						fputs("\n", stdout);
						break;
					case 3:		/* City */
						input("Enter city fragment: ", find_addr.city,
						 sizeof(find_addr.city));
						pField = find_addr.city;
						pfun = find_city;
						break;
					case 4:		/* State */
						input("Enter state fragment: ", find_addr.state,
						 sizeof(find_addr.state));
						pField = find_addr.state;
						pfun = find_state;
						break;
					case 5:		/* Zip */
						input("Enter zip fragment: ", find_addr.zip,
						 sizeof(find_addr.zip));
						pField = find_addr.zip;
						pfun = find_zip;
					}

				fputs("\n", stdout);
				field = TRUE;
				continue;
			case 4:		/* Do the search */
				if(!field)
					{
					fputs("No field has been set, please set one.\n\n", stdout);
					continue;
					}
				if(verify_criteria(list, pField) == FALSE)
					return;

				done = TRUE;
				break;
			default:
				return;
			}

		fputs("\n", stdout);
		}

	switch(DLL_FindRecord(list, &get_addr, &find_addr, (int (*)()) pfun))
		{
		case DLL_NORMAL:
			display(&get_addr);
			break;
		case DLL_NULL_LIST:
			fputs("List is empty.\n\n", stderr);
			break;
		case DLL_NOT_FOUND:
			fputs("Record not found.\n\n", stderr);
			break;
		case DLL_NULL_FUNCTION:
			fputs("Function pfun is NULL.\n\n", stderr);
			break;
		default:
			fputs("Unknown return type.\n\n", stderr);
			break;
		}
	}


/*
 * search_menu() : Setup search menu
 */
int search_menu(void)
	{
	char s[80];
	int  c;

	fputs("(1) Choose search origin\n", stdout);
	fputs("(2) Choose search direction\n", stdout);
	fputs("(3) Choose search field\n", stdout);
	fputs("(4) Start search\n", stdout);

	do
		{
		fputs("\nSet search parameter: ", stdout);
		gets(s);
		c = atoi(s);
		}
	while(c < 0 || c > 4);

	fputs("\n", stdout);
	return(c);
	} 


/*
 * origin_menu() : Setup origin menu
 */
int origin_menu(void)
	{
	char s[80];
	int  c;

	fputs("(1) Originate at head\n", stdout);
	fputs("(2) Originate at current\n", stdout);
	fputs("(3) Originate at tail\n", stdout);

	do
		{
		fputs("\nChoose origin of search: ", stdout);
		gets(s);
		c = atoi(s);
		}
	while(c < 0 || c > 3);

	fputs("\n", stdout);
	return(c);
	} 


/*
 * direction_menu() : Setup search menu
 */
int direction_menu(void)
	{
	char s[80];
	int  c;

	fputs("(1) Direction down\n", stdout);
	fputs("(2) Direction up\n", stdout);

	do
		{
		fputs("\nChoose direction of search: ", stdout);
		gets(s);
		c = atoi(s);
		}
	while(c < 0 || c > 2);

	fputs("\n", stdout);
	return(c);
	}


/*
 * verify_criteria() : Verify the search criteria
 */
Boolean verify_criteria(List *list, char *pField)
	{
	char yn[2], org[19], dir[22];
	DLL_SearchModes *SearchModes;

	strcpy(yn, "N");
	SearchModes = DLL_GetSearchModes(list);

	switch(SearchModes->search_origin)
		{
		case DLL_ORIGIN_DEFAULT:
			strcpy(org, "DLL_ORIGIN_DEFAULT");
			break;
		case DLL_HEAD:
			strcpy(org, "DLL_HEAD");
			break;
		case DLL_CURRENT:
			strcpy(org, "DLL_CURRENT");
			break;
		case DLL_TAIL:
			strcpy(org, "DLL_TAIL");
			break;
		}

	switch(SearchModes->search_dir)
		{
		case DLL_DIRECTION_DEFAULT:
			strcpy(dir, "DLL_DIRECTION_DEFAULT");
			break;
		case DLL_DOWN:
			strcpy(dir, "DLL_DOWN");
			break;
		case DLL_UP:
			strcpy(dir, "DLL_UP");
			break;
		}

	fprintf(stdout, "%s\n%s%s\n%s%s\n%s%s\n\n",
	 "Search modes are set to:",
	 "     Origin: ", org,
	 "  Direction: ", dir,
	 "Search with: ", pField);
	input("Continue with search? [y|n] ", yn, sizeof(yn));

	if(*yn != 'y' && *yn != 'Y')
		return(FALSE);

	return(TRUE);
	}


/*
 * field_menu() : Setup direction menu
 */
int field_menu(void)
	{
	char s[80];
	int  c;

	fputs("(1) Name field\n", stdout);
	fputs("(2) Street field\n", stdout);
	fputs("(3) City field\n", stdout);
	fputs("(4) State field\n", stdout);
	fputs("(5) Zip field\n", stdout);

	do
		{
		fputs("\nChoose field to search on: ", stdout);
		gets(s);
		c = atoi(s);
		}
	while(c < 0 || c > 5);

	fputs("\n", stdout);
	return(c);
	} 


/*
 * find_name() : Find occurance of substring in name string.
 */
int find_name(NameAddr *record, NameAddr *match)
	{
	char *p;

	if((p = strstr(record->name, match->name)) == NULL)
		return(1);
#ifdef	DEBUG
	fputs("DEBUG MODE *** DATA FROM find_name() ***\n", stderr);
	fprintf(stderr, "record->name: %s\n match->name: %s\np: %p\n\n",
	 record->name, match->name, p);
#endif

	return(0);
	}


/*
 * find_street() : Find occurance of substring in street string.
 */
int find_street(NameAddr *record, NameAddr *match)
	{
	char *p;

	if((p = strstr(record->street, match->street)) == NULL)
		return(1);
#ifdef	DEBUG
	fputs("DEBUG MODE *** DATA FROM find_street() ***\n", stderr);
	fprintf(stderr, "record->street: %s\n match->street: %s\np: %p\n\n",
	 record->street, match->street, p);
#endif

	return(0);
	}


/*
 * find_city() : Find occurance of substring in city string.
 */
int find_city(NameAddr *record, NameAddr *match)
	{
	char *p;

	if((p = strstr(record->city, match->city)) == NULL)
		return(1);
#ifdef	DEBUG
	fputs("DEBUG MODE *** DATA FROM find_city() ***\n", stderr);
	fprintf(stderr, "record->city: %s\n match->city: %s\np: %p\n\n",
	 record->city, match->city, p);
#endif

	return(0);
	}


/*
 * find_state() : Find occurance of substring in state string.
 */
int find_state(NameAddr *record, NameAddr *match)
	{
	char *p;

	if((p = strstr(record->state, match->state)) == NULL)
		return(1);
#ifdef	DEBUG
	fputs("DEBUG MODE *** DATA FROM find_state() ***\n", stderr);
	fprintf(stderr, "record->state: %s\n match->state: %s\np: %p\n\n",
	 record->state, match->state, p);
#endif

	return(0);
	}


/*
 * find_zip() : Find occurance of substring in zip string.
 */
int find_zip(NameAddr *record, NameAddr *match)
	{
	char *p;

	if((p = strstr(record->zip, match->zip)) == NULL)
		return(1);
#ifdef	DEBUG
	fputs("DEBUG MODE *** DATA FROM find_zip() ***\n", stderr);
	fprintf(stderr, "record->zip: %s\n match->zip: %s\np: %p\n\n",
	 record->zip, match->zip, p);
#endif

	return(0);
	}


/*
 * display_all() : Display all records
 */
void display_all(List *list)
	{
	DLL_StoreCurrentPointer(list);

	if(DLL_CurrentPointerToHead(list) != DLL_NORMAL)
		{
		fputs("List is empty.\n\n", stderr);
		return;
		}

	do
		{
		DLL_GetCurrentRecord(list, &get_addr);
		printf("%s, %s, %s, %s, %s\n", get_addr.name, get_addr.street,
		 get_addr.city, get_addr.state, get_addr.zip);
		}
	while(DLL_IncrementCurrentPointer(list) == DLL_NORMAL);

	DLL_RestoreCurrentPointer(list);
	fputs("\n", stdout);
	}


/*
 * number_of_records() : Returns the number of total records in list
 */
void number_of_records(List *list)
	{
	printf("The total number of addresses in this list is: %ld\n\n",
	 DLL_GetNumberOfRecords(list));
	}


/*
 * get_current_record() :
 */
Boolean get_current_record(List *list)
	{
	DLL_Return ExitCode;

	if((ExitCode = DLL_GetCurrentRecord(list, &get_addr)) != DLL_NORMAL)
		{
		(void)(ExitCode == DLL_NULL_LIST && fputs("List is empty.\n\n", stderr));
		return(FALSE);
		}

	display(&get_addr);
	return(TRUE);
	}


/*
 * get_prior_record() :
 */
void get_prior_record(List *list)
	{
	DLL_Return ExitCode;

	if((ExitCode = DLL_GetPriorRecord(list, &get_addr)) != DLL_NORMAL)
		{
		(void)(ExitCode == DLL_NULL_LIST && fputs("List is empty.\n\n", stderr));
		(void)(ExitCode == DLL_NOT_FOUND && fputs("At beginning of list.\n\n",
		 stderr));
		return;
		}

	display(&get_addr);
	}


/*
 * get_next_record() :
 */
void get_next_record(List *list)
	{
	DLL_Return ExitCode;

	if((ExitCode = DLL_GetNextRecord(list, &get_addr)) != DLL_NORMAL)
		{
		(void)(ExitCode == DLL_NULL_LIST
		 && fputs("List is empty.\n\n", stderr));
		(void)(ExitCode == DLL_NOT_FOUND
		 && fputs("At end of list.\n\n", stderr));
		return;
		}

	display(&get_addr);
	}


/*
 * get_first_record() :
 */
void get_first_record(List *list)
	{
	if(DLL_CurrentPointerToHead(list) != DLL_NORMAL)
		{
		fputs("List is empty.\n\n", stderr);
		return;
		}

	get_current_record(list);
	}


/*
 * get_last_record() :
 */
void get_last_record(List *list)
	{
	if(DLL_CurrentPointerToTail(list) != DLL_NORMAL)
		{
		fputs("List is empty.\n\n", stderr);
		return;
		}

	get_current_record(list);
	}


/*
 * delete_current_record() :
 */
void delete_current_record(List *list)
	{
	char str[2];

	if(!get_current_record(list))
		return;

	strcpy(str, "N");
	input("Are you sure you want to delete this record? [y|n] ",
	 str, sizeof(str));

	if(*str != 'y' && *str != 'Y')
		{
		fputs("Record was not deleted.\n\n", stdout);
		return;
		}

	if(DLL_DeleteCurrentRecord(list) != DLL_NORMAL)
		{
		fputs("List is empty.\n\n", stderr);
		return;
		}

	fputs("Record was deleted.\n\n", stdout);
	}


/*
 * save() : Save file to disk.
 */
void save(List *list)
	{
	char pathname[] = "test.dat";
	DLL_Return ExitCode;

	if((ExitCode = DLL_SaveList(list, pathname)) != DLL_NORMAL)
		{
		(void)(ExitCode == DLL_NULL_LIST && fputs("List is empty.\n\n", stderr));
		(void)(ExitCode == DLL_OPEN_ERROR && fputs("Cannot open file.\n\n",
		 stderr));
		(void)(ExitCode == DLL_WRITE_ERROR && fputs("Cannot write to file.\n\n",
		 stderr));
		(void)(ExitCode == DLL_NOT_MODIFIED &&
		 fputs("List has not changed, no save was done.\n\n", stderr));
		return;
		}

	fputs("List was saved.\n\n", stderr);
	}


/*
 * load() : Load address file.
 */
void load(List *list)
	{
	char pathname[] = "test.dat";
	DLL_Return ExitCode;
	int (*pfun)() = NULL;

	switch(sort_menu())
		{
		case 1:
			pfun = sort_name;
			break;
		case 2:
			pfun = sort_state;
			break;
		case 3:
			pfun = sort_zip;
			break;
		case 4:
			pfun = NULL;
			break;
		}

	if((ExitCode = DLL_LoadList(list, pathname, pfun)) != DLL_NORMAL)
		{
		(void)(ExitCode == DLL_MEM_ERROR && fputs("Fatal memory error.\n\n",
		 stderr));
		(void)(ExitCode == DLL_NULL_LIST && fputs("List is empty.\n\n", stderr));
		(void)(ExitCode == DLL_OPEN_ERROR && fputs("Cannot open file.\n\n",
		 stderr));
		(void)(ExitCode == DLL_READ_ERROR && fputs("Read file error.\n\n",
		 stderr));
		}
	}
