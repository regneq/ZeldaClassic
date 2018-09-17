#ifndef GLOBALSYMBOLS_H //2.53 Updated to 16th Jan, 2017
#define GLOBALSYMBOLS_H

#include "DataStructs.h"
#include <string>
#include <map>
#include <vector>

using std::string;
using std::map;
using std::vector;
using namespace ZScript;

class SymbolTable;
namespace ZScript
{
	class Function;
	class Scope;
}

static const int SETTER = 0;
static const int GETTER = 1;
static const int FUNCTION = 2;

struct AccessorTable
{
    string name;
    int rettype;
    int setorget;
    int var;
    int numindex;
    int params[20];
};

class LibrarySymbols
{
public:
	static LibrarySymbols* getTypeInstance(ZScript::DataType type);

	virtual void addSymbolsToScope(ZScript::Scope& scope);
    virtual ~LibrarySymbols();

protected:
    AccessorTable *table;
	LibrarySymbols() : refVar(0) {}
    int refVar;

	ZScript::Function* getFunction(string const& name) const;

private:
    map<string, ZScript::Function*> functions;
	
	// Generates the code for functions which can't be auto generated.
	virtual void generateCode() = 0;
};

class GlobalSymbols : public LibrarySymbols
{
public:
    static GlobalSymbols& getInst()
    {
	    static GlobalSymbols instance;
        return instance;
    }
private:
    GlobalSymbols();
	void generateCode();
};

#define X(NAME, TYPE) \
class NAME##Symbols : public LibrarySymbols \
{ \
public: \
	static NAME##Symbols& getInst() \
	{ \
		static NAME##Symbols instance; \
		return instance; \
	} \
 \
private: \
	NAME##Symbols(); \
	void generateCode(); \
};
#include "classes.xtable"
#undef X

#endif


