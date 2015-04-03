#include "SExpr.h"

#include <ctype.h>
#include <iostream>
#include <algorithm>

const char SExpr::special[] = {' ', '(', ')', '\r', '\n', '\t'};

SExpr::SExpr(const std::vector<SExpr>& l) :
    atom(false),
    null(false),
    name(""),
    list(l)
{ }

SExpr::SExpr(const std::string& n) :
    atom(true),
    null(false),
    name(n),
    list()
{ }

// Constructor for null SExpression
SExpr::SExpr() :
    atom(false),
    null(true),
    name(""),
    list()
{ }

SExpr SExpr::get(unsigned int i)
{
    if (i<list.size())
    {
        return list[i];
    }
    SExpr null;
    return null;
}

void SExpr::append(const std::vector<SExpr>& l)
{
    if (atom) return;
    null = false;

    list.insert(list.end(), l.begin(), l.end());
}

void SExpr::append(const SExpr s)
{
    if (atom) return;
    null = false;

    list.push_back(s);
}

std::string SExpr::toString()
{
    // MAY have to deal with special chars (I don't think so though..)
    if (atom)
    {
        return name;
    }

    std::string s = "(";
    for (unsigned int i=0; i<list.size(); i++) {
        if (s.length() > 1)
            s += ' ';
        s += list[i].toString();
    }
    s += ")";

    return s;
}

std::string SExpr::print(int indent, int lineLimit, int level)
{
    std::string prefix(indent * level, ' ');
    std::string s = toString();
    if (atom || (int)s.length() + indent * level <= lineLimit)
        return prefix + s;

    s = prefix + '(';
    for (unsigned int i = 0; i < list.size(); i++)
    {
        std::string e = list[i].print(indent, lineLimit, level+1);
        if (i>0)
        {
            s += "\n";
            s += e;
        }
        else
            s += e.substr(indent * level + 1);
    }
    s += ')';
    return s;
}

SExpr SExpr::find(std::string name)
{
    if (!atom)
    {
        for (unsigned int i = 0; i<list.size(); i++)
        {
            SExpr exp = list[i];
            if (exp.count() > 0 && exp.get(0).isAtom()
                && exp.get(0).getName() == name)
                return exp;
        }
    }

    SExpr null;
    return null;
}

SExpr SExpr::read(std::string s, int& p)
{
    while (p < (int)s.length() && isspace(s[p]))
        p++;

    unsigned int q;
    SExpr se;

    switch (s[p]) {
    case '"': { // need braces to scope the vars below blea
        std::string name;
        while (p < (int)s.length() && s[p] == '"')
        {
            name += '"';
            q = s.find('"', ++p);
            if (q == std::string::npos)
                q = s.size();
            name += s.substr(p, q - p);
            if (q < s.size())
                q++;
            p = q;
            name += '"';
        }
        SExpr exp(name);
        return exp;
    }
    case '(': {
        p++;

        while (true)
        {
            SExpr e = read(s, p);
            if (e.isNull())
                break;
            se.append(e);
        }
        if (p < (int)s.length() && s[p] == ')')
            p++;
        return se;
    }
    case ')':
        return se;

    default:
        q = findSpecialChar(s, p);
        if (q == std::string::npos)
            q = s.length();
        SExpr se(s.substr(p, q - p));
        p = q;
        return se;
    }
}

int SExpr::findSpecialChar(std::string s, int p)
{
    unsigned int position = std::string::npos;

    for (int i=0; i<6; i++)
    {
        unsigned int f = s.find(special[i], p);
        if (f == std::string::npos)
            continue;

        if (position == std::string::npos || f < position)
            position = f;
    }

    return position;
}


int main(int argc, char** argv)
{
    std::string test = "(image (height (hurry hurry 55) (\"W(HY)??? \" lolajk???sdalfjksd?? )) (width 99))";
    int i = 0;
    SExpr s = SExpr::read(test, i);
    std::cout << "Finding" << std::endl;
    SExpr found = s.find("height").find("\"W(HY)??? \"");

    std::cout << s.print() << std::endl << std::endl;
    std::cout << found.print() << std::endl;
}
