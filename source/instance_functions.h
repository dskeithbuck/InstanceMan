#pragma once

#include "c4d.h"

// Sets the reference object of an instance
inline Bool g_LinkInstance(BaseDocument* doc, BaseObject* instance, BaseObject* refObj)
{
	if (instance == nullptr || refObj == nullptr)
		return false;

	if (instance->GetType() != Oinstance)
		return false;

	// Create a new baselink, set refObj as reference and pass it to the instance object
	AutoAlloc<BaseLink> link;
	if (link == nullptr)
		return false;

	link->SetLink(refObj);
	GeData data;
	data.SetBaseLink(link);
	doc->AddUndo(UNDOTYPE::CHANGE_SMALL, instance);
	instance->SetParameter(DescID(INSTANCEOBJECT_LINK), data, DESCFLAGS_SET::NONE);

	return true;
}

// Creates a new single instance from a given object
inline BaseObject* g_CreateInstance(BaseDocument* doc, BaseObject* refObj)
{
	if (refObj == nullptr || doc == nullptr)
		return nullptr;


	// Allocate instance object
	auto instanceObject = BaseObject::Alloc(Oinstance);
	if (instanceObject == nullptr)
		return nullptr;

	doc->AddUndo(UNDOTYPE::NEWOBJ, instanceObject);


	// Set Name to the same as the reference object
	doc->AddUndo(UNDOTYPE::CHANGE_SMALL, instanceObject);
	instanceObject->SetName(refObj->GetName());

	g_LinkInstance(doc, instanceObject, refObj);


	// Deselect reference object and select new instance object
	doc->AddUndo(UNDOTYPE::BITS, refObj);
	refObj->DelBit(BIT_ACTIVE);
	doc->AddUndo(UNDOTYPE::BITS, instanceObject);
	instanceObject->SetBit(BIT_ACTIVE);

	return instanceObject;
}

// Moves all children of srcObj to tgtObj
inline void g_MoveChildren(BaseDocument* doc, BaseObject* srcObj, BaseObject* tgtObj)
{
	if (!srcObj)
		return;

	auto child = srcObj->GetDown();

	while (child)
	{
		const auto temp = child->GetNext();
		doc->AddUndo(UNDOTYPE::CHANGE, child);
		child->Remove();
		child->InsertUnderLast(tgtObj);
		child = temp;
	}
	tgtObj->SetBit(tgtObj->GetBit(BIT_OFOLD));
}


// Simply create a copy from the given instance and put it above
static void g_CreateInstanceCopy(BaseDocument* doc, BaseObject* obj)
{
	if (obj == nullptr || doc == nullptr)
		return;

	// Make a copy of the current instance and prepare it for document insertion
	const auto copyObj = static_cast<BaseObject*>(obj->GetClone(COPYFLAGS::RECURSIONCHECK, nullptr));

	// Deselect reference object and select new instance object
	doc->AddUndo(UNDOTYPE::BITS, obj);
	obj->DelBit(BIT_ACTIVE);

	doc->AddUndo(UNDOTYPE::NEWOBJ, copyObj);
	doc->InsertObject(copyObj, obj->GetUp(), obj->GetPred());
}


// Resolve nested instances and return the true reference object
inline BaseObject* g_GetInstanceRefDeep(BaseDocument* doc, BaseObject* obj)
{
	if (obj == nullptr)
		return nullptr;

	if (obj->GetType() != Oinstance)
		return obj;

	GeData data;

	if (!obj->GetParameter(DescID(INSTANCEOBJECT_LINK), data, DESCFLAGS_GET::NONE))
		return obj;

	const auto linkedEntity = static_cast<BaseObject*>(data.GetLinkAtom(doc, Obase));

	if (linkedEntity)
	{
		if (linkedEntity->IsInstanceOf(Oinstance))
			return g_GetInstanceRefDeep(doc, linkedEntity);

		return linkedEntity;
	}

	return nullptr;
}


// Gets the reference linked in the given instance object
inline BaseObject* g_GetInstanceRefShallow(BaseDocument* doc, BaseObject* obj)
{
	if (obj == nullptr)
		return nullptr;

	GeData data;
	if (!obj->GetParameter(DescID(INSTANCEOBJECT_LINK), data, DESCFLAGS_GET::NONE))
		return nullptr;

	const auto linkedEntity = static_cast<BaseObject*>(data.GetLinkAtom(doc, Obase));

	return linkedEntity;
}


// If multiple objects selected, use last selected as reference
static void g_CreateInstancesFromSelection(BaseDocument* doc, BaseObject* refObj, BaseObject* obj)
{
	if (obj == nullptr || doc == nullptr || refObj == nullptr)
		return;


	const auto instance = g_CreateInstance(doc, refObj);
	if (!instance)
		return;

	// TODO: Check if this works correctly
	obj->CopyMatrixTo(instance);

	doc->InsertObject(instance, obj->GetUp(), obj->GetPred());
	g_MoveChildren(doc, obj, instance);

	// Remove the current object, since we want to swap the object with the instance
	// Move children beforehand!
	doc->AddUndo(UNDOTYPE::DELETEOBJ, obj);
	obj->Remove();
	BaseObject::Free(obj);
}


// Create a new instance linked to the given object
static void g_CreateInstanceFromSingle(BaseDocument* doc, BaseObject* refObj)
{
	if (refObj == nullptr || doc == nullptr)
		return;

	const auto instance = g_CreateInstance(doc, refObj);
	if (!instance)
		return;

	// Copy matrix
	refObj->CopyMatrixTo(instance);

	doc->InsertObject(instance, refObj->GetUp(), refObj->GetPred());
}

// Makes an instance editable, no matter of what type it is - works with render- and multiinstance
// Adds Undos!
inline BaseObject* g_ConvertInstance(BaseDocument* doc, BaseObject* obj)
{
	if (obj == nullptr)
		return nullptr;

	// Set to normal instance mode, otherwise "make editable" will fail
	if (obj->GetType() == Oinstance)
	{
		doc->AddUndo(UNDOTYPE::CHANGE_SMALL, obj);
		obj->SetParameter(DescID(INSTANCEOBJECT_RENDERINSTANCE_MODE), GeData(INSTANCEOBJECT_RENDERINSTANCE_MODE_NONE), DESCFLAGS_SET::NONE);
	}


	// Make editable
	ModelingCommandData mcd;
	mcd.doc = doc;
	mcd.op = obj;
	doc->AddUndo(UNDOTYPE::DELETEOBJ, obj);
	if (!SendModelingCommand(MCOMMAND_MAKEEDITABLE, mcd))
		return nullptr;

	const auto res = static_cast<BaseObject*>(mcd.result->GetIndex(0));

	doc->AddUndo(UNDOTYPE::NEWOBJ, res);
	return res;
}
