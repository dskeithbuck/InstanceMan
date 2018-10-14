#include "c4d.h"

#include "constants.h"
#include "Command_Select.h"
#include "Command_Create.h"

Bool PluginStart()
{
	if(!RegisterCommandPlugin(PID_IM_SELECT, String(PNAME_IM_SELECT), 0, nullptr, String(PHELP_IM_SELECT), Command_Select::Alloc()))
		return false;
	if(!RegisterCommandPlugin(PID_IM_CREATE, String(PNAME_IM_CREATE), 0, nullptr, String(PHELP_IM_CREATE), Command_Create::Alloc()))
		return false;
	
		
	return true;
}

Bool PluginMessage(Int32 id, void* data)
{
	switch (id)
	{
	case C4DPL_INIT_SYS:
		{
			return g_resource.Init();
		}
	}
	return true;
}

void PluginEnd()
{
}

