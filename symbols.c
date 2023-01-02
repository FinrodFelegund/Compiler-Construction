#include "symbols.h"

void setvar(char *id, int nr)
{

	vars[(int)(*id)] = nr;

}

int getvar(char *id)
{

	return vars[(int)(*id)];

}
