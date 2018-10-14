#pragma once

#include "c4d.h"
#include "constants.h"

class Command_FrameObjects : public CommandData
{
INSTANCEOF(Command_FrameObjects,CommandData)

public:
	static Command_FrameObjects* Alloc()
	{
		return NewObjClear(Command_FrameObjects);
	}

	Bool Execute(BaseDocument* doc) override
	{
		if (doc == nullptr)
			return false;

		// Create Array that holds all objects to operate on
		const AutoAlloc<AtomArray> activeObjects;
		doc->GetActiveObjects(activeObjects, GETACTIVEOBJECTFLAGS::SELECTIONORDER | GETACTIVEOBJECTFLAGS::CHILDREN);

		// Allocation failed
		if (activeObjects == nullptr)
			return false;

		// GeData container to store individual instance mode
		GeData data;
		for (auto i = 0; i < activeObjects->GetCount(); ++i)
		{
			const auto obj = static_cast<BaseObject*>(activeObjects->GetIndex(i));
			if(obj->GetType() == Oinstance)
			{
				// Save the current instance mode and set it to normal instances
				obj->GetParameter(DescID(INSTANCEOBJECT_RENDERINSTANCE_MODE), data, DESCFLAGS_GET::NONE);
				obj->SetParameter(DescID(INSTANCEOBJECT_RENDERINSTANCE_MODE), GeData(INSTANCEOBJECT_RENDERINSTANCE_MODE_NONE), DESCFLAGS_SET::NONE);
			}
		}

		// IDM_SHOWACTIVE
		CallCommand(COMMAND_FRAMEOBJECTS);

		// Restore all instances to their previous modes
		for (auto i = 0; i < activeObjects->GetCount(); ++i)
		{
			const auto obj = static_cast<BaseObject*>(activeObjects->GetIndex(i));
			if(obj->GetType() == Oinstance)
			{
				obj->SetParameter(DescID(INSTANCEOBJECT_RENDERINSTANCE_MODE), data.GetInt32(), DESCFLAGS_SET::NONE);
			}
		}


		EventAdd();
		return true;
	}
};
