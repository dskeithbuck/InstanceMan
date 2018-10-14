#pragma once

#include "c4d.h"
#include "instance_functions.h"
#include "lib_browser.h"

class Command_Swap : public CommandData
{
INSTANCEOF(Command_FrameObjects,CommandData)

public:
	static Command_Swap* Alloc()
	{
		return NewObjClear(Command_Swap);
	}


	Bool Execute(BaseDocument* doc) override
	{
		if (doc == nullptr)
			return false;

		doc->StartUndo();

		// Operate on a single object
		auto instance = doc->GetActiveObject();

		// No object present
		if (instance == nullptr)
			return false;

		if (instance->GetType() != Oinstance)
			return false;

		auto refObj = g_GetInstanceRefDeep(doc, instance);
		if (refObj == nullptr)
			return false;

		const auto instanceUp = instance->GetUp();
		const auto instancePred = instance->GetPred();
		const auto instanceMatrix = instance->GetMl();
		const auto refMatrix = refObj->GetMl();


		// Move the instance object to the reference object's place
		if (instance->GetDown() != refObj && instanceUp != refObj) // Abort when one of both objects is child or parent of the other one
		{
			doc->AddUndo(UNDOTYPE::DELETEOBJ, instance);
			instance->Remove();

			// Copy Matrix


			doc->AddUndo(UNDOTYPE::CHANGE, instance);
			instance->SetMl(refMatrix);

			doc->AddUndo(UNDOTYPE::CHANGE, refObj);
			refObj->SetMl(instanceMatrix);

			doc->InsertObject(instance, refObj->GetUp(), refObj->GetPred());

			instance->Message(MSG_UPDATE);
			refObj->Message(MSG_UPDATE);
		}

		// 
		if (refObj->GetDown() != instance && refObj->GetUp() != instance) // Abort when one of both objects is child or parent of the other one
		{
			if (instancePred != refObj) // Skip if the constellation has just been changed so both objects are next to each other
			{
				doc->AddUndo(UNDOTYPE::DELETEOBJ, refObj);
				refObj->Remove();

				// Copy Matrix

				doc->AddUndo(UNDOTYPE::CHANGE, instance);
				instance->SetMl(refMatrix);

				doc->AddUndo(UNDOTYPE::CHANGE, refObj);
				refObj->SetMl(instanceMatrix);

				doc->InsertObject(refObj, instanceUp, instancePred);

				refObj->Message(MSG_UPDATE);
				instance->Message(MSG_UPDATE);
			}
		}

		doc->EndUndo();

		EventAdd();


		return true;
	}
};
