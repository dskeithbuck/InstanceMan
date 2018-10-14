#pragma once

#include "c4d.h"

class Command_Create: public CommandData
{
public:
	Bool Execute(BaseDocument* doc) override
	{
		return true;
	}

	static Command_Create* Alloc()
	{
		return NewObjClear(Command_Create);
	}
};