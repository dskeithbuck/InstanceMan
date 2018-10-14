#pragma once
#include "c4d.h"

#include "instance_functions.h"

class Command_MakeEditable : public CommandData
{
INSTANCEOF(Command_MakeEditable, CommandData)

public:
	static Command_MakeEditable* Alloc()
	{
		return NewObjClear(Command_MakeEditable);
	}

	Bool Execute(BaseDocument* doc) override
	{
		if (!doc)
			return false;

		doc->StartUndo();

		// Create Array that holds all objects to operate on
		const AutoAlloc<AtomArray> activeObjects;
		doc->GetActiveObjectsFilter(*activeObjects, false, NOTOK, Oinstance);


		// No object selected, no function
		if (activeObjects == nullptr)
			return false;

		for (auto i = 0; i < activeObjects->GetCount(); ++i)
		{
			auto instance = static_cast<BaseObject*>(activeObjects->GetIndex(i));
			if (instance == nullptr)
				continue;


			const auto instanceUp = instance->GetUp();
			const auto instancePred = instance->GetPred();

			const auto obj = g_ConvertInstance(doc, instance);
			if (obj == nullptr)
				return false;

			doc->InsertObject(obj, instanceUp, instancePred);
		}

		doc->EndUndo();
		EventAdd();

		return true;
	}
};
