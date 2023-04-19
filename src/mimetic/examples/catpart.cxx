/***************************************************************************
    copyright            : (C) 2002-2008 by Stefano Barbato
    email                : stefano@codesink.org

    $Id: catpart.cxx,v 1.3 2008-10-07 11:06:25 tat Exp $
 ***************************************************************************/

/** \example catpart.cc
 *  extract a Part based on command line parameters
 *  more info on:
 *      catpart -h
 */ 
#include <iostream>
#include <sstream>
#include <iterator>
#include <fstream>
#include <cassert>
#include <mimetic/mimetic.h>

using namespace std;
using namespace mimetic;

unsigned int g_matches = 0;

void usage()
{
    cout << "catpart [params] [in_file...]" << endl;
    cout << "\t-r                look for matches in nested entities"<<endl;
    cout << "\t-t type[/subtype] matches Content-Type" << endl;
    cout << "\t-p param[=value]  matches Content-Type param" << endl;
    cout << "\t-f name[=value]   matches an header field" << endl;
    cout << "\t-l                don't write anything but filename" << endl;
    cout << "\t-q                totaly quiet; exit code = num of matches" << endl;
    cout << "\t-H                ignore header fields" << endl;
    cout << "\t-B                ignore entities' body" << endl;
    cout << "\t-C                ignore child entities" << endl;
    exit(-1);
}


struct MatchParamRq
{
    typedef pair<istring,istring> Param;
    typedef pair<istring,istring> Field;
    MatchParamRq()
    : recursive(0), justFilename(0), quiet(0)
    {
    }
    bool operator()(const MimeEntity* pMe) const
    {
        // check for content type match
        const Header& h = pMe->header();
        if(type.length() && type != h.contentType().type())
            return false;
        if(subtype.length() && subtype != h.contentType().subtype())
            return false;

        bool matched;
        // check for params matches
        if(paramList.size())
        {
            ContentType::ParamList::const_iterator ctpbit,ctpeit;
            ctpbit = h.contentType().paramList().begin();
            ctpeit = h.contentType().paramList().end();
            matched = 0;
            for(; !matched && ctpbit != ctpeit; ++ctpbit)
            {
                list<Param>::const_iterator pbit, peit;
                pbit = paramList.begin(), peit = paramList.end();
                for(; pbit != peit; ++pbit)
                {
                    if(ctpbit->name() != pbit->first)
                        break;
                    // case insensitive
                    istring value = ctpbit->value();
                    if(value.find(pbit->second) != string::npos)
                        matched++;
                }
            }
            if(!matched)
                return false;
        }

        // check for field matches
        if(fieldList.size())
        {
            matched = 0;
            Header::const_iterator hbit, heit;
            hbit = h.begin();
            heit = h.end();
            for(; !matched && hbit != heit; ++hbit)
            {
                list<Field>::const_iterator fbit, feit;
                fbit = fieldList.begin(), feit = fieldList.end();
                for(; fbit != feit; ++fbit)
                {
                    if(hbit->name() != fbit->first)
                        break;
                    // case insensitive
                    istring value = hbit->value();
                    if(value.find(fbit->second) != string::npos)
                        matched++;
                }
            }
            if(!matched)
                return false;
        }

        return true;
        
    }
    istring type, subtype;
    list<Param> paramList;
    list<Field> fieldList;
    bool recursive, justFilename, quiet;
};

void die(bool b, const string& msg)
{
    if(b)
    {
        cerr << "Error: " << msg << endl << endl;
        usage();
    }
}

void printPart(const MimeEntity& me, const MatchParamRq& mpr, string& fqn)
{
    while(mpr(&me))
    {
        ++g_matches;
        if(mpr.quiet)
            break;
        if(mpr.justFilename)
        {
            if(fqn.length() == 0)
                break;
            cout << fqn << endl;
            fqn.clear();
        } else {
            if(g_matches > 1)
                cout << endl;
            cout << me;
        }
        break; // never loop
    }
    if(!mpr.recursive)
        return; // just top level entity
    MimeEntityList::const_iterator mbit, meit;
    mbit = me.body().parts().begin(), meit = me.body().parts().end();
    for(; mbit != meit; ++mbit)
        printPart(**mbit, mpr, fqn);
}

int main(int argc, char** argv)
{
    std::ios_base::sync_with_stdio(false);
    MatchParamRq mpr;
    int ignoreMask = 0;

    int p = 1;
    while(p < argc)
    {
        string param = argv[p];
        if(param == "-h")
            usage();
        else if (param == "-r") 
            mpr.recursive = 1;
        else if (param == "-q") 
            mpr.quiet = 1;
        else if (param == "-l") 
            mpr.justFilename = 1;
        else if (param == "-H") 
            ignoreMask += imHeader;
        else if (param == "-B") 
            ignoreMask += imBody;
        else if (param == "-C") 
            ignoreMask += imChildParts;
        else if (param == "-t") {
            die( ++p == argc, param + " requires an argument");
            ContentType ct(argv[p]);
            die(mpr.type.length() != 0, "just one -t allowed");
            mpr.type = ct.type();
            mpr.subtype = ct.subtype();
        } else if (param == "-p") {
            die( ++p == argc, param + " requires an argument");
            string switch_param = argv[p];
            StringTokenizer stok(&switch_param, "=");
            pair<string,string> item;
            stok.next(item.first) && stok.next(item.second);
            mpr.paramList.push_back(item);
        } else if (param == "-f") {
            die( ++p == argc, param + " requires an argument");
            string switch_param = argv[p];
            StringTokenizer stok(&switch_param, "=");
            pair<string,string> item;
            stok.next(item.first) && stok.next(item.second);
            mpr.fieldList.push_back(item);
        } else if( param.length() == 2 && param[0] == '-') {
            usage();
        } else {
            // filename list starts here
            // first filename: argv[p]
            break;
        }
        ++p;
    }
    
    string fqn;
    if(argc == p)
    { // read from stdin
        istreambuf_iterator<char> bit(cin), eit;
        MimeEntity me;
        me.load(bit, eit, ignoreMask);
        fqn = "stdin";    
        printPart(me, mpr, fqn);
    } else
        for(int fc = p; fc < argc; ++fc)
        {
            fqn = argv[fc];
            File in(fqn);
            if(!in)
            {
                cerr << "ERR: unable to open file " << argv[fc]
                     << endl;
                continue;
            }
            MimeEntity me;
            me.load(in.begin(), in.end(), ignoreMask);
            printPart(me, mpr, fqn);
        }
    return g_matches;
}

