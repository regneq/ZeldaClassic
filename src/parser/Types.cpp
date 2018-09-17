#include "../precompiled.h" // always first
#include "Types.h"

#include <cassert>
#include <set>
#include <string>
#include <typeinfo>

#include "CompileError.h"
#include "DataStructs.h"
#include "Scope.h"

using namespace std;
using namespace ZScript;

////////////////////////////////////////////////////////////////
// Forward Declarations

class DataTypeImplSimple;
class DataTypeImplArray;
class DataTypeImplBuiltinClass;
		
////////////////////////////////////////////////////////////////
// DataTypeIds

enum DataTypeId
{
	idStart = 0,
	idVoid = 0,
	idUntyped,
	idBool,
	idFloat,
	idConstFloat,
	idEnd
};

////////////////////////////////////////////////////////////////
// DataTypeImpl

class DataType::Impl : private NoCopy
{
public:
	// Standard types.
	static DataTypeImplSimple const stdVoid;
	static DataTypeImplSimple const stdUntyped;
	static DataTypeImplSimple const stdBool;
	static DataTypeImplSimple const stdFloat;
	static DataTypeImplSimple const stdConstFloat;
	static DataTypeImplArray const stdAryFloat;
#	define X(NAME, TYPE) \
	static DataTypeImplBuiltinClass const std##NAME;
#	include "classes.xtable"
#	undef X
	
	virtual ~Impl() {}
	virtual Impl* clone() const = 0;

	// Basics
	virtual string toString() const = 0;
	virtual bool canBeGlobal() const {return true;}

	// Comparison. As per <=> operator.
	int compare(DataType::Impl const& rhs) const;

	// If this is a standard type and doesn't need to be deleted by its
	// parent.
	bool const standard;

protected:
	Impl(bool standard = false) : standard(standard) {}

private:
	// This is used to compare between instances of the same derived
	// type in compare's implementation.  rhs is guaranteed to be
	// the same class as the derived type.
	virtual int selfCompare(DataType::Impl const& rhs) const = 0;

};

bool operator==(DataType::Impl const&, DataType::Impl const&);
bool operator!=(DataType::Impl const&, DataType::Impl const&);
bool operator<(DataType::Impl const&, DataType::Impl const&);
bool operator<=(DataType::Impl const&, DataType::Impl const&);
bool operator>(DataType::Impl const&, DataType::Impl const&);
bool operator>=(DataType::Impl const&, DataType::Impl const&);

bool canCast(DataType::Impl const& from, DataType::Impl const& to);
DataType::Impl const& getNaiveType(DataType::Impl const& base);

// Returns source if it's standard or NULL, clones it otherwise.
DataType::Impl const* copy(DataType::Impl const* source)
{
	if (source && !source->standard) return source->clone();
	return source;
}

class DataTypeImplSimple : public DataType::Impl
{
public:
	DataTypeImplSimple(DataTypeId id, string const& name,
	                   bool standard = false);
	DataTypeImplSimple* clone() const /*override*/ {
		return new DataTypeImplSimple(id_, name_);}

	virtual string toString() const {return name_;}

private:
	int selfCompare(DataType::Impl const& rhs) const;

	DataTypeId id_;
	string name_;
};

class DataTypeImplBuiltinClass : public DataType::Impl
{
public:
	DataTypeImplBuiltinClass(ZClass::StdId, bool standard = false);
	DataTypeImplBuiltinClass* clone() const /*override*/ {
		return new DataTypeImplBuiltinClass(id);}

	virtual string toString() const;

	ZClass::StdId id;

private:
	int selfCompare(DataType::Impl const& rhs) const;
};

class DataTypeImplArray : public DataType::Impl
{
public:
	DataTypeImplArray(DataType::Impl const* elementType,
	                  bool standard = false);
	DataTypeImplArray(DataTypeImplArray const&);
	~DataTypeImplArray();
	DataTypeImplArray& operator=(DataTypeImplArray const&);
	DataTypeImplArray* clone() const /*override*/;
	
	virtual string toString() const;

	DataType::Impl const* elementType;
			
private:
	int selfCompare(DataType::Impl const& rhs) const;			
};

////////////////////////////////////////////////////////////////
// DataType::Impl

DataTypeImplSimple const DataType::Impl::stdVoid(
	idVoid, "void", true);
DataTypeImplSimple const DataType::Impl::stdUntyped(
	idUntyped, "untyped", true);
DataTypeImplSimple const DataType::Impl::stdBool(
	idBool, "bool", true);
DataTypeImplSimple const DataType::Impl::stdFloat(
	idFloat, "float", true);
DataTypeImplSimple const DataType::Impl::stdConstFloat(
	idConstFloat, "const float", true);
DataTypeImplArray const DataType::Impl::stdAryFloat(&stdFloat, true);

#define X(NAME, TYPE) \
DataTypeImplBuiltinClass const DataType::Impl::std##NAME( \
	ZClass::stdId##NAME, true);
#include "classes.xtable"
#undef X

int DataType::Impl::compare(DataType::Impl const& rhs) const
{
	type_info const& lhsType = typeid(*this);
	type_info const& rhsType = typeid(rhs);
	if (lhsType.before(rhsType)) return -1;
	if (rhsType.before(lhsType)) return 1;
	return selfCompare(rhs);
}

bool operator==(DataType::Impl const& lhs, DataType::Impl const& rhs)
{
	return lhs.compare(rhs) == 0;
}

bool operator!=(DataType::Impl const& lhs, DataType::Impl const& rhs)
{
	return lhs.compare(rhs) != 0;
}

bool operator<(DataType::Impl const& lhs, DataType::Impl const& rhs)
{
	return lhs.compare(rhs) < 0;
}

bool operator<=(DataType::Impl const& lhs, DataType::Impl const& rhs)
{
	return lhs.compare(rhs) <= 0;
}

bool operator>(DataType::Impl const& lhs, DataType::Impl const& rhs)
{
	return lhs.compare(rhs) > 0;
}

bool operator>=(DataType::Impl const& lhs, DataType::Impl const& rhs)
{
	return lhs.compare(rhs) >= 0;
}

bool ZScript::isArray(DataType type)
{
	return bool(type.getElementType());
}

bool ZScript::isClass(DataType type)
{
	return bool(type.getClass());
}

bool canCast(DataType::Impl const& from, DataType::Impl const& to)
{	
	DataType::Impl const& fn = getNaiveType(from);
	DataType::Impl const& tn = getNaiveType(to);
	
	if (fn == DataType::Impl::stdVoid) return false;
	if (tn == DataType::Impl::stdVoid) return false;
	
	if (fn == tn) return true;
	if (fn == DataType::Impl::stdUntyped) return true;
	if (tn == DataType::Impl::stdUntyped) return true;

	if (fn == DataType::Impl::stdFloat && tn == DataType::Impl::stdConstFloat)
		return true;
	// We can cast from const float to float since it's a value type.
	if (fn == DataType::Impl::stdConstFloat && tn == DataType::Impl::stdFloat)
		return true;
	
	if ((fn == DataType::Impl::stdFloat || fn == DataType::Impl::stdConstFloat)
	    && tn == DataType::Impl::stdBool)
		return true;

	return false;
}

DataType::Impl const& getNaiveType(DataType::Impl const& base)
{
	DataType::Impl const* current = &base;
	while (DataTypeImplArray const* array =
	       dynamic_cast<DataTypeImplArray const*>(current))
		current = array->elementType;
	return *current;
}

////////////////////////////////////////////////////////////////
// DataTypeImplSimple

DataTypeImplSimple::DataTypeImplSimple(
	DataTypeId id, string const& name, bool standard)
		: DataType::Impl(standard), id_(id), name_(name)
{}

int DataTypeImplSimple::selfCompare(DataType::Impl const& rhs) const
{
	return id_ - static_cast<DataTypeImplSimple const&>(rhs).id_;
}

////////////////////////////////////////////////////////////////
// DataTypeImplBuiltinClass

DataTypeImplBuiltinClass::DataTypeImplBuiltinClass(
	ZClass::StdId id, bool standard)
	: DataType::Impl(standard), id(id)
{}

string DataTypeImplBuiltinClass::toString() const
{
	return ZClass::getStandard(id)->name;
}

int DataTypeImplBuiltinClass::selfCompare(DataType::Impl const& rhs) const
{
	return id - static_cast<DataTypeImplBuiltinClass const&>(rhs).id;
}

////////////////////////////////////////////////////////////////
// DataTypeImplArray

DataTypeImplArray::DataTypeImplArray(
	DataType::Impl const* elementType, bool standard)
	: DataType::Impl(standard), elementType(elementType)
{}

DataTypeImplArray::DataTypeImplArray(DataTypeImplArray const& other)
	: elementType(copy(other.elementType))
{}

DataTypeImplArray::~DataTypeImplArray()
{
	if (elementType && !elementType->standard) delete elementType;
}

DataTypeImplArray& DataTypeImplArray::operator=(DataTypeImplArray const& rhs)
{
	if (elementType && !elementType->standard) delete elementType;
	elementType = copy(rhs.elementType);
	return *this;
}

DataTypeImplArray* DataTypeImplArray::clone() const
{
	return new DataTypeImplArray(*this);
}

string DataTypeImplArray::toString() const
{
	return elementType->toString() + "[]";
}

int DataTypeImplArray::selfCompare(DataType::Impl const& rhs) const
{
	return elementType->compare(
		*static_cast<DataTypeImplArray const&>(rhs).elementType);
}

////////////////////////////////////////////////////////////////
// DataType

DataType const DataType::stdInvalid;

// Simple Types
DataType const DataType::stdVoid(&DataType::Impl::stdVoid);
DataType const DataType::stdUntyped(&DataType::Impl::stdUntyped);
DataType const DataType::stdBool(&DataType::Impl::stdBool);
DataType const DataType::stdFloat(&DataType::Impl::stdFloat);
DataType const DataType::stdConstFloat(&DataType::Impl::stdConstFloat);
DataType const DataType::stdAryFloat(&DataType::Impl::stdAryFloat);

// Classes
#define X(NAME, TYPE) \
DataType const DataType::std##NAME(&DataType::Impl::std##NAME);
#include "classes.xtable"
#undef X

DataType DataType::arrayOf(DataType type)
{
	return new DataTypeImplArray(copy(type.pimpl_));
}

DataType::DataType() : pimpl_(NULL) {}

DataType::DataType(DataTypeIdBuiltin id)
{
	switch (id)
	{
	case ZVARTYPEID_VOID:
		pimpl_ = &DataType::Impl::stdVoid;
		break;
	case ZVARTYPEID_UNTYPED:
		pimpl_ = &DataType::Impl::stdUntyped;
		break;
	case ZVARTYPEID_BOOL:
		pimpl_ = &DataType::Impl::stdBool;
		break;
	case ZVARTYPEID_FLOAT:
		pimpl_ = &DataType::Impl::stdFloat;
		break;
	case ZVARTYPEID_CONST_FLOAT:
		pimpl_ = &DataType::Impl::stdConstFloat;
		break;
	case ZVARTYPEID_ARY_FLOAT:
		pimpl_ = &DataType::Impl::stdAryFloat;
		break;
	case ZVARTYPEID_AUDIO:
		pimpl_ = &DataType::Impl::stdAudio;
		break;
	case ZVARTYPEID_DEBUG:
		pimpl_ = &DataType::Impl::stdDebug;
		break;
	case ZVARTYPEID_GAME:
		pimpl_ = &DataType::Impl::stdGame;
		break;
	case ZVARTYPEID_GRAPHICS:
		pimpl_ = &DataType::Impl::stdGraphics;
		break;
	case ZVARTYPEID_INPUT:
		pimpl_ = &DataType::Impl::stdInput;
		break;
	case ZVARTYPEID_LINK:
		pimpl_ = &DataType::Impl::stdLink;
		break;
	case ZVARTYPEID_SCREEN:
		pimpl_ = &DataType::Impl::stdScreen;
		break;
	case ZVARTYPEID_TEXT:
		pimpl_ = &DataType::Impl::stdText;
		break;
	case ZVARTYPEID_BITMAP:
		pimpl_ = &DataType::Impl::stdBitmap;
		break;
	case ZVARTYPEID_CHEAT:
		pimpl_ = &DataType::Impl::stdCheat;
		break;
	case ZVARTYPEID_COMBOS:
		pimpl_ = &DataType::Impl::stdComboData;
		break;
	case ZVARTYPEID_DMAPDATA:
		pimpl_ = &DataType::Impl::stdDMapData;
		break;
	case ZVARTYPEID_DOORSET:
		pimpl_ = &DataType::Impl::stdDoorSet;
		break;
	case ZVARTYPEID_DROPSET:
		pimpl_ = &DataType::Impl::stdDropSet;
		break;
	case ZVARTYPEID_EWPN:
		pimpl_ = &DataType::Impl::stdEnemyWeapon;
		break;
	case ZVARTYPEID_FFC:
		pimpl_ = &DataType::Impl::stdFfc;
		break;
	case ZVARTYPEID_GAMEDATA:
		pimpl_ = &DataType::Impl::stdGameData;
		break;
		/*
	case ZVARTYPEID_INFOSHOPDATA:
		pimpl_ = &DataType::Impl::stdInfoShopData;
		break;
		*/
	case ZVARTYPEID_ITEM:
		pimpl_ = &DataType::Impl::stdItem;
		break;
	case ZVARTYPEID_ITEMCLASS:
		pimpl_ = &DataType::Impl::stdItemData;
		break;
	case ZVARTYPEID_LWPN:
		pimpl_ = &DataType::Impl::stdLinkWeapon;
		break;
	case ZVARTYPEID_MAPDATA:
		pimpl_ = &DataType::Impl::stdMapData;
		break;
	case ZVARTYPEID_ZMESSAGE:
		pimpl_ = &DataType::Impl::stdMessageData;
		break;
	case ZVARTYPEID_MIDI:
		pimpl_ = &DataType::Impl::stdMidi;
		break;
	case ZVARTYPEID_MISCCOLOR:
		pimpl_ = &DataType::Impl::stdMiscColor;
		break;
	case ZVARTYPEID_NPC:
		pimpl_ = &DataType::Impl::stdNpc;
		break;
	case ZVARTYPEID_NPCDATA:
		pimpl_ = &DataType::Impl::stdNpcData;
		break;
	case ZVARTYPEID_PALCYCLE:
		pimpl_ = &DataType::Impl::stdPalCycle;
		break;
	case ZVARTYPEID_PALETTE:
		pimpl_ = &DataType::Impl::stdPalette;
		break;
	case ZVARTYPEID_RGBDATA:
		pimpl_ = &DataType::Impl::stdRgb;
		break;
	case ZVARTYPEID_SHOPDATA:
		pimpl_ = &DataType::Impl::stdShopData;
		break;
	case ZVARTYPEID_SPRITEDATA:
		pimpl_ = &DataType::Impl::stdSpriteData;
		break;
	case ZVARTYPEID_WARPRING:
		pimpl_ = &DataType::Impl::stdWarpRing;
		break;
	}
}

DataType::DataType(DataType const& other)
{
	pimpl_ = copy(other.pimpl_);
}

DataType::~DataType()
{
	if (pimpl_ && !pimpl_->standard) delete pimpl_;
}

DataType& DataType::operator=(DataType const& rhs)
{
	if (pimpl_ && !pimpl_->standard) delete pimpl_;
	pimpl_ = copy(rhs.pimpl_);
	return *this;
}

bool DataType::isValid() const
{
	return pimpl_;
}

bool DataType::safe_bool() const
{
	return pimpl_ && (*pimpl_ != *stdVoid.pimpl_);
}

int DataType::compare(DataType const& rhs) const
{
	if (pimpl_ == rhs.pimpl_) return 0;
	if (!pimpl_) return -1;
	if (!rhs.pimpl_) return 1;
	return pimpl_->compare(*rhs.pimpl_);
}

string DataType::toString() const
{
	return pimpl_->toString();
}

bool DataType::canBeGlobal() const
{
	return pimpl_->canBeGlobal();
}

bool DataType::canCastTo(DataType target) const
{
	return canCast(*pimpl_, *target.pimpl_);
}

// Arrays

optional<DataType> DataType::getElementType() const
{
	if (!pimpl_) return nullopt;
	if (DataTypeImplArray const* a =
	    dynamic_cast<DataTypeImplArray const*>(pimpl_))
		return DataType(copy(a->elementType));
	return nullopt;
}

// Classes

ZClass* DataType::getClass() const
{
	if (DataTypeImplBuiltinClass const* pclass =
	    dynamic_cast<DataTypeImplBuiltinClass const*>(pimpl_))
		return ZClass::getStandard(pclass->id);
	return NULL;
}

// Comparison

bool ZScript::operator==(DataType const& lhs, DataType const& rhs)
{
	return lhs.compare(rhs) == 0;
}

bool ZScript::operator!=(DataType const& lhs, DataType const& rhs)
{
	return lhs.compare(rhs) != 0;
}

bool ZScript::operator<(DataType const& lhs, DataType const& rhs)
{
	return lhs.compare(rhs) < 0;
}

bool ZScript::operator<=(DataType const& lhs, DataType const& rhs)
{
	return lhs.compare(rhs) <= 0;
}

bool ZScript::operator>(DataType const& lhs, DataType const& rhs)
{
	return lhs.compare(rhs) > 0;
}

bool ZScript::operator>=(DataType const& lhs, DataType const& rhs)
{
	return lhs.compare(rhs) >= 0;
}

int ZScript::getArrayDepth(DataType type)
{
	int depth = 0;
	while (optional<DataType> elementType = type.getElementType())
	{
		++depth;
		type = *elementType;
	}
	return depth;
}

DataType ZScript::getNaiveType(DataType type)
{
	while (optional<DataType> elementType = type.getElementType())
		type = *elementType;
	return type;
}

////////////////////////////////////////////////////////////////
// Script Types

namespace // file local
{
	struct ScriptTypeData
	{
		string name;
		DataType thisType;
	};

	ScriptTypeData scriptTypes[ScriptType::idEnd] = {
		{"invalid", DataType::stdInvalid},
		{"global", DataType::stdVoid},
		{"ffc", DataType::stdFfc},
		{"item", DataType::stdItemData},
	};
}

ScriptType const ScriptType::invalid(idInvalid);
ScriptType const ScriptType::global(idGlobal);
ScriptType const ScriptType::ffc(idFfc);
ScriptType const ScriptType::item(idItem);

string const& ScriptType::getName() const
{
	if (isValid()) return scriptTypes[id_].name;
	return scriptTypes[idInvalid].name;
}

DataType ScriptType::getThisType() const
{
	if (isValid()) return scriptTypes[id_].thisType;
	return scriptTypes[idInvalid].thisType;
}

bool ZScript::operator==(ScriptType const& lhs, ScriptType const& rhs)
{
	if (!lhs.isValid()) return !rhs.isValid();
	return lhs.id_ == rhs.id_;
}

bool ZScript::operator!=(ScriptType const& lhs, ScriptType const& rhs)
{
	if (lhs.isValid()) return lhs.id_ != rhs.id_;
	return rhs.isValid();
}
