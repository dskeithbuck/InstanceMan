#pragma once
#include "c4d.h"

#include "c4d_helpers.h"

class Command_Select : public CommandData
{
public:
	Bool Execute(BaseDocument* doc) override
	{
		if (!doc)
			return false;

		// Create Array that holds all objects to operate on; respects order of selection
		const AutoAlloc<AtomArray> activeObjects;
		doc->GetActiveObjectsFilter(*activeObjects, false, NOTOK, Obase);


		// No object selected, no function
		if (activeObjects == nullptr)
			return false;


		// Iterate through all selected objects
		for (auto i = 0; i < activeObjects->GetCount(); ++i)
		{
			const auto atom = activeObjects->GetIndex(i);

			// No object was selected
			if (atom == nullptr)
				return false;

			// Treat atom as BaseList2D
			auto obj = static_cast<BaseList2D*>(atom);


			BaseList2D* root = rh::getInstanceRoot(obj);

			if (root)
				ApplicationOutput(root->GetName());


			// Currently processing an instance?
			// get the linked object from it
			if (obj->GetType() == Oinstance)
			{
				GeData data;
				atom->GetParameter(INSTANCEOBJECT_LINK, data, DESCFLAGS_GET::NONE);

				// Get the linked object
				C4DAtomGoal* goal = data.GetLinkAtom(doc);
				if (!goal)
					return false;


				// TODO: Get the true reference and all Baselinks and select the linked objects
			}
		}


		return true;
	}

	static Command_Select* Alloc()
	{
		return NewObjClear(Command_Select);
	}
};
