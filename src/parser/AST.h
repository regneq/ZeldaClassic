#ifndef ZSCRIPT_AST_H
#define ZSCRIPT_AST_H

// Declare for flex and bison.
namespace ZScript
{
	class AST;
}
#define YYSTYPE ZScript::AST*

#ifdef _MSC_VER
#if (_MSC_VER <= 1600)
#define snprintf _snprintf
#endif
#endif

#include <list>
#include <vector>
#include <map>
#include <memory>
#include <string>
#include "y.tab.hpp"
#include "Compiler.h"
#include "CompileOption.h"
#include "owning_ptr.h"
#include "owning_vector.h"

////////////////////////////////////////////////////////////////

extern std::string curfilename;

namespace ZScript
{
	////////////////////////////////////////////////////////////////
	// Forward Declarations
	
	// ASTVisitors.h
	class ASTVisitor;
	// CompileError.h
	class CompileError;
	class CompileErrorHandler;
	// Scope.h
	class Scope;
	class ZClass;
	// ZScript.h
	class Datum;
	class Literal;
	class Function;

	// Local
	class AST; // virtual
	class ASTFile;
	class ASTFloat;
	class ASTString;
	class ASTSetOption;
	// Statements
	class ASTStmt; // virtual
	class ASTBlock;
	class ASTStmtIf;
	class ASTStmtIfElse;
	class ASTStmtSwitch;
	class ASTSwitchCases;
	class ASTStmtFor;
	class ASTStmtWhile;
	class ASTStmtDo;
	class ASTStmtReturn;
	class ASTStmtReturnVal;
	class ASTStmtBreak;
	class ASTStmtContinue;
	class ASTStmtEmpty;
	// Declarations
	class ASTDecl; // virtual
	class ASTScript;
	class ASTImportDecl;
	class ASTFuncDecl;
	class ASTDataDeclList;
	class ASTDataDecl;
	class ASTDataDeclExtraArray;
	class ASTDataTypeDef;
	class ASTScriptTypeDef;
	// Expressions
	class ASTExpr; // virtual
	class ASTExprConst;
	class ASTExprAssign;
	class ASTExprIdentifier;
	class ASTExprArrow;
	class ASTExprIndex;
	class ASTExprCall;
	class ASTUnaryExpr; // virtual
	class ASTExprNegate;
	class ASTExprNot;
	class ASTExprBitNot;
	class ASTExprIncrement;
	class ASTExprPreIncrement;
	class ASTExprDecrement;
	class ASTExprPreDecrement;
	class ASTBinaryExpr; // virtual
	class ASTLogExpr; // virtual
	class ASTExprAnd;
	class ASTExprOr;
	class ASTRelExpr; // virtual
	class ASTExprGT;
	class ASTExprGE;
	class ASTExprLT;
	class ASTExprLE;
	class ASTExprEQ;
	class ASTExprNE;
	class ASTAddExpr; // virtual
	class ASTExprPlus;
	class ASTExprMinus;
	class ASTMultExpr; // virtual
	class ASTExprTimes;
	class ASTExprDivide;
	class ASTExprModulo;
	class ASTBitExpr; // virtual
	class ASTExprBitAnd;
	class ASTExprBitOr;
	class ASTExprBitXor;
	class ASTShiftExpr; // virtual
	class ASTExprLShift;
	class ASTExprRShift;
	// Literals
	class ASTLiteral; // virtual
	class ASTNumberLiteral;
	class ASTBoolLiteral;
	class ASTStringLiteral;
	class ASTArrayLiteral;
	class ASTOptionValue;
	// Types
	class ASTScriptType;
	class ASTDataType;

	//////////////////////////////////////////////////////////////////////////////
	class Location
	{
	public:
		static Location const NONE;

		int first_line;
		int last_line;
		int first_column;
		int last_column;
		std::string fname;

		Location()
			: first_line(-1), last_line(-1),
			  first_column(-1), last_column(-1),
			  fname(curfilename)
		{}

		Location(YYLTYPE loc)
			: first_line(loc.first_line), last_line(loc.last_line),
			  first_column(loc.first_column), last_column(loc.last_column),
			  fname(curfilename)
		{}

		std::string asString() const;
	};

	////////////////////////////////////////////////////////////////

	class AST
	{
	public:
		AST(Location const& location);
		virtual ~AST() {}
		// Calls subclass's copy constructor on self.
		virtual AST* clone() const = 0;

		virtual void execute(ASTVisitor& visitor, void* param = NULL) = 0;
		virtual std::string asString() const {return "unknown";}

		// Filename and linenumber.
		Location location;

		// List of expected compile error ids for this node. They are
		// removed as they are encountered.
		owning_vector<ASTExprConst> compileErrorCatches;

		// If this node has been disabled due to an error.
		bool disabled;

		void bindScope(Scope* scope) {scope_ = scope;}

		// Subclass Predicates (replacing typeof and such).
		virtual bool isTypeArrow() const {return false;}
		virtual bool isTypeIndex() const {return false;}
		virtual bool isTypeIdentifier() const {return false;}
		virtual bool isTypeVarDecl() const {return false;}
		virtual bool isTypeArrayDecl() const {return false;}
		virtual bool isStringLiteral() const {return false;}
		virtual bool isArrayLiteral() const {return false;}

	private:
		Scope* scope_;
	};


	////////////////////////////////////////////////////////////////

	class ASTFile : public AST
	{
	public:
		ASTFile(Location const& location);
		ASTFile* clone() const /*override*/ {return new ASTFile(*this);}
    
		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;
		std::string asString() const /*override*/;

		// Add a declaration to the proper list based on its type.
		void addDeclaration(ASTDecl* declaration);
		bool hasDeclarations() const;

		owning_vector<ASTSetOption> options;
		owning_vector<ASTImportDecl> imports;
		owning_vector<ASTDataDeclList> variables;
		owning_vector<ASTFuncDecl> functions;
		owning_vector<ASTDataTypeDef> dataTypes;
		owning_vector<ASTScriptTypeDef> scriptTypes;
		owning_vector<ASTScript> scripts;
	};

	class ASTFloat : public AST
	{
	public:
		enum Type {typeDecimal, typeBinary, typeHex};
	
		ASTFloat(char* value, Type type, Location const& location);
		ASTFloat(char const* value, Type type, Location const& location);
		ASTFloat(std::string const& value, Type type,
		         Location const& location);
		ASTFloat(long value, Type type, Location const& location);
		ASTFloat* clone() const /*override*/ {return new ASTFloat(*this);}
	
		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;
    	
		std::pair<std::string,std::string> parseValue() const;

		Type type;
		std::string value;
		bool negative;
	};

	class ASTString : public AST
	{
	public:
		ASTString(const char* str, Location const& location);
		ASTString(std::string const& str, Location const& location);
		ASTString* clone() const /*override*/ {return new ASTString(*this);}
	
		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		std::string value;
	};

	class ASTSetOption : public AST
	{
	public:
		ASTSetOption(std::string const& name, ASTExprConst* expr,
		             Location const& location);
		ASTSetOption(std::string const& name, CompileOptionSetting setting,
		             Location const& location);
		virtual ASTSetOption* clone() const /*override*/ {
			return new ASTSetOption(*this);}

		virtual void execute(
			ASTVisitor& visitor, void* param = NULL) /*override*/;
		virtual std::string asString() const /*override*/;

		CompileOptionSetting getSetting(
				CompileErrorHandler* = NULL) const;
	
		std::string name;
		CompileOption option;
		owning_ptr<ASTExprConst> expr;
		CompileOptionSetting setting;
	};

	////////////////////////////////////////////////////////////////
	// Statements

	class ASTStmt : public AST
	{
	public:
		ASTStmt(Location const& location);
		ASTStmt* clone() const /*override*/ = 0;

		bool isDisabled() const {return disabled_;}
		void disable() {disabled_ = true;}
	
	private:
		bool disabled_;
	};
    
	class ASTBlock : public ASTStmt
	{
	public:
		ASTBlock(Location const& location);
		ASTBlock* clone() const /*override*/ {return new ASTBlock(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		owning_vector<ASTSetOption> options;
		owning_vector<ASTStmt> statements;
	};
    
	class ASTStmtIf : public ASTStmt
	{
	public:
		ASTStmtIf(ASTExpr* condition, ASTStmt* thenStatement,
		          Location const& location);
		ASTStmtIf* clone() const /*override*/ {return new ASTStmtIf(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		owning_ptr<ASTExpr> condition;
		owning_ptr<ASTStmt> thenStatement;
	};

	class ASTStmtIfElse : public ASTStmtIf
	{
	public:
		ASTStmtIfElse(ASTExpr* condition, ASTStmt* thenStatement,
		              ASTStmt* elseStatement, Location const& location);
		ASTStmtIfElse* clone() const /*override*/ {
			return new ASTStmtIfElse(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		owning_ptr<ASTStmt> elseStatement;
	};

	// A switch statement.
	class ASTStmtSwitch : public ASTStmt
	{
	public:
		ASTStmtSwitch(Location const& location);
		ASTStmtSwitch* clone() const /*override*/ {
			return new ASTStmtSwitch(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		// The key expression used to switch.
		owning_ptr<ASTExpr> key;
		// A vector of case groupings.
		owning_vector<ASTSwitchCases> cases;
	};

	// A grouping of switch statement labels, and the code for the group.
	class ASTSwitchCases : public AST
	{
	public:
		ASTSwitchCases(Location const& location);
		ASTSwitchCases* clone() const /*override*/ {
			return new ASTSwitchCases(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		// The list of case labels.
		owning_vector<ASTExprConst> cases;
		// If the default case is included in this grouping.
		bool isDefault;
		// The block to run.
		owning_ptr<ASTBlock> block;
	};


	class ASTStmtFor : public ASTStmt
	{
	public:
		ASTStmtFor(ASTStmt* setup, ASTExpr* test, ASTStmt* increment,
		           ASTStmt* body, Location const& location);
		ASTStmtFor* clone() const /*override*/ {
			return new ASTStmtFor(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		owning_ptr<ASTStmt> setup;
		owning_ptr<ASTExpr> test;
		owning_ptr<ASTStmt> increment;
		owning_ptr<ASTStmt> body;
	};

	class ASTStmtWhile : public ASTStmt
	{
	public:
		ASTStmtWhile(ASTExpr* test, ASTStmt* body, Location const& location);
		ASTStmtWhile* clone() const /*override*/ {
			return new ASTStmtWhile(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		owning_ptr<ASTExpr> test;
		owning_ptr<ASTStmt> body;
	};

	class ASTStmtDo : public ASTStmt
	{
	public:
		ASTStmtDo(ASTExpr* test, ASTStmt* body, Location const& location);
		ASTStmtDo* clone() const /*override*/ {return new ASTStmtDo(*this);}
	
		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		owning_ptr<ASTExpr> test;
		owning_ptr<ASTStmt> body;
	};

	class ASTStmtReturn : public ASTStmt
	{
	public:
		ASTStmtReturn(Location const& location);
		ASTStmtReturn* clone() const /*override*/ {
			return new ASTStmtReturn(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;
	};

	class ASTStmtReturnVal : public ASTStmtReturn
	{
	public:
		ASTStmtReturnVal(ASTExpr* value, Location const& location);
		ASTStmtReturnVal* clone() const /*override*/ {
			return new ASTStmtReturnVal(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		owning_ptr<ASTExpr> value;
	};

	class ASTStmtBreak : public ASTStmt
	{
	public:
		ASTStmtBreak(Location const& location);
		ASTStmtBreak* clone() const /*override*/ {
			return new ASTStmtBreak(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;
	};

	class ASTStmtContinue : public ASTStmt
	{
	public:
		ASTStmtContinue(Location const& location);
		ASTStmtContinue* clone() const /*override*/ {
			return new ASTStmtContinue(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;
	};

	class ASTStmtEmpty : public ASTStmt
	{
	public:
		ASTStmtEmpty(Location const& location);
		ASTStmtEmpty* clone() const /*override*/ {
			return new ASTStmtEmpty(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;
	};

	////////////////////////////////////////////////////////////////
	// Declarations

	// virtual
	class ASTDecl : public ASTStmt
	{
	public:
		// Different declaration types.
		enum Type
		{
			TYPE_VOID,
			TYPE_SCRIPT,
			TYPE_IMPORT,
			TYPE_CONSTANT,
			TYPE_FUNCTION,
			TYPE_DATALIST,
			TYPE_DATA,
			TYPE_DATATYPE,
			TYPE_SCRIPTTYPE
		};

		ASTDecl(Location const& location);
		ASTDecl* clone() const /*override*/ = 0;

		// Return the subclass id.
		virtual Type getDeclarationType() const = 0;
	};

	// Declares a script.
	class ASTScript : public ASTDecl
	{
	public:
		ASTScript(Location const& location);
		ASTScript* clone() const /*override*/ {return new ASTScript(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		Type getDeclarationType() const /*override*/ {return TYPE_SCRIPT;}
    
		// Adds a declaration to the proper vector.
		void addDeclaration(ASTDecl& declaration);

		owning_ptr<ASTScriptType> type;
		std::string name;
		owning_vector<ASTSetOption> options;
		owning_vector<ASTDataDeclList> variables;
		owning_vector<ASTFuncDecl> functions;
		owning_vector<ASTDataTypeDef> types;
	};

	class ASTImportDecl : public ASTDecl
	{
	public:
		ASTImportDecl(std::string const& filename, Location const& location);
		ASTImportDecl* clone() /*override*/ const {
			return new ASTImportDecl(*this);}
    
		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		Type getDeclarationType() const /*override*/ {return TYPE_IMPORT;}

		std::string const& getFilename() const {return filename_;}
		ASTFile* getTree() {return tree_.get();}
		ASTFile const* getTree() const {return tree_.get();}
		void giveTree(ASTFile* tree) {tree_ = tree;}
	
	private:
		std::string filename_;
		owning_ptr<ASTFile> tree_;
	};

	class ASTFuncDecl : public ASTDecl
	{
	public:
		ASTFuncDecl(Location const& location);
		ASTFuncDecl* clone() const /*override*/ {
			return new ASTFuncDecl(*this);}
    
		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		Type getDeclarationType() const /*override*/ {return TYPE_FUNCTION;}

		owning_ptr<ASTDataType> returnType;
		owning_vector<ASTDataDecl> parameters;
		std::string name;
		owning_ptr<ASTBlock> block;
	};

	// A line of variable/constant declarations:
	// int a, b, c[];
	class ASTDataDeclList : public ASTDecl
	{
	public:
		ASTDataDeclList(Location const& location);
		ASTDataDeclList(ASTDataDeclList const&);
		ASTDataDeclList& operator=(ASTDataDeclList const& rhs);
		ASTDataDeclList* clone() const /*override*/ {
			return new ASTDataDeclList(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;
		Type getDeclarationType() const /*override*/ {return TYPE_DATALIST;}

		std::vector<ASTDataDecl*> const& getDeclarations() const {
			return declarations_.data();}
		void addDeclaration(ASTDataDecl* declaration);

		// The base type at the start of the line shared by all the declarations.
		owning_ptr<ASTDataType> baseType;

	private:
		// The list of individual data declarations.
		owning_vector<ASTDataDecl> declarations_;
	};

	// Declares a single variable or constant. May or may not be inside an
	// ASTDataDeclList.
	class ASTDataDecl : public ASTDecl
	{
	public:
		ASTDataDecl(Location const& location);
		ASTDataDecl(ASTDataDecl const&);
		ASTDataDecl& operator=(ASTDataDecl const& rhs);
		ASTDataDecl* clone() const /*override*/ {return new ASTDataDecl(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		Type getDeclarationType() const /*override*/ {return TYPE_DATA;}

		ASTExpr* getInitializer() {return initializer_.get();}
		ASTExpr const* getInitializer() const {return initializer_.get();}
		void setInitializer(ASTExpr* initializer);

		// The list containing this declaration. Should be set by that list when
		// this is added.
		ASTDataDeclList* list;

		// Reference back to the datum manager for this node. Should be set by
		// that Datum when it is created.
		Datum* manager;

		// This type of this data (minus the extra arrays). This should only be
		// set if this declaration is not part of a list, as the list's base type
		// should be used instead in that case.
		owning_ptr<ASTDataType> baseType;

		// The symbol this declaration is binding.
		std::string name;

		// Extra array type for this specific declaration. The final type is the
		// list's base type combined with these.
		owning_vector<ASTDataDeclExtraArray> extraArrays;

	private:
		// The initialization expression. Optional.
		owning_ptr<ASTExpr> initializer_;
	};

	// Resolves the type, using either the list's or this node's own base type
	// as appropriate.
	DataType resolveType(ASTDataDecl const&, Scope* scope);

	bool hasSize(ASTDataDecl const&);

	// The extra array parameters appended to a data declaration name.
	class ASTDataDeclExtraArray : public AST
	{
	public:
		ASTDataDeclExtraArray(Location const& location);
		ASTDataDeclExtraArray* clone() const /*override*/ {
			return new ASTDataDeclExtraArray(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		// The vector of array dimensions. Empty means unspecified.
		owning_vector<ASTExpr> dimensions;

		// If this declares an a sized array.
		bool hasSize() const {return dimensions.size();}

		// Get the total size of this array at compile time.
		optional<int> getCompileTimeSize(
				CompileErrorHandler* errorHandler = NULL)
				const;
	};

	class ASTDataTypeDef : public ASTDecl
	{
	public:
		ASTDataTypeDef(ASTDataType* type, std::string const& name,
		               Location const& location);
		ASTDataTypeDef* clone() const /*override*/ {
			return new ASTDataTypeDef(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		Type getDeclarationType() const /*override*/ {return TYPE_DATATYPE;}

		owning_ptr<ASTDataType> type;
		std::string name;
	};

	class ASTScriptTypeDef : public ASTDecl
	{
	public:
		ASTScriptTypeDef(ASTScriptType* oldType, std::string const& newName,
		                 Location const& location);
		ASTScriptTypeDef* clone() const /*override*/ {
			return new ASTScriptTypeDef(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		Type getDeclarationType() const /*override*/ {
			return TYPE_SCRIPTTYPE;}

		owning_ptr<ASTScriptType> oldType;
		std::string newName;
	};

	////////////////////////////////////////////////////////////////
	// Expressions

	class ASTExpr : public ASTStmt
	{
	public:
		ASTExpr(Location const& location);
		ASTExpr* clone() const /*override*/ = 0;

		virtual bool isConstant() const = 0;

		// Return this expression's value if it has already been resolved at
		// compile time.
		virtual optional<long> getCompileTimeValue(CompileErrorHandler*)
			const
		{return nullopt;}

		// Returns the read or write type for this expression. Void for
		// either means that it can't be read from/written to.
		virtual DataType getReadType() const {return DataType::stdVoid;}
		virtual DataType getWriteType() const {return DataType::stdVoid;}
	};

	// Wrap around an expression to type it as constant.
	class ASTExprConst : public ASTExpr
	{
	public:
		ASTExprConst(ASTExpr* content, Location const& location);
		ASTExprConst* clone() const /*override*/ {
			return new ASTExprConst(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		bool isConstant() const /*override*/ {return true;}

		optional<long> getCompileTimeValue(CompileErrorHandler* errorHandler)
			const /*override*/;
		DataType getReadType() const /*override*/ {
			return content ? content->getReadType() : DataType::stdVoid;}
	
		owning_ptr<ASTExpr> content;
	};

	class ASTExprAssign : public ASTExpr
	{
	public:
		ASTExprAssign(ASTExpr* left, ASTExpr* right,
		              Location const& location);
		ASTExprAssign* clone() const /*override*/ {
			return new ASTExprAssign(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		bool isConstant() const /*override*/ {
			return right && right->isConstant();}

		optional<long> getCompileTimeValue(CompileErrorHandler*)
			const /*override*/;
		DataType getReadType() const /*override*/ {
			return right ? right->getReadType() : DataType::stdVoid;}
		DataType getWriteType() const /*override*/ {
			return right ? right->getWriteType() : DataType::stdVoid;}
	
		owning_ptr<ASTExpr> left;
		owning_ptr<ASTExpr> right;
	};

	class ASTExprIdentifier : public ASTExpr
	{
	public:
		ASTExprIdentifier(std::string const& name, Location const& location);
		ASTExprIdentifier* clone() const /*override*/ {
			return new ASTExprIdentifier(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;
		std::string asString() const /*override*/;
		bool isTypeIdentifier() const /*override*/ {return true;}

		bool isConstant() const /*override*/ {return constant_;}
		void markConstant() {constant_ = true;}

		optional<long> getCompileTimeValue(CompileErrorHandler*)
			const /*override*/;
		DataType getReadType() const /*override*/;
		DataType getWriteType() const /*override*/;
	
		// The identifier components separated by '.'.
		std::vector<std::string> components;

		// What this identifier refers to.
		Datum* binding;
		
	private:
		bool constant_;
	};

	class ASTExprArrow : public ASTExpr
	{
	public:
		ASTExprArrow(ASTExpr* left, std::string const& right,
		             Location const& location);
		ASTExprArrow* clone() const /*override*/ {
			return new ASTExprArrow(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;
		std::string asString() const /*override*/;
		bool isTypeArrow() const /*override*/ {return true;}

		bool isConstant() const /*override*/ {return false;}
		DataType getReadType() const /*override*/;
		DataType getWriteType() const /*override*/;
	
		owning_ptr<ASTExpr> left;
		std::string right;
		owning_ptr<ASTExpr> index;

		ZClass* leftClass;
		Function* readFunction;
		Function* writeFunction;
	};

	class ASTExprIndex : public ASTExpr
	{
	public:
		ASTExprIndex(ASTExpr* array, ASTExpr* index, Location const& location);
		ASTExprIndex* clone() const /*override*/ {
			return new ASTExprIndex(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;
		bool isTypeIndex() const /*override*/ {return true;}
    
		bool isConstant() const /*override*/;
		DataType getReadType() const /*override*/;
		DataType getWriteType() const /*override*/;
	
		owning_ptr<ASTExpr> array;
		owning_ptr<ASTExpr> index;
	};

	class ASTExprCall : public ASTExpr
	{
	public:
		ASTExprCall(Location const& location);
		ASTExprCall* clone() const /*override*/ {
			return new ASTExprCall(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		bool isConstant() const /*override*/ {return false;}
		DataType getReadType() const /*override*/;
	
		owning_ptr<ASTExpr> left;
		owning_vector<ASTExpr> parameters;

		Function* binding;
	};

	// virtual
	class ASTUnaryExpr : public ASTExpr
	{
	public:
		ASTUnaryExpr(Location const& location);
		ASTUnaryExpr* clone() const /*override*/ = 0;

		bool isConstant() const /*override*/ {return operand->isConstant();}

		owning_ptr<ASTExpr> operand;
	};

	class ASTExprNegate : public ASTUnaryExpr
	{
	public:
		ASTExprNegate(Location const& location);
		ASTExprNegate* clone() const /*override*/ {
			return new ASTExprNegate(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		optional<long> getCompileTimeValue(CompileErrorHandler*)
			const /*override*/;
		DataType getReadType() const /*override*/ {
			return DataType::stdFloat;}
	};

	class ASTExprNot : public ASTUnaryExpr
	{
	public:
		ASTExprNot(Location const& location);
		ASTExprNot* clone() const /*override*/ {
			return new ASTExprNot(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		optional<long> getCompileTimeValue(CompileErrorHandler*)
			const /*override*/;
		DataType getReadType() const /*override*/ {
			return DataType::stdBool;}
	};

	class ASTExprBitNot : public ASTUnaryExpr
	{
	public:
		ASTExprBitNot(Location const& location);
		ASTExprBitNot* clone() const /*override*/ {
			return new ASTExprBitNot(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		optional<long> getCompileTimeValue(CompileErrorHandler*)
			const /*override*/;
		DataType getReadType() const /*override*/ {
			return DataType::stdFloat;}
	};

	class ASTExprIncrement : public ASTUnaryExpr
	{
	public:
		ASTExprIncrement(Location const& location);
		ASTExprIncrement* clone() const /*override*/ {
			return new ASTExprIncrement(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		bool isConstant() const /*override*/ {return false;}
		DataType getReadType() const /*override*/ {return DataType::stdFloat;}
		DataType getWriteType() const /*override*/ {
			return operand ? operand->getWriteType() : DataType::stdVoid;}
	};

	class ASTExprPreIncrement : public ASTUnaryExpr
	{
	public:
		ASTExprPreIncrement(Location const& location);
		ASTExprPreIncrement* clone() const /*override*/ {
			return new ASTExprPreIncrement(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		bool isConstant() const /*override*/ {return false;}
		DataType getReadType() const /*override*/ {return DataType::stdFloat;}
		DataType getWriteType() const /*override*/ {
			return operand ? operand->getWriteType() : DataType::stdVoid;}
	};

	class ASTExprDecrement : public ASTUnaryExpr
	{
	public:
		ASTExprDecrement(Location const& location);
		ASTExprDecrement* clone() const /*override*/ {
			return new ASTExprDecrement(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		bool isConstant() const /*override*/ {return false;}
		DataType getReadType() const /*override*/ {
			return DataType::stdFloat;}
		DataType getWriteType() const /*override*/ {
			return operand ? operand->getWriteType() : DataType::stdVoid;}
	};

	class ASTExprPreDecrement : public ASTUnaryExpr
	{
	public:
		ASTExprPreDecrement(Location const& location);
		ASTExprPreDecrement* clone() const /*override*/ {
			return new ASTExprPreDecrement(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		bool isConstant() const /*override*/ {return false;}
		DataType getReadType() const /*override*/ {
			return DataType::stdFloat;}
		DataType getWriteType() const /*override*/ {
			return operand ? operand->getWriteType() : DataType::stdVoid;}
	};

	// virtual
	class ASTBinaryExpr : public ASTExpr
	{
	public:
		ASTBinaryExpr(ASTExpr* left, ASTExpr* right,
		              Location const& location);
		ASTBinaryExpr* clone() const /*override*/ = 0;

		bool isConstant() const /*override*/;

		owning_ptr<ASTExpr> left;
		owning_ptr<ASTExpr> right;
	};

	// virtual
	class ASTLogExpr : public ASTBinaryExpr
	{
	public:
		ASTLogExpr(ASTExpr* left, ASTExpr* right, Location const& location);
		ASTLogExpr* clone() const /*override*/ = 0;

		DataType getReadType() const /*override*/ {return DataType::stdBool;}
	};

	class ASTExprAnd : public ASTLogExpr
	{
	public:
		ASTExprAnd(ASTExpr* left, ASTExpr* right, Location const& location);
		ASTExprAnd* clone() const /*override*/ {
			return new ASTExprAnd(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		optional<long> getCompileTimeValue(CompileErrorHandler*)
			const /*override*/;
	};

	class ASTExprOr : public ASTLogExpr
	{
	public:
		ASTExprOr(ASTExpr* left, ASTExpr* right, Location const& location);
		ASTExprOr* clone() const /*override*/ {return new ASTExprOr(*this);}
    
		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		optional<long> getCompileTimeValue(CompileErrorHandler*)
			const /*override*/;
	};

	// virtual
	class ASTRelExpr : public ASTBinaryExpr
	{
	public:
		ASTRelExpr(ASTExpr* left, ASTExpr* right, Location const& location);
		ASTRelExpr* clone() const /*override*/ = 0;

		DataType getReadType() const /*override*/ {
			return DataType::stdBool;}
	};

	class ASTExprGT : public ASTRelExpr
	{
	public:
		ASTExprGT(ASTExpr* left, ASTExpr* right, Location const& location);
		ASTExprGT* clone() const /*override*/ {return new ASTExprGT(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		optional<long> getCompileTimeValue(CompileErrorHandler*)
			const /*override*/;
	};

	class ASTExprGE : public ASTRelExpr
	{
	public:
		ASTExprGE(ASTExpr* left, ASTExpr* right, Location const& location);
		ASTExprGE* clone() const /*override*/ {return new ASTExprGE(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		optional<long> getCompileTimeValue(CompileErrorHandler*)
			const /*override*/;
	};

	class ASTExprLT : public ASTRelExpr
	{
	public:
		ASTExprLT(ASTExpr* left, ASTExpr* right, Location const& location);
		ASTExprLT* clone() const /*override*/ {return new ASTExprLT(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		optional<long> getCompileTimeValue(CompileErrorHandler*)
			const /*override*/;
	};

	class ASTExprLE : public ASTRelExpr
	{
	public:
		ASTExprLE(ASTExpr* left, ASTExpr* right, Location const& location);
		ASTExprLE* clone() const /*override*/ {return new ASTExprLE(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		optional<long> getCompileTimeValue(CompileErrorHandler*)
			const /*override*/;
	};

	class ASTExprEQ : public ASTRelExpr
	{
	public:
		ASTExprEQ(ASTExpr* left, ASTExpr* right, Location const& location);
		ASTExprEQ* clone() const /*override*/ {return new ASTExprEQ(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		optional<long> getCompileTimeValue(CompileErrorHandler*)
			const /*override*/;
	};

	class ASTExprNE : public ASTRelExpr
	{
	public:
		ASTExprNE(ASTExpr* left, ASTExpr* right, Location const& location);
		ASTExprNE* clone() const /*override*/ {return new ASTExprNE(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		optional<long> getCompileTimeValue(CompileErrorHandler*)
			const /*override*/;
	};

	// virtual
	class ASTAddExpr : public ASTBinaryExpr
	{
	public:
		ASTAddExpr(ASTExpr* left, ASTExpr* right, Location const& location);
		ASTAddExpr* clone() const /*override*/ = 0;

		DataType getReadType() const /*override*/ {
			return DataType::stdFloat;}
	};

	class ASTExprPlus : public ASTAddExpr
	{
	public:
		ASTExprPlus(ASTExpr* left, ASTExpr* right, Location const& location);
		ASTExprPlus* clone() const /*override*/ {
			return new ASTExprPlus(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		optional<long> getCompileTimeValue(CompileErrorHandler*)
			const /*override*/;
	};

	class ASTExprMinus : public ASTAddExpr
	{
	public:
		ASTExprMinus(ASTExpr* left, ASTExpr* right,
		             Location const& location);
		ASTExprMinus* clone() const /*override*/ {
			return new ASTExprMinus(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		optional<long> getCompileTimeValue(CompileErrorHandler*)
			const /*override*/;
	};

	// virtual
	class ASTMultExpr : public ASTBinaryExpr
	{
	public:
		ASTMultExpr(ASTExpr* left, ASTExpr* right, Location const& location);
		ASTMultExpr* clone() const /*override*/ = 0;

		DataType getReadType() const /*override*/ {
			return DataType::stdFloat;}
	};

	class ASTExprTimes : public ASTMultExpr
	{
	public:
		ASTExprTimes(ASTExpr* left, ASTExpr* right,
		             Location const& location);
		ASTExprTimes* clone() const /*override*/ {
			return new ASTExprTimes(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		optional<long> getCompileTimeValue(CompileErrorHandler*)
			const /*override*/;
	};

	class ASTExprDivide : public ASTMultExpr
	{
	public:
		ASTExprDivide(ASTExpr* left, ASTExpr* right,
		              Location const& location);
		ASTExprDivide* clone() const /*override*/ {
			return new ASTExprDivide(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		optional<long> getCompileTimeValue(CompileErrorHandler*)
			const /*override*/;
	};

	class ASTExprModulo : public ASTMultExpr
	{
	public:
		ASTExprModulo(ASTExpr* left, ASTExpr* right,
		              Location const& location);
		ASTExprModulo* clone() const /*override*/ {
			return new ASTExprModulo(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		optional<long> getCompileTimeValue(CompileErrorHandler*)
			const /*override*/;
	};

	// virtual
	class ASTBitExpr : public ASTBinaryExpr
	{
	public:
		ASTBitExpr(ASTExpr* left, ASTExpr* right, Location const& location);
		ASTBitExpr* clone() const /*override*/ = 0;

		DataType getReadType() const /*override*/ {
			return DataType::stdFloat;}
	};

	class ASTExprBitAnd : public ASTBitExpr
	{
	public:
		ASTExprBitAnd(ASTExpr* left, ASTExpr* right,
		              Location const& location);
		ASTExprBitAnd* clone() const /*override*/ {
			return new ASTExprBitAnd(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		optional<long> getCompileTimeValue(CompileErrorHandler*)
			const /*override*/;
	};

	class ASTExprBitOr : public ASTBitExpr
	{
	public:
		ASTExprBitOr(ASTExpr* left, ASTExpr* right,
		             Location const& location);
		ASTExprBitOr* clone() const /*override*/ {
			return new ASTExprBitOr(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		optional<long> getCompileTimeValue(CompileErrorHandler*)
			const /*override*/;
	};

	class ASTExprBitXor : public ASTBitExpr
	{
	public:
		ASTExprBitXor(ASTExpr* left, ASTExpr* right,
		              Location const& location);
		ASTExprBitXor* clone() const /*override*/{
			return new ASTExprBitXor(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		optional<long> getCompileTimeValue(CompileErrorHandler*)
			const /*override*/;
	};

	// virtual
	class ASTShiftExpr : public ASTBinaryExpr
	{
	public:
		ASTShiftExpr(ASTExpr* left, ASTExpr* right,
		             Location const& location);
		ASTShiftExpr* clone() const /*override*/ = 0;

		DataType getReadType() const /*override*/ {
			return DataType::stdFloat;}
	};

	class ASTExprLShift : public ASTShiftExpr
	{
	public:
		ASTExprLShift(ASTExpr* left, ASTExpr* right,
		              Location const& location);
		ASTExprLShift* clone() const /*override*/ {
			return new ASTExprLShift(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		optional<long> getCompileTimeValue(CompileErrorHandler*)
			const /*override*/;
	};

	class ASTExprRShift : public ASTShiftExpr
	{
	public:
		ASTExprRShift(ASTExpr* left, ASTExpr* right,
		              Location const& location);
		ASTExprRShift* clone() const /*override*/ {
			return new ASTExprRShift(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		optional<long> getCompileTimeValue(CompileErrorHandler*)
			const /*override*/;
	};

	// Literals

	// virtual
	class ASTLiteral : public ASTExpr
	{
	public:
		ASTLiteral(Location const& location);
		ASTLiteral* clone() const /*override*/ = 0;

		Literal* manager;
	};

	class ASTNumberLiteral : public ASTLiteral
	{
	public:
		ASTNumberLiteral(ASTFloat* value, Location const& location);
		ASTNumberLiteral* clone() const /*override*/ {
			return new ASTNumberLiteral(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		bool isConstant() const /*override*/ {return true;}
		optional<long> getCompileTimeValue(CompileErrorHandler*)
			const /*override*/;
		DataType getReadType() const /*override*/ {
			return DataType::stdFloat;}
	
		owning_ptr<ASTFloat> value;
	};

	class ASTBoolLiteral : public ASTLiteral
	{
	public:
		ASTBoolLiteral(bool value, Location const& location);
		ASTBoolLiteral* clone() const /*override*/ {
			return new ASTBoolLiteral(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		bool isConstant() const /*override*/ {return true;}
		optional<long> getCompileTimeValue(CompileErrorHandler*)
			const /*override*/ {
			return value ? 10000L : 0L;}
		DataType getReadType() const /*override*/ {
			return DataType::stdBool;}
	
		bool value;
	};

	class ASTStringLiteral : public ASTLiteral
	{
	public:
		ASTStringLiteral(char const* str, Location const& location);
		ASTStringLiteral(std::string const& str, Location const& location);
		ASTStringLiteral(ASTString const& raw);
		ASTStringLiteral(ASTStringLiteral const& base);
		ASTStringLiteral& operator=(ASTStringLiteral const& rhs);
		ASTStringLiteral* clone() const /*override*/ {
			return new ASTStringLiteral(*this);}

		void execute (ASTVisitor& visitor, void* param = NULL) /*override*/;
		bool isStringLiteral() const /*override*/ {return true;}

		bool isConstant() const /*override*/ {return true;}

		DataType getReadType() const /*override*/ {
			return DataType::stdAryFloat;}
		
		// The data declaration that this literal may be part of. If NULL that
		// means this is not part of a data declaration. This should be managed by
		// that declaration and not modified by this object at all.
		ASTDataDecl* declaration;

		std::string value;
	};

	class ASTArrayLiteral : public ASTLiteral
	{
	public:
		ASTArrayLiteral(Location const& location);
		ASTArrayLiteral(ASTArrayLiteral const& base);
		ASTArrayLiteral& operator=(ASTArrayLiteral const& rhs);
		ASTArrayLiteral* clone() const {return new ASTArrayLiteral(*this);}

		void execute (ASTVisitor& visitor, void* param = NULL) /*override*/;
		bool isArrayLiteral() const /*override*/ {return true;}

		bool isConstant() const /*override*/ {return true;}
		DataType getReadType() const /*override*/ {return readType_;}
		void setReadType(DataType type) {readType_ = type;}

		// The data declaration that this literal may be part of. If NULL that
		// means this is not part of a data declaration. This should be managed by
		// that declaration and not modified by this object at all.
		ASTDataDecl* declaration;

		// Optional type specification.
		owning_ptr<ASTDataType> type;
		// Optional size specification.
		owning_ptr<ASTExpr> size;
		// The array elements.
		owning_vector<ASTExpr> elements;

	private:
		// Cached read type.
		DataType readType_;
	};

	class ASTOptionValue : public ASTLiteral
	{
	public:
		ASTOptionValue(std::string const& name, Location const& location);
		ASTOptionValue* clone() const /*override*/ {
			return new ASTOptionValue(*this);}

		virtual void execute(ASTVisitor& visitor, void* param = NULL)
			/*override*/;
		virtual std::string asString() const /*override*/;

		virtual bool isConstant() const /*override*/ {return true;}
		virtual DataType getReadType() const /*override*/ {
			return DataType::stdFloat;}

		std::string name;
		CompileOption option;
		optional<long> value;
	};

	// Types

	class ASTScriptType : public AST
	{
	public:
		ASTScriptType(ScriptType type, Location const& location);
		ASTScriptType(std::string const& name, Location const& location);
		ASTScriptType* clone() const /*override*/ {
			return new ASTScriptType(*this);}

		void execute(ASTVisitor& visitor, void* param = NULL);

		// If type is invalid, try to resolve using name.
		ScriptType type;
		std::string name;
	};

	ScriptType resolveScriptType(ASTScriptType const&, Scope const&);

	class ASTDataType : public AST
	{
	public:
		ASTDataType(DataType type, Location const& location);
		ASTDataType(std::string const& name, Location const& location);
		ASTDataType* clone() const /*override*/ {
			return new ASTDataType(*this);}
	
		void execute(ASTVisitor& visitor, void* param = NULL) /*override*/;

		// If type is invalid, try to resolve using name.
		DataType type;
		std::string name;
	};

	DataType resolveDataType(ASTDataType const&, Scope const&);
}

#endif

