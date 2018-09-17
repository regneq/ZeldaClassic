#include "../precompiled.h"
#include "ZScript.h"

#include <sstream>
#include "CompilerUtils.h"
#include "CompileError.h"
#include "DataStructs.h"
#include "GlobalSymbols.h"
#include "Types.h"
#include "Scope.h"

using namespace std;
using namespace ZScript;

////////////////////////////////////////////////////////////////
// ZScript::Program

Program::Program(ASTFile& root, CompileErrorHandler* errorHandler)
	: root_(root), rootScope_(new RootScope())
{
	// Create the ~Init script.
	if (Script* initScript =
	    	createScript(
				*this, *rootScope_, ScriptType::global,
				"~Init", errorHandler))
	{
		scripts.push_back(initScript);
		scriptsByName_[initScript->getName()] = initScript;
	}
}

Program::~Program()
{
	deleteElements(scripts);
	delete rootScope_;
}

Script* Program::getScript(string const& name) const
{
	return find<Script*>(scriptsByName_, name).value_or(NULL);
}

Script* Program::getScript(ASTScript* node) const
{
	return find<Script*>(scriptsByNode_, node).value_or(NULL);
}

Script* Program::addScript(
		ASTScript& node, Scope& parentScope, CompileErrorHandler* handler)
{
	Script* script = createScript(*this, parentScope, node, handler);
	if (!script) return NULL;

	scripts.push_back(script);
	scriptsByName_[script->getName()] = script;
	scriptsByNode_[&node] = script;
	return script;
}

vector<Function*> Program::getUserGlobalFunctions() const
{
	vector<Function*> functions = rootScope_->getLocalFunctions();
	for (vector<Function*>::iterator it = functions.begin();
	     it != functions.end();)
	{
		Function& function = **it;
		if (!function.node) it = functions.erase(it);
		else ++it;
	}
	return functions;
}

vector<Function*> Program::getUserFunctions() const
{
	vector<Function*> functions = getFunctions(*this);
	for (vector<Function*>::iterator it = functions.begin();
	     it != functions.end();)
	{
		Function& function = **it;
		if (!function.node) it = functions.erase(it);
		else ++it;
	}
	return functions;
}

vector<Function*> ZScript::getFunctions(Program const& program)
{
	vector<Function*> functions = getFunctionsInBranch(program.getScope());

	vector<ZClass*> classes = getClassesInBranch(program.getScope());
	for (vector<ZClass*>::const_iterator it = classes.begin();
	     it != classes.end(); ++it)
	{
		Scope const& scope = (*it)->getScope();
		appendElements(functions, scope.getLocalFunctions());
		appendElements(functions, scope.getLocalGetters());
		appendElements(functions, scope.getLocalSetters());
	}
	
	return functions;
}

////////////////////////////////////////////////////////////////
// ZScript::Script

// ZScript::Script

Script::Script(Program& program) : program(program) {}

Script::~Script()
{
	deleteElements(code);
}

// ZScript::UserScript

UserScript::UserScript(Program& program, ASTScript& node)
	: Script(program), node(node)
{}

ScriptType UserScript::getType() const
{
	return resolveScriptType(*node.type, *scope->getParent());
}

// ZScript::BuiltinScript

BuiltinScript::BuiltinScript(
		Program& program, ScriptType type, string const& name)
	: Script(program), type(type), name(name)
{}

// ZScript

UserScript* ZScript::createScript(
		Program& program, Scope& parentScope, ASTScript& node,
		CompileErrorHandler* errorHandler)
{
	UserScript* script = new UserScript(program, node);

	ScriptScope* scope = parentScope.makeScriptChild(*script);
	if (!scope)
	{
		if (errorHandler)
			errorHandler->handleError(
				CompileError::ScriptRedef(&node, node.name));
		delete script;
		return NULL;
	}
	script->scope = scope;

	if (!resolveScriptType(*node.type, parentScope).isValid())
	{
		if (errorHandler)
			errorHandler->handleError(
				CompileError::ScriptBadType(&node, node.name));
		delete script;
		return NULL;
	}

	return script;
}

BuiltinScript* ZScript::createScript(
		Program& program, Scope& parentScope, ScriptType type,
		string const& name, CompileErrorHandler* errorHandler)
{
	BuiltinScript* script = new BuiltinScript(program, type, name);

	ScriptScope* scope = parentScope.makeScriptChild(*script);
	if (!scope)
	{
		if (errorHandler)
			errorHandler->handleError(CompileError::ScriptRedef(NULL, name));
		delete script;
		return NULL;
	}
	script->scope = scope;

	if (!type.isValid())
	{
		if (errorHandler)
			errorHandler->handleError(
				CompileError::ScriptBadType(NULL, name));
		delete script;
		return NULL;
	}

	return script;
}

Function* ZScript::getRunFunction(Script const& script)
{
	return getOnly<Function*>(script.getScope().getLocalFunctions("run"))
		.value_or(NULL);
}

optional<int> ZScript::getLabel(Script const& script)
{
	if (Function* run = getRunFunction(script))
		return run->getLabel();
	return nullopt;
}

////////////////////////////////////////////////////////////////
// ZScript::Datum

Datum::Datum(Scope& scope, DataType type)
	: scope(scope), type(type), id(ScriptParser::getUniqueVarID())
{}

bool Datum::tryAddToScope(CompileErrorHandler* errorHandler)
{
	return scope.add(*this, errorHandler);
}

bool ZScript::isGlobal(Datum const& datum)
{
	return (datum.scope.isGlobal() || datum.scope.isScript())
		&& datum.getName();
}

optional<int> ZScript::getStackOffset(Datum const& datum)
{
	return lookupStackPosition(datum.scope, datum);
}

// ZScript::Literal

Literal* Literal::create(
		Scope& scope, ASTLiteral& node, DataType const& type,
		CompileErrorHandler* errorHandler)
{
	Literal* literal = new Literal(scope, node, type);
	if (literal->tryAddToScope(errorHandler)) return literal;
	delete literal;
	return NULL;
}

Literal::Literal(Scope& scope, ASTLiteral& node, DataType const& type)
	: Datum(scope, type), node(node)
{
	node.manager = this;
}

// ZScript::Variable

Variable* Variable::create(
		Scope& scope, ASTDataDecl& node, DataType const& type,
		CompileErrorHandler* errorHandler)
{
	Variable* variable = new Variable(scope, node, type);
	if (variable->tryAddToScope(errorHandler)) return variable;
	delete variable;
	return NULL;
}

Variable::Variable(
		Scope& scope, ASTDataDecl& node, DataType const& type)
	: Datum(scope, type),
	  node(node),
	  globalId((scope.isGlobal() || scope.isScript())
	           ? optional<int>(ScriptParser::getUniqueGlobalID())
	           : nullopt)
{
	node.manager = this;
}

// ZScript::BuiltinVariable

BuiltinVariable* BuiltinVariable::create(
		Scope& scope, DataType const& type, string const& name,
		CompileErrorHandler* errorHandler)
{
	BuiltinVariable* builtin = new BuiltinVariable(scope, type, name);
	if (builtin->tryAddToScope(errorHandler)) return builtin;
	delete builtin;
	return NULL;
}

BuiltinVariable::BuiltinVariable(
		Scope& scope, DataType const& type, string const& name)
	: Datum(scope, type),
	  name(name),
	  globalId((scope.isGlobal() || scope.isScript())
	           ? optional<int>(ScriptParser::getUniqueGlobalID())
	           : nullopt)
{}

// ZScript::Constant

Constant* Constant::create(
		Scope& scope, ASTDataDecl& node, DataType const& type, long value,
		CompileErrorHandler* errorHandler)
{
	Constant* constant = new Constant(scope, node, type, value);
	if (constant->tryAddToScope(errorHandler)) return constant;
	delete constant;
	return NULL;
}

Constant::Constant(
		Scope& scope, ASTDataDecl& node, DataType const& type, long value)
	: Datum(scope, type), node(node), value(value)
{
	node.manager = this;
}

optional<string> Constant::getName() const {return node.name;}

// ZScript::BuiltinConstant


BuiltinConstant* BuiltinConstant::create(
		Scope& scope, DataType const& type, string const& name, long value,
		CompileErrorHandler* errorHandler)
{
	BuiltinConstant* builtin = new BuiltinConstant(scope, type, name, value);
	if (builtin->tryAddToScope(errorHandler)) return builtin;
	delete builtin;
	return NULL;
}

BuiltinConstant::BuiltinConstant(
		Scope& scope, DataType const& type, string const& name, long value)
	: Datum(scope, type), name(name), value(value)
{}

// ZScript::FunctionSignature

FunctionSignature::FunctionSignature(
		string const& name, vector<DataType> const& parameterTypes)
	: name(name), parameterTypes(parameterTypes)
{}

FunctionSignature::FunctionSignature(Function const& function)
	: name(function.name), parameterTypes(function.paramTypes)
{}
		
int FunctionSignature::compare(FunctionSignature const& rhs) const
{
	int c = name.compare(rhs.name);
	if (c) return c;
	c = parameterTypes.size() - rhs.parameterTypes.size();
	if (c) return c;
	for (int i = 0; i < (int)parameterTypes.size(); ++i)
	{
		c = parameterTypes[i].compare(rhs.parameterTypes[i]);
		if (c) return c;
	}
	return 0;
}

string FunctionSignature::asString() const
{
	ostringstream oss;
	oss << name << "(";
	for (vector<DataType>::const_iterator it = parameterTypes.begin();
		 it != parameterTypes.end(); ++it)
	{
		if (it != parameterTypes.begin()) oss << ", ";
		oss << it->toString();
	}
	oss << ")";
	return oss.str();
}

bool ZScript::operator==(
		FunctionSignature const& lhs, FunctionSignature const& rhs)
{
	return lhs.compare(rhs) == 0;
}

bool ZScript::operator!=(
		FunctionSignature const& lhs, FunctionSignature const& rhs)
{
	return lhs.compare(rhs) != 0;
}

bool ZScript::operator<=(
		FunctionSignature const& lhs, FunctionSignature const& rhs)
{
	return lhs.compare(rhs) <= 0;
}

bool ZScript::operator<(
		FunctionSignature const& lhs, FunctionSignature const& rhs)
{
	return lhs.compare(rhs) < 0;
}

bool ZScript::operator>=(
		FunctionSignature const& lhs, FunctionSignature const& rhs)
{
	return lhs.compare(rhs) >= 0;
}

bool ZScript::operator>(
		FunctionSignature const& lhs, FunctionSignature const& rhs)
{
	return lhs.compare(rhs) > 0;
}

// ZScript::Function

Function::Function(DataType returnType, string const& name,
				   vector<DataType> const& paramTypes, int id)
	: node(NULL), internalScope(NULL), thisVar(NULL),
	  returnType(returnType), name(name), paramTypes(paramTypes),
	  id(id), label_(nullopt)
{}

Function::~Function()
{
	deleteElements(ownedCode_);
}

vector<Opcode*> Function::takeCode()
{
	vector<Opcode*> code = ownedCode_;
	ownedCode_.clear();
	return code;
}

void Function::giveCode(vector<Opcode*>& code)
{
	appendElements(ownedCode_, code);
	code.clear();
}

Script* Function::getScript() const
{
	if (!internalScope) return NULL;
	Scope* parentScope = internalScope->getParent();
	if (!parentScope) return NULL;
	if (!parentScope->isScript()) return NULL;
	ScriptScope* scriptScope =
		dynamic_cast<ScriptScope*>(parentScope);
	return &scriptScope->script;
}

int Function::getLabel() const
{
	if (!label_) label_ = ScriptParser::getUniqueLabelID();
	return *label_;
}

void Function::clearLabel()
{
	label_ = nullopt;
}

bool Function::isTracing() const
{
	std::string prefix = name.substr(0, 5);
	return returnType == DataType::stdVoid
		&& (prefix == "Trace" || prefix == "print");
}

bool ZScript::isRun(Function const& function)
{
	return function.internalScope->getParent()->isScript()
		&& function.returnType == DataType::stdVoid
		&& function.name == "run";
}

int ZScript::getStackSize(Function const& function)
{
	return *lookupStackSize(*function.internalScope);
}

int ZScript::getParameterCount(Function const& function)
{
	return function.paramTypes.size() + (isRun(function) ? 1 : 0);
}

////////////////////////////////////////////////////////////////
// ZClass

vector<ZClass*> ZClass::std_;

void ZClass::generateStandard()
{
	deleteElements(std_);
	std_.clear();
#	define X(NAME, TYPE) \
	std_.push_back(new ZClass(#NAME, DataType::std##NAME));
#	include "classes.xtable"
#	undef X
}

void ZClass::clearStandard()
{
	std_.clear();
}

ZClass* ZClass::getStandard(StdId id)
{
	return std_[int(id)];
}

ZClass::ZClass(string const& name, DataType const& type)
		: name(name), scope_(new ClassScope())
{
	LibrarySymbols::getTypeInstance(type)->addSymbolsToScope(*scope_);
}

ZClass::~ZClass()
{
	delete scope_;
}
