# include "ffscript.h"

#ifndef _FFASM_H_
#define _FFASM_H_

#include "zdefs.h"
#include <utility>
#include <string>
#include <list>
#include "zelda.h"
#include "zasm.h"

//What are these for exactly?
//#define fflong(x,y,z)       (((x[(y)][(z)])<<24)+((x[(y)][(z)+1])<<16)+((x[(y)][(z)+2])<<8)+(x[(y)][(z)+3]))
//#define ffword(x,y,z)       (((x[(y)][(z)])<<8)+(x[(y)][(z)+1]))

int parse_script(zasm::script& script);
int parse_script_file(zasm::script& script, const char *path, bool report_success);
long ffparse(char *string);

#endif

