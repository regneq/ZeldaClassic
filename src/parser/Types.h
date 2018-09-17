#ifndef ZSCRIPT_TYPES_H
#define ZSCRIPT_TYPES_H

#include <string>
#include "CompilerUtils.h"

namespace ZScript
{
	// Forward Declarations
	class Scope;
	class ZClass;

	////////////////////////////////////////////////////////////////
	// DataTypeId - temporary for interacting with GlobalSymbols.
	// TODO fix global symbols so this isn't needed.
	
	enum DataTypeIdBuiltin
	{
		ZVARTYPEID_VOID,
		ZVARTYPEID_UNTYPED,
		ZVARTYPEID_BOOL,
		ZVARTYPEID_FLOAT,
		ZVARTYPEID_CONST_FLOAT,
		ZVARTYPEID_ARY_FLOAT,
		ZVARTYPEID_AUDIO,
		ZVARTYPEID_DEBUG,
		ZVARTYPEID_GAME,
		ZVARTYPEID_GRAPHICS,
		ZVARTYPEID_INPUT,
		ZVARTYPEID_LINK,
		ZVARTYPEID_SCREEN,
		ZVARTYPEID_TEXT,
		ZVARTYPEID_BITMAP,
		ZVARTYPEID_CHEAT,
		ZVARTYPEID_COMBOS,
		ZVARTYPEID_DMAPDATA,
		ZVARTYPEID_DOORSET,
		ZVARTYPEID_DROPSET,
		ZVARTYPEID_EWPN,
		ZVARTYPEID_FFC,
		ZVARTYPEID_GAMEDATA,
		ZVARTYPEID_INFOSHOPDATA,
		ZVARTYPEID_ITEM,
		ZVARTYPEID_ITEMCLASS,
		ZVARTYPEID_LWPN,
		ZVARTYPEID_MAPDATA,
		ZVARTYPEID_ZMESSAGE,
		ZVARTYPEID_MIDI,
		ZVARTYPEID_MISCCOLOR,
		ZVARTYPEID_NPC,
		ZVARTYPEID_NPCDATA,
		ZVARTYPEID_PALCYCLE,
		ZVARTYPEID_PALETTE,
		ZVARTYPEID_RGBDATA,
		ZVARTYPEID_SHOPDATA,
		ZVARTYPEID_SPRITEDATA,
		ZVARTYPEID_WARPRING
	};	
	
	////////////////////////////////////////////////////////////////
	// DataType
	
	// Value semantics
	class DataType : public SafeBool<DataType>
	{
	public:
		class Impl;

		static DataType const stdInvalid;
		// Simple Types
		static DataType const stdVoid;
		static DataType const stdUntyped;
		static DataType const stdBool;
		static DataType const stdFloat;
		static DataType const stdConstFloat;
		static DataType const stdAryFloat;
		// Classes
#		define X(NAME, TYPE) static DataType const std##NAME;
#		include "classes.xtable"
#		undef X

		static DataType arrayOf(DataType type);
	
		// Create default, invalid data type.
		DataType();
		// Create data type from enum id. TODO phase out.
		DataType(DataTypeIdBuiltin id);

		DataType(DataType const& other);
		~DataType();
		DataType& operator=(DataType const& rhs);

		bool isValid() const;
		// False if invalid, or VOID.
		bool safe_bool() const;

		// Comparison. As per <=> operator.
		int compare(DataType const& rhs) const;
		
		// Basics
		std::string toString() const;
		bool canBeGlobal() const;
		bool canCastTo(DataType target) const;

		// Arrays
		// If this is an array, get its element type.
		optional<DataType> getElementType() const;

		// Classes
		ZClass* getClass() const;
		
	private:
		DataType(Impl const* pimpl) : pimpl_(pimpl) {}

		// Owned by a type store, not us.
		Impl const* pimpl_;
	};

	bool operator==(DataType const&, DataType const&);
	bool operator!=(DataType const&, DataType const&);
	bool operator<(DataType const&, DataType const&);
	bool operator<=(DataType const&, DataType const&);
	bool operator>(DataType const&, DataType const&);
	bool operator>=(DataType const&, DataType const&);

	bool isArray(DataType type);
	bool isClass(DataType type);
	
	// Get the number of nested arrays in the type.
	int getArrayDepth(DataType type);

	// Get the type without considering arrays.
	DataType getNaiveType(DataType type);

	////////////////////////////////////////////////////////////////
	// Script Types

	// Basically an enum.
	class ScriptType
	{
		friend bool operator==(ScriptType const& lhs, ScriptType const& rhs);
		friend bool operator!=(ScriptType const& lhs, ScriptType const& rhs);

	public:
		enum Id
		{
			idInvalid,
			idStart,
			idGlobal = idStart,
			idFfc,
			idItem,
			idEnd
		};
	
		ScriptType() : id_(idInvalid) {}
		
		std::string const& getName() const;
		DataType getThisType() const;
		bool isValid() const {return id_ >= idStart && id_ < idEnd;}

		static ScriptType const invalid;
		static ScriptType const global;
		static ScriptType const ffc;
		static ScriptType const item;

	private:
		ScriptType(Id id) : id_(id) {}
		
		Id id_;
	};

	// All invalid values are equal to each other.
	bool operator==(ScriptType const& lhs, ScriptType const& rhs);
	bool operator!=(ScriptType const& lhs, ScriptType const& rhs);
}

#endif
