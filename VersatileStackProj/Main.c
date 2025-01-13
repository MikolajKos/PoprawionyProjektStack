#include "stdafx.h"
#include "cli_mess.h"
#include "stack.h"
#include "MY_STUDENT.h"
#include "user_interface.h"

//SF
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

int main() {
	UserMenu();

	//SF
	if (_CrtDumpMemoryLeaks())
		printf("LEAK OF MEMORY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	return 0;
}