#pragma once

#include "c4d.h"
#include "instance_functions.h"

// TODO: Add function to rename all instances of a single selected object that is not of type instance

class Command_Rename : public CommandData
{
INSTANCEOF(Command_Select, CommandData)

public:
	static Command_Rename* Alloc()
	{
		return NewObjClear(Command_Rename);
	}

	Bool Execute(BaseDocument* doc) override
	{
		if (doc == nullptr)
			return false;

		doc->StartUndo();

		// Create Array that holds all objects to operate on
		const AutoAlloc<AtomArray> activeObjects;
		doc->GetActiveObjects(activeObjects, GETACTIVEOBJECTFLAGS::SELECTIONORDER | GETACTIVEOBJECTFLAGS::CHILDREN);

		// Allocation failed
		if (activeObjects == nullptr)
			return false;

		// Detect Key modifiers#
		BaseContainer state;
		GetInputState(BFM_INPUT_KEYBOARD, BFM_INPUT_MODIFIERS, state);
		const auto bCtrl = (state.GetInt32(BFM_INPUT_QUALIFIER) & QCTRL) != 0;

		for (auto i = 0; i < activeObjects->GetCount(); ++i)
		{
			const auto atom = activeObjects->GetIndex(i);

			// Something failed
			if (atom == nullptr)
				return false;

			// Treat atom as BaseObject
			const auto obj = static_cast<BaseObject*>(atom);

			if (obj->GetType() == Oinstance)
			{
				const auto refObj = bCtrl ? g_GetInstanceRefDeep(doc, obj) : g_GetInstanceRefShallow(doc, obj);
				if (refObj)
				{
					doc->AddUndo(UNDOTYPE::CHANGE_SMALL, obj);
					obj->SetName(refObj->GetName());
				}
			}
		}

		doc->EndUndo();
		EventAdd();

		return true;
	}
};
