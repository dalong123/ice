// **********************************************************************
//
// Copyright (c) 2002
// ZeroC, Inc.
// Billerica, MA, USA
//
// All Rights Reserved.
//
// Ice is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License version 2 as published by
// the Free Software Foundation.
//
// **********************************************************************

#include <IceUtil/Functional.h>
#include <IceUtil/OutputUtil.h>

#include <Slice/CPlusPlusUtil.h> // TODO: ???

#include <Gen.h>

using namespace std;
using namespace Slice;
using namespace IceUtil;

static const string internalId = "_internal.";

Slice::Gen::Gen(const string& name, const string& base, const string& include,
		const vector<string>& includePaths, const string& dir) :
    _name(name),
    _base(base),
    _include(include),
    _includePaths(includePaths),
    _dir(dir)
{
    _orgName = "http://www.noorg.org"; // TODO: argument!
    for(vector<string>::iterator p = _includePaths.begin(); p != _includePaths.end(); ++p)
    {
	if(p->length() && (*p)[p->length() - 1] != '/')
	{
	    *p += '/';
	}
    }

    string::size_type pos = _base.rfind('/');
    if(pos != string::npos)
    {
	_base.erase(0, pos + 1);
    }
}

Slice::Gen::~Gen()
{
}

bool
Slice::Gen::visitClassDefStart(const ClassDefPtr& p)
{
    XMLOutput O;

    //string fileO = _base + ".wsdl";
    string fileO = containedToId(p) + p->name() + ".wsdl";
    if(!_dir.empty())
    {
	fileO = _dir + '/' + fileO;
    }

    O.open(fileO.c_str());
    if(!O)
    {
	cerr << _name << ": can't open `" << fileO << "' for writing: " << strerror(errno) << endl;
	return false;
    }

    printHeader(O);
    O << "\n<!-- Generated from file `" << changeInclude(_base, _includePaths) << ".ice' -->\n";

    string scopeId = containedToId(p);

    //
    // TODO: It would be better if start() aligned the attributes
    // correctly.
    //
    ostringstream os;
    os << "wsdl:definitions name=\"" << scopeId << p->name() << "\""
       << "\n                 xmlns:wsdl=\"http://schemas.xmlsoap.org/wsdl/\""
       << "\n                 xmlns:xsd1=\"" << _orgName << "/schemas\""
       << "\n                 xmlns:tns=\"" << _orgName << "/definitions\""
       << "\n                 targetNamespace=\"" << _orgName << "/definitions\"";

    O << se(os.str());

    // TODO: schemaLocation?
    O << sp << nl << "<wsdl:import namespace=\"" << _orgName << "/schemas\" location=\"" << _base << ".xsd\"/>";


    OperationList ops = p->allOperations();
    for(OperationList::const_iterator q = ops.begin(); q != ops.end(); ++q)
    {
	emitMessage(O, *q);
    }

    O << sp;

    os.str("");
    os << "wsdl:portType name=\"" << scopeId << "PortType\"";
    O << se(os.str());

    for(OperationList::const_iterator r = ops.begin(); r != ops.end(); ++r)
    {
	emitOperation(O, *r);
    }

    O << ee; // PortType

    O << ee; // definitions

    return false;
}

void
Slice::Gen::emitMessage(XMLOutput& O, const OperationPtr& p)
{
    O << sp;

    string scopeId = containedToId(p);
    
    ostringstream os;
    os << "wsdl:message name=\"input." << p->name() << "\"";
    O << se(os.str());

    O << nl << "<wsdl:part name=\"body\" element=\"xsd1:" << scopeId << "request." << p->name() << "\"/>";

    O << ee; // message

    os.str("");
    os << "wsdl:message name=\"output." << p->name() << "\"";
    O << se(os.str());

    O << nl << "<wsdl:part name=\"body\" element=\"xsd1:" << scopeId << "reply." << p->name() << "\"/>";

    O << ee; // message

    os.str("");
    os << "wsdl:message name=\"metadata." << p->name() << "\"";
    O << se(os.str());

    list<string> metaData = p->getMetaData();
    for(list<string>::iterator iter = metaData.begin(); iter != metaData.end(); ++iter)
    {
        O << nl << "<wsdl:part name=\"" << *iter << "\"/>";
    }

    O << ee; // message
}

void
Slice::Gen::emitOperation(XMLOutput& O, const OperationPtr& p)
{
    string scopeId = containedToId(p);
    
    ostringstream os;
    os << "wsdl:operation name=\"" << p->name() << "\"";
    O << se(os.str());

    O << nl << "<wsdl:input message=\"tns:input." << p->name() << "\"/>";
    O << nl << "<wsdl:output message=\"tns:output." << p->name() << "\"/>";

    O << ee; // operation
}

void
Slice::Gen::printHeader(XMLOutput& O)
{
    static const char* header =
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<!--\n"
"**********************************************************************\n"
"Copyright (c) 2002\n"
"ZeroC, Inc.\n"
"Billerica, MA, USA\n"
"\n"
"All Rights Reserved.\n"
"\n"
"Ice is free software; you can redistribute it and/or modify it under\n"
"the terms of the GNU General Public License version 2 as published by\n"
"the Free Software Foundation.\n"
"\n"
"Generated by the `slice2wsdl' converter\n"
"**********************************************************************\n"
"-->";
    
    O.zeroIndent();
    O << header;
    O << "\n<!-- Ice version " << ICE_STRING_VERSION << " -->";
    O.restoreIndent();
}


string
Slice::Gen::containedToId(const ContainedPtr& contained)
{
    assert(contained);

    string scoped = contained->scope();
    if(scoped[0] == ':')
    {
	scoped.erase(0, 2);
    }

    string id;

    id.reserve(scoped.size());

    for(unsigned int i = 0; i < scoped.size(); ++i)
    {
	if(scoped[i] == ':')
	{
	    id += '.';
	    ++i;
	}
	else
	{
	    id += scoped[i];
	}
    }

    return id;
}
