#pragma once

#include "c4d.h"

#include "SettingsDialog.h"
#include "constants.h"

class Command_Settings : public CommandData
{
INSTANCEOF(Command_Settings, CommandData)

public:
	static maxon::Result<Command_Settings*> Alloc()
	{
		iferr_scope;
		return NewObj(Command_Settings) iferr_return;
	}

	Bool Execute(BaseDocument* doc) override
	{
		return _dlg.Open(DLG_TYPE::ASYNC, PID_IM, -1, -1, 400, 0);
	}

	Bool RestoreLayout(void* secret) override
	{
		return _dlg.RestoreLayout(PID_IM, 0, secret);
	}

private:
	SettingsDialog _dlg;
};
