#pragma once

#include "c4d.h"

#include "instance_functions.h"

class Command_Swap : public CommandData
{
INSTANCEOF(Command_FrameObjects,CommandData)

public:
	static maxon::Result<Command_Swap*> Alloc()
	{
		iferr_scope;
		return NewObj(Command_Swap) iferr_return;
	}

	Int32 GetState(BaseDocument* doc) override
	{
		// Disable Menu entry if no valid object is selected
		const auto obj = doc->GetActiveObject();
		doc->GetActiveObject();
		if (!obj)
			return 0;

		if (obj->IsInstanceOf(Oinstance))
			return CMD_ENABLED;

		auto bc = obj->GetDataInstance();
		if (bc)
		{
			const auto dataBc = bc->GetContainerInstance(PID_IM);
			if (dataBc)
			{
				const auto link = dataBc->GetObjectLink(SWAPTARGET, doc);
				if (link) return CMD_ENABLED;
			}
		}
		return 0;
	}

	Bool Execute(BaseDocument* doc) override
	{
		if (!doc)
			return false;

		doc->StartUndo();

		// Operate on a single object
		auto obj = doc->GetActiveObject();

		// No object present
		if (!obj)
			return false;

		auto swapTarget = g_GetInstanceRef(obj);

		// Swap target is an instance object
		// Check if the object has already been swapped and evaluate the object to swap with
		if (!obj->IsInstanceOf(Oinstance))
		{
			auto bc = obj->GetDataInstance();
			if (bc)
			{
				const auto dataBc = bc->GetContainerInstance(PID_IM);
				if (dataBc)
					swapTarget = dataBc->GetObjectLink(SWAPTARGET, doc);
			}
		}

		if (!swapTarget)
			return false;

		// Temporarily save insertion points and matrices as objects get removed in the process
		const auto objUp = obj->GetUp();
		const auto objPred = obj->GetPred();
		const auto objMatrix = obj->GetMl();

		// Check ift Ctrl is pressed
		const auto bCtrl = rh::g_CheckModifierKey(QCTRL);

		// Store the instance hierarchy if Ctrl is pressed
		const AutoAlloc<AtomArray> refObjChildren;
		if (bCtrl)
			rh::g_GetChildren(swapTarget, refObjChildren);

		// Using a clone so we do not have to care about insertion point validation
		// Clone has no hierarchy as it is saved for swapping
		const auto swapTargetClone = static_cast<BaseObject*>(swapTarget->GetClone(bCtrl ? COPYFLAGS::NO_HIERARCHY : COPYFLAGS::NONE, nullptr));


		// Swap children
		// Move the instance's children directly to the clone of refObj, since it has noc hierarchy anymore

		if (bCtrl)
		{
			g_MoveChildren(obj, swapTargetClone);

			// Now move the refObject's children to the instance
			for (auto i = 0; i < refObjChildren->GetCount(); ++i)
			{
				auto child = static_cast<BaseObject*>(refObjChildren->GetIndex(i));
				if (!child)
					continue;

				doc->AddUndo(UNDOTYPE::DELETEOBJ, child);
				child->Remove();
				child->InsertUnderLast(obj);
			}
		}


		// Remove instance from the objects list
		doc->AddUndo(UNDOTYPE::DELETEOBJ, obj);
		obj->Remove();

		// Set the instance matrix to the one of refObj, so everything is at the same place
		doc->AddUndo(UNDOTYPE::CHANGE, obj);
		obj->SetMl(swapTargetClone->GetMl());

		// Set the refObj matrix to the one of the instance, matrix swap is complete
		doc->AddUndo(UNDOTYPE::CHANGE, swapTarget);
		swapTargetClone->SetMl(objMatrix);

		// Insert it to the refObj's position
		doc->InsertObject(obj, swapTarget->GetUp(), swapTarget->GetPred());

		// Add a link to the swapped object
		if (obj->IsInstanceOf(Oinstance))
		{
			auto targetBc = swapTargetClone->GetDataInstance();
			BaseContainer dataBc;
			if (targetBc)
			{
				dataBc.SetLink(SWAPTARGET, obj);
				targetBc->SetContainer(PID_IM, dataBc);
			}
		}
			// Remove link when swapping back
		else
		{
			auto targetBc = obj->GetDataInstance();
			BaseContainer dataBc;
			if (targetBc)
			{
				dataBc.SetLink(SWAPTARGET, nullptr);
				targetBc->SetContainer(PID_IM, dataBc);
			}
		}
		// Now insert the refObject at the instance's old position
		doc->InsertObject(swapTargetClone, objUp, objPred);

		// Update all links
		swapTarget->TransferGoal(swapTargetClone, false);

		// Set all states
		swapTargetClone->SetAllBits(swapTarget->GetAllBits());

		// Set selections
		obj->ToggleBit(BIT_ACTIVE);
		swapTargetClone->ToggleBit(BIT_ACTIVE);

		// Finally delete the object
		swapTarget->Remove();
		BaseObject::Free(swapTarget);

		doc->EndUndo();

		EventAdd();


		return true;
	}
};
