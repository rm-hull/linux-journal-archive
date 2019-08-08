/*
 * dll_main.c : An API for a double linked list.
 *
 * Carl J. Nobile
 * Created: December 22, 1996
 * Updated: 12/12/97
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dll_main.h"


/*
 * DLL_CreateList() : Creates a structure of type List
 *
 * Status   : Public
 *
 * Arguments: name -- Pointer to a pointer to a name of structure to create
 *
 * Returns  : Pointer to created structure
 *            NULL if unsuccessfull
 */
List *DLL_CreateList(List **name)
	{
	if((*name = (List *) malloc(sizeof(List))) == NULL)
		return(NULL);

	return(*name);
	}


/*
 * DLL_DestroyList() : Destroys Info, Node, and List structures
 *
 * Status   : Public
 *
 * Arguments: name -- Pointer to a pointer to a name of structure to destroy
 *
 * Returns  : NONE
 */
void DLL_DestroyList(List **name)
	{
	DLL_DeleteEntireList(*name);
	free(*name);
	*name = NULL;
	}


/*
 * DLL_InitializeList() : Initializes double link list
 *
 * Status   : Public
 *
 * Arguments: list          -- Pointer to type List
 *            infosize      -- Size of user Info
 *
 * Returns  : DLL_NORMAL    -- Initialization was done successfully
 *            DLL_ZERO_INFO -- sizeof(Info) is zero
 *            DLL_NULL_LIST -- Info is NULL
 */
DLL_Return DLL_InitializeList(List *list, size_t infosize)
	{
	if(infosize == (size_t) 0)
		return(DLL_ZERO_INFO);

	if(list == NULL)
		return(DLL_NULL_LIST);

	list->head = NULL;
	list->tail = NULL;
	list->current = NULL;
	list->saved = NULL;
	list->infosize = infosize;
	list->listsize = 0L;
	list->modified = DLL_FALSE;
	list->search_origin = DLL_HEAD;
	list->search_dir = DLL_DOWN;
	return(DLL_NORMAL);
	}


/*
 * DLL_IsListEmpty() : Checks for an empty list
 *
 * Status   : Public
 *
 * Arguments: list      -- Pointer to type List
 *
 * Returns  : DLL_TRUE  -- List is empty
 *            DLL_FALSE -- List has items in it
 */
DLL_Boolean DLL_IsListEmpty(List *list)
	{
	if(list->head == NULL || list->tail == NULL)
		return(DLL_TRUE);

	return(DLL_FALSE);
	}


/*
 * DLL_IsListFull() : Checks for an empty list
 *
 * Status   : Public
 *
 * Arguments: list      -- Pointer to type List
 *
 * Returns  : DLL_TRUE  -- List is full (memory dependent)
 *            DLL_FALSE -- List is empty or partially full
 */
DLL_Boolean DLL_IsListFull(List *list)
	{
	Node *newN;
	Info *newI;

	if((newN = (Node *) malloc(sizeof(Node))) == NULL)
		return(DLL_TRUE);

	if((newI = (Info *) malloc(list->infosize)) == NULL)
		{
		free(newN);
		return(DLL_TRUE);
		}

	free(newN);
	free(newI);
	return(DLL_FALSE);
	}


/*
 * DLL_CurrentPointerToHead() : Moves the current pointer to
 *                              the head of the list.
 * Status   : Public
 *
 * Arguments: list          -- Pointer to type List
 *
 * Returns  : DLL_NORMAL    -- Record found
 *            DLL_NULL_LIST -- Empty list
 */ 
DLL_Return DLL_CurrentPointerToHead(List *list)
	{
	if(list->head == NULL)
		return(DLL_NULL_LIST);

	list->current = list->head;
	return(DLL_NORMAL);
	}


/*
 * DLL_CurrentPointerToTail() : Moves the current pointer to
 *                              the tail of the list.
 * Status   : Public
 *
 * Arguments: list          -- Pointer to type List
 *
 * Returns  : DLL_NORMAL    -- Record found
 *            DLL_NULL_LIST -- Empty list
 */ 
DLL_Return DLL_CurrentPointerToTail(List *list)
	{
	if(list->tail == NULL)
		return(DLL_NULL_LIST);

	list->current = list->tail;
	return(DLL_NORMAL);
	}


/*
 * DLL_IncrementCurrentPointer() : Moves the current pointer to next position.
 *
 * Status   : Public
 *
 * Arguments: list          -- Pointer to type List
 *
 * Returns  : DLL_NORMAL    -- Record found
 *            DLL_NULL_LIST -- Empty list
 *            DLL_NOT_FOUND -- Record not found
 */ 
DLL_Return DLL_IncrementCurrentPointer(List *list)
	{
	if(list->current == NULL)
		return(DLL_NULL_LIST);

	if(list->current->next == NULL)
		return(DLL_NOT_FOUND);

	list->current = list->current->next;
	return(DLL_NORMAL);
	}


/*
 * DLL_DecrementCurrentPointer() : Moves the current pointer to last position.
 *
 * Status   : Public
 *
 * Arguments: list          -- Pointer to type List
 *
 * Returns  : DLL_NORMAL    -- Record found
 *            DLL_NULL_LIST -- Empty list
 *            DLL_NOT_FOUND -- Record not found
 */ 
DLL_Return DLL_DecrementCurrentPointer(List *list)
	{
	if(list->current == NULL)
		return(DLL_NULL_LIST);

	if(list->current->prior == NULL)
		return(DLL_NOT_FOUND);

	list->current = list->current->prior;
	return(DLL_NORMAL);
	}


/*
 * DLL_StoreCurrentPointer() : Saves the current pointer.
 *
 * Status   : Public
 *
 * Arguments: list          -- Pointer to type List
 *
 * Returns  : DLL_NORMAL    -- Record found
 *            DLL_NOT_FOUND -- Record not found
 */ 
DLL_Return DLL_StoreCurrentPointer(List *list)
	{
	if(list->current == NULL)
		return(DLL_NOT_FOUND);

	list->saved = list->current;
	return(DLL_NORMAL);
	}


/*
 * DLL_RestoreCurrentPointer() : Loads the previously saved current pointer.
 *
 * Status   : Public
 *
 * Arguments: list          -- Pointer to type List
 *
 * Returns  : DLL_NORMAL    -- Record found
 *            DLL_NOT_FOUND -- Record not found
 */ 
DLL_Return DLL_RestoreCurrentPointer(List *list)
	{
	if(list->saved == NULL)
		return(DLL_NOT_FOUND);

	list->current = list->saved;
	list->saved = NULL;
	return(DLL_NORMAL);
	}


/*
 * DLL_GetNumberOfRecords() : Return number of records.
 *
 * Status   : Public
 *
 * Arguments: list -- Pointer to type List
 *
 * Returns  : Number of records
 */
unsigned long DLL_GetNumberOfRecords(List *list)
	{
	return list->listsize;
	}


/*
 * DLL_AddRecord() : Creates a new node in list with or without sorting.
 *
 * Status   : Public
 *
 * Arguments: list          -- Pointer to type List
 *            info          -- Record to add
 *            pFun          -- Pointer to search function
 *
 * Returns  : DLL_NORMAL    -- Node was added successfully
 *            DLL_MEM_ERROR -- Memory allocation failed
 */
DLL_Return DLL_AddRecord(List *list, Info *info, int (*pFun)(Info *, Info *))
	{
	Node *newN, *old, *step;
	Info *newI;

	if((newN = (Node *) malloc(sizeof(Node))) == NULL)
		return(DLL_MEM_ERROR);

	if((newI = (Info *) malloc(list->infosize)) == NULL)
		return(DLL_MEM_ERROR);

	memcpy(newI, info, list->infosize);

	if(!list->head)
		{
		newN->info = newI;
		newN->next = NULL;
		newN->prior = NULL;
		list->head = newN;
		list->tail = newN;
		list->current = newN;
		}
	else
		{
		if(pFun)						/* If NULL don't do sort */
			{
			step = list->head;
			old = list->tail;

			while(step)				/* Loop through records until a match is found */
				{
				if(((*pFun)(step->info, newI)) >= 0)
					break;

				old = step;
				step = (Node *) step->next;
				}
			}
		else
			{
			step = NULL;
			old = list->tail;
			}

		/*
		 * The order of the 'if' statements below is critical and
		 * cannot be changed or a no sort (NULL) situation will fail.
		 */
		if(!step)					/* New last record */
			{
			newN->info = newI;
			old->next = newN;
			newN->next = NULL;
			newN->prior = old;
			list->tail = newN;
			list->current = newN;
			}
		else
			if(!step->prior)		/* New first record */
				{
				newN->info = newI;
				newN->prior = NULL;
				newN->next = step;
				step->prior = newN;
				list->head = newN;
				list->current = newN;
				}
			else						/* New middle record */
				{
				newN->info = newI;
				step->prior->next = newN;
				newN->next = step;
				newN->prior = step->prior;
				step->prior = newN;
				list->current = newN;
				}
		}

	list->listsize++;
	list->modified = DLL_TRUE;
	return(DLL_NORMAL);
	}


/*
 * DLL_UpdateCurrentRecord() : Updates current record
 *
 * Status   : Public
 *
 * Arguments: list          -- Pointer to type List
 *            record        -- Pointer to an Info structure in list
 *
 * Returns  : DLL_NORMAL    -- Record updated
 *            DLL_NULL_LIST -- Empty list
 */
DLL_Return DLL_UpdateCurrentRecord(List *list, Info *record)
	{
	if(list->current == NULL)
		return(DLL_NULL_LIST);

	memcpy(list->current->info, record, list->infosize);
	return(DLL_NORMAL);
	}


/*
 * DLL_FindRecord() : Find a record in list
 *
 * Status   : Public
 *
 * Arguments: list              -- Pointer to type List
 *            record            -- Pointer to an Info structure in list
 *            match             -- Pointer to an Info structure to match
 *                                 to list
 *            pFun              -- Pointer to search function
 *
 * Returns  : DLL_NORMAL        -- Record found
 *            DLL_NULL_LIST     -- Empty list
 *            DLL_NOT_FOUND     -- Record not found
 *            DLL_NULL_FUNCTION -- pFun is NULL
 */
DLL_Return DLL_FindRecord(List *list, Info *record, Info *match,
 int (*pFun)(Info *, Info *))
	{
	Node *step;
	register DLL_SrchDir dir;

	if(pFun == NULL)
		return(DLL_NULL_FUNCTION);

	switch(list->search_origin)
		{
		case DLL_CURRENT:
			step = list->current;
			dir = list->search_dir;
			break;
		case DLL_TAIL:
			step = list->tail;
			list->search_dir = dir = DLL_UP;
			break;
		case DLL_HEAD:
		default:
			list->search_origin = DLL_HEAD;
			step = list->head;
			list->search_dir = dir = DLL_DOWN;
		}

	if(step == NULL)
		return(DLL_NULL_LIST);

	while(step != NULL)
		{
		if(((*pFun)(step->info, match)) == 0)
			{
			memcpy(record, step->info, list->infosize);
			list->current = step;
			return(DLL_NORMAL);
			}

		step = (dir == DLL_DOWN) ? (Node *) step->next : (Node *) step->prior;
		}

	return(DLL_NOT_FOUND);
	}


/*
 * DLL_SetSearchModes() : Sets the pointer used to start a search origin
 *                        and the direction indicator.
 *
 * Status   : Public
 *
 * Arguments: list             -- Pointer to type List
 *            origin           -- Indicates the start search pointer to use
 *            dir              -- Indicates the direction to search in
 *
 * Returns  : DLL_NORMAL       -- Values assigned were accepted
 *            DLL_NOT_MODIFIED -- Values were not assigned--invalid type
 *                                (defaults are still in place)
 */
DLL_Return DLL_SetSearchModes(List *list, DLL_SrchOrigin origin,
 DLL_SrchDir dir)
	{
	switch(origin)
		{
		case DLL_HEAD:
		case DLL_CURRENT:
		case DLL_TAIL:
		case DLL_ORIGIN_DEFAULT:
			break;
		default:
			return(DLL_NOT_MODIFIED);
		}

	switch(dir)
		{
		case DLL_DOWN:
		case DLL_UP:
		case DLL_DIRECTION_DEFAULT:
			break;
		default:
			return(DLL_NOT_MODIFIED);
		}

	if(origin != DLL_ORIGIN_DEFAULT)
		list->search_origin = origin;

	if(dir != DLL_DIRECTION_DEFAULT)
		list->search_dir = dir;

	return(DLL_NORMAL);
	}


/*
 * DLL_GetSearchModes() : Returns the search modes
 *
 * Status   : Public
 *
 * Arguments: list -- Pointer to type List
 *
 * Returns  : Pointer to type DLL_SearchModes
 */
DLL_SearchModes *DLL_GetSearchModes(List *list)
	{
	static DLL_SearchModes SM;

	SM.search_origin = list->search_origin;
	SM.search_dir = list->search_dir;
	return(&SM);
	}


/*
 * DLL_GetCurrentRecord() : Gets the record pointed to by current
 *
 * Status   : Public
 *
 * Arguments: list          -- Pointer to type List
 *            record        -- Pointer to a pointer to an Info structure
 *
 * Returns  : DLL_NORMAL    -- Record returned
 *            DLL_NULL_LIST -- List is empty
 */
DLL_Return DLL_GetCurrentRecord(List *list, Info *record)
	{
	if(list->current == NULL)
		return(DLL_NULL_LIST);

	memcpy(record, list->current->info, list->infosize);
	return(DLL_NORMAL);
	}


/*
 * DLL_GetPriorRecord() : Gets the record pointed to by current->prior
 *
 * Status   : Public
 *
 * Arguments: list          -- Pointer to type List
 *            record        -- Pointer to a pointer to an Info structure
 *
 * Returns  : DLL_NORMAL    -- Record returned
 *            DLL_NULL_LIST -- List is empty
 *            DLL_NOT_FOUND -- Beginning of list
 */
DLL_Return DLL_GetPriorRecord(List *list, Info *record)
	{
	if(list->current == NULL)
		return(DLL_NULL_LIST);

	if(list->current->prior == NULL)
		return(DLL_NOT_FOUND);

	list->current = list->current->prior;
	memcpy(record, list->current->info, list->infosize);
	return(DLL_NORMAL);
	}


/*
 * DLL_GetNextRecord() : Gets the record pointed to by current->next
 *
 * Status   : Public
 *
 * Arguments: list          -- Pointer to type List
 *            record        -- Pointer to a pointer to an Info structure
 *
 * Returns  : DLL_NORMAL    -- Record returned
 *            DLL_NULL_LIST -- List is empty
 *            DLL_NOT_FOUND -- End of list
 */
DLL_Return DLL_GetNextRecord(List *list, Info *record)
	{
	if(list->current == NULL)
		return(DLL_NULL_LIST);

	if(list->current->next == NULL)
		return(DLL_NOT_FOUND);

	list->current = list->current->next;
	memcpy(record, list->current->info, list->infosize);
	return(DLL_NORMAL);
	}


/*
 * DLL_DeleteCurrentRecord() : Delete a record from the list.
 *
 * Status   : Public
 *
 * Arguments: list          -- Pointer to type List
 *
 * Returns  : DLL_NORMAL    -- Record deleted
 *            DLL_NULL_LIST -- List is empty
 */
DLL_Return DLL_DeleteCurrentRecord(List *list)
	{
	Info *oldI;
	Node *oldN;

	if(list->current == NULL)
		return(DLL_NULL_LIST);

	oldI = list->current->info;
	oldN = list->current;

	if(list->current == list->head)		/* current is first record */
		{
		/* A single record in a list can't do this ...next->prior */
		if(list->current->next != NULL)
			list->current->next->prior = NULL;

		list->head = list->current->next;
		list->current = list->head;
		}
	else
		if(list->current == list->tail)	/* current is last record */
			{
			list->current->prior->next = NULL;
			list->tail = list->current->prior;
			list->current = list->tail;
			}
		else										/* current is a middle record */
			{
			list->current->prior->next = list->current->next;
			list->current->next->prior = list->current->prior;
			list->current = list->current->next;
			}
			
	free(oldI);
	free(oldN);
	list->listsize--;
	list->modified = DLL_TRUE;
	return(DLL_NORMAL);
	}


/*
 * DLL_DeleteEntireList() : Delete the entire list.
 *
 * Status   : Public
 *
 * Arguments: list          -- Pointer to type List
 *
 * Returns  : DLL_NORMAL    -- List deleted
 *            DLL_NULL_LIST -- List is empty
 */
DLL_Return DLL_DeleteEntireList(List *list)
	{
	Info *oldI;
	Node *oldN;

	if(list->head == NULL)
		return(DLL_NULL_LIST);

	do
		{
		oldI = list->head->info;
		oldN = list->head;
		list->head = list->head->next;
		free(oldI);
		free(oldN);
		}
	while(list->head != NULL);

	list->head = NULL;
	list->tail = NULL;
	list->current = NULL;
	list->saved = NULL;
	list->listsize = 0L;
	list->modified = DLL_TRUE;
	list->search_origin = DLL_HEAD;
	list->search_dir = DLL_DOWN;
	return(DLL_NORMAL);
	}


/*
 * DLL_SaveList() : Save list to disk
 *
 * Status   : Public
 *
 * Arguments: list             -- Pointer to type List
 *            path             -- Pointer to path and filename
 *
 * Return   : DLL_NORMAL       -- File written successfully
 *            DLL_NULL_LIST    -- List is empty
 *            DLL_OPEN_ERROR   -- File open error
 *            DLL_WRITE_ERROR  -- File write error
 *            DLL_NOT_MODIFIED -- Unmodified list no save was done
 */
DLL_Return DLL_SaveList(List *list, const char *path)
	{
	Node *step;
	FILE *fp;

	if(list->head == NULL)
		return(DLL_NULL_LIST);

	if(list->modified == DLL_FALSE)
		return(DLL_NOT_MODIFIED);

	if((fp = fopen(path, "wb")) == NULL)
		return(DLL_OPEN_ERROR);

	step = list->head;

	while(step != NULL)
		{
		if(fwrite(step->info, 1, list->infosize, fp) != list->infosize)
			{
			fclose(fp);
			return(DLL_WRITE_ERROR);
			}

		step = (Node *) step->next;
		}

	fclose(fp);
	list->modified = DLL_FALSE;
	return(DLL_NORMAL);
	}


/*
 * DLL_LoadList() : Load list to disk
 *
 * Status   : Public
 *
 * Arguments: list           -- Pointer to type List
 *            path           -- Pointer to path and filename
 *            pFun           -- Pointer to search function
 *
 * Return   : DLL_NORMAL     -- File written successfully
 *            DLL_MEM_ERROR  -- Memory allocation failed
 *            DLL_OPEN_ERROR -- File open error
 *            DLL_READ_ERROR -- File read error
 */
DLL_Return DLL_LoadList(List *list, const char *path,
 int (*pFun)(Info *, Info *))
	{
	Info *set;
	FILE *fp;
	DLL_Return ExitCode;

	if((fp = fopen(path, "rb")) == NULL)
		return(DLL_OPEN_ERROR);

	DLL_DeleteEntireList(list);

	list->head = list->tail = NULL;

	if((set = (Info *) malloc(list->infosize)) == NULL)
		return(DLL_MEM_ERROR);

	for(;;)
		{
		if(fread(set, 1, list->infosize, fp) != list->infosize)
			{
			if(feof(fp))
				ExitCode = DLL_NORMAL;
			else
				ExitCode = DLL_READ_ERROR;

			break;
			}

		if((ExitCode = DLL_AddRecord(list, set, pFun)) == DLL_MEM_ERROR)
			break;
		}

	if(!pFun)
		list->modified = DLL_FALSE;

	free(set);
	fclose(fp);
	return(ExitCode);
	}


/*
 * DLL_Version() : Returns a pointer to version information
 *
 * Status   : Public
 *
 * Arguments: NONE
 *
 * Return   : char * -- Pointer to version info
 */
char *DLL_Version(void)
	{
	strcpy(version, VERSION);
	strcat(version, "  ");
	strcat(version, VERDATE);
	strcat(version, "\n");
	strcat(version, CREDITS);
	strcat(version, "\n");
	return(version);
	}
