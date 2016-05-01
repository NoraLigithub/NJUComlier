#ifndef COMPILER_COMMON_H

#define COMPILER_COMMON_H
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "marco.h"


#define isSyntax(node, token) \
	((node != NULL)&&(strcmp((node)->name, str(token)) == 0))

typedef int bool;
#define true 1;
#define false 0; 

#endif
