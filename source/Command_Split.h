#pragma once

#include "c4d.h"
#include "instance_functions.h"
#include "InputDialog.h"

class Command_Split : public CommandData
{
INSTANCEOF(Command_Split, CommandData)

public:
	static Command_Split* Alloc()
	{
		return NewObjClear(Command_Split);
	}


	Bool Execute(BaseDocument* doc) override
	{
		if (!doc)
			return false;


		doc->StartUndo();

		// Create Array that holds all objects to operate on
		const AutoAlloc<AtomArray> activeObjects;
		doc->GetActiveObjects(activeObjects, GETACTIVEOBJECTFLAGS::SELECTIONORDER | GETACTIVEOBJECTFLAGS::CHILDREN);

		// Allocation failed
		if (activeObjects == nullptr)
			return false;

		// Minimum of 2 objects have to be selected
		if (activeObjects->GetCount() <= 1)
			return false;


		// Detect Key modifiers#
		BaseContainer state;
		GetInputState(BFM_INPUT_MOUSE, BFM_INPUT_MOUSELEFT, state);
		const auto bCtrl = (state.GetInt32(BFM_INPUT_QUALIFIER) & QCTRL) != 0;
		String newName;

		// Remove all objects that are not instances
		activeObjects->FilterObject(Oinstance, Oinstance, true);
		// Convert the lastly selected instance to the new reference object
		auto lastObject = static_cast<BaseObject*>(activeObjects->GetIndex(activeObjects->GetCount() - 1));
		activeObjects->Remove(lastObject);
		if(activeObjects->GetCount() == 0)
			return false;

		if (!lastObject)
			return false;

		if (lastObject->GetType() != Oinstance)
			return false;

		// Popup InputDialog to enter a new name
		if (bCtrl)
		{
			const auto res = _dlg.Open(-1, -1, 400, 0, true);
			if (!res)
				return false;

			newName = _dlg.GetName();
		}

		const auto parent = lastObject->GetUp();
		const auto pred = lastObject->GetPred();
		const auto refObj = g_ConvertInstance(doc, lastObject);

		if (!refObj)
			return false;

		refObj->SetName(newName.IsEmpty() ? refObj->GetName() : newName);
		doc->InsertObject(refObj, parent, pred);

		for (auto i = 0; i < activeObjects->GetCount(); ++i)
		{
			const auto obj = static_cast<BaseObject*>(activeObjects->GetIndex(i));

			// Update instance links
			g_LinkInstance(doc, obj, refObj);
			obj->SetName(refObj->GetName());
		}

		doc->EndUndo();

		EventAdd();
		return true;
	}


private:
	InputDialog _dlg;
};
