#pragma once

#include <string>
#include <vector>

class SExpr {
public:
    SExpr(const std::vector<SExpr>& l);
    SExpr(const std::string& n);
    SExpr();

    bool isAtom() { return atom; }
    bool isNull() { return null; }
    int count() { return atom ? -1 : list.size(); }

    std::string getName() { return name; }
    SExpr get(unsigned int i);

    // Adds the given list/single of SExpressions to this list
    // No effect if an atom
    void append(const std::vector<SExpr>& l);
    void append(const SExpr s);

    // String representation of this SExpression (and nested ones)
    std::string toString();
    // Nicely formatted SExpression (more human-readable)
    std::string print(int indent = 2, int lineLimit = 64, int level = 0);

    // returns SExpression that is a list who's first expression is an atom
    // with the same name as given string
    // For searching SExpression for a given key
    SExpr find(std::string name);

    // Translate the given string into an SExpression
    static SExpr read(std::string s, int& p);

    // Special chars that we need to look out for
    static const char special[];
    // Returns index of FIRST special char in string. returns string::npos if none found
    static int findSpecialChar(std::string s, int p);
private:
    // True if an atom. Implies null=false, list->empty
    bool atom;
    // A null (blank) SExpression. Implies atom=false, list->empty
    bool null;
    // Describes the expression, only non-empty if atom
    std::string name;
    // List of nested expressions, only non-empty if not atom
    std::vector<SExpr> list;

};
