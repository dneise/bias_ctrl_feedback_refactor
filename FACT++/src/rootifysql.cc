#include "Database.h"

#include "Time.h"
#include "Configuration.h"

#include <TROOT.h>
#include <TSystem.h>
#include <TFile.h>
#include <TTree.h>

using namespace std;

// ------------------------------------------------------------------------

void SetupConfiguration(Configuration &conf)
{
    po::options_description control("Rootify SQL");
    control.add_options()
        ("uri,u",         var<string>()->required(),   "Database link as in\n\tuser:password@server[:port]/database.")
        ("query,q",       var<string>(""),             "MySQL query (overwrites --file)")
        ("file",          var<string>("rootify.sql"),  "An ASCII file with the MySQL query (overwrites --query)")
        ("ignore-null,i", po_switch(),                 "Do not skip rows containing any NULL field")
        ("out,o",         var<string>("rootify.root"), "Output root file name")
        ("force,f",       po_switch(),                 "Force overwriting an existing root file ('RECREATE')")
        ("update",        po_switch(),                 "Update an existing root file with the new tree ('UPDATE')")
        ("compression,c", var<uint16_t>(1),            "zlib compression level for the root file")
        ("tree,t",        var<string>("Result"),       "Name of the root tree")
        ("display,d",     po_switch(),                 "Displays contents on the screen (most usefull in combination with mysql statements as SHOW or EXPLAIN)")
        ("null,n",        po_switch(),                 "Redirect the output file to /dev/null")
        ("delimiter",     var<string>(""),             "The delimiter used if contents are displayed with --display (default=\\t)")
        ("verbose,v",     var<uint16_t>(1),            "Verbosity (0: quiet, 1: default, 2: more, 3, ...)")
        ;

    po::positional_options_description p;
    p.add("file", 1); // The 1st positional options (n=1)
    p.add("out",  1); // The 2nd positional options (n=1)

    conf.AddOptions(control);
    conf.SetArgumentPositions(p);
}

void PrintUsage()
{
    cout <<
        "rootifysql - Converts the result of a mysql query into a root file\n"
        "\n"
        "For convenience, this documentation uses the extended version of the options, "
        "refer to the output below to get the abbreviations.\n"
        "\n"
        "Writes the result of a mysql query into a root file. For each column, a branch is "
        "created of type double with the field name as name. This is usually the column name "
        "if not specified otherwise by the AS mysql directive.\n"
        "\n"
        "Columns with CHAR or VARCHAR as field type are ignored. DATETIME, DATE and TIME "
        "columns are converted to unix time (time_t). Rows containing any file which is "
        "NULL are skipped if not suppressed by the --ignore-null option. Ideally, the query "
        "is compiled in a way that no NULL field is returned. With the --display option the "
        "result of the request is printed on the screen (NULL skipping still in action). "
        "This can be useful to create an ascii file or to show results as 'SHOW DATABASES' "
        "or 'EXPLAIN table'. To redirect the contents into an ascii file, the option -v0 "
        "is useful. To suppredd writing to an output file --null can be used.\n"
        "\n"
        "The default is to read the query from a file called rootify.sql. Except if a different "
        "filename is specified by the --file option or a query is given with --query.\n"
        "\n"
        "Comments in the query-file can be placed according to the SQL standard inline "
        "/*comment*/ or on individual lines introduces with # or --.\n"
        "\n"
        "In case of succes, 0 is returned, a value>0 otherwise.\n"
        "\n"
        "Usage: rootifysql [rootify.sql [rootify.root]] [-u URI] [-q query|-f file] [-i] [-o out] [-f] [-cN] [-t tree] [-vN]\n"
        "\n"
        ;
    cout << endl;
}

int main(int argc, const char* argv[])
{
    Time start;

    gROOT->SetBatch();

    Configuration conf(argv[0]);
    conf.SetPrintUsage(PrintUsage);
    SetupConfiguration(conf);

    if (!conf.DoParse(argc, argv))
        return 127;

    // ----------------------------- Evaluate options --------------------------
    const string   uri         = conf.Get<string>("uri");
    const string   out         = conf.Get<string>("out");
    const string   file        = conf.Get<string>("file");
    const string   tree        = conf.Get<string>("tree");
    const bool     force       = conf.Get<bool>("force");
    const bool     ignore      = conf.Get<bool>("ignore-null");
    const bool     update      = conf.Get<bool>("update");
    const bool     display     = conf.Get<bool>("display");
    const bool     noout       = conf.Get<bool>("null");
    const uint16_t verbose     = conf.Get<uint16_t>("verbose");
    const uint16_t compression = conf.Get<uint16_t>("compression");
    const string   delimiter   = conf.Get<string>("delimiter");
    // -------------------------------------------------------------------------

    if (verbose>0)
        cout << "\n--------------------- Rootify SQL ----------------------" << endl;

    string query  = conf.Get<string>("query");
    if (query.empty())
    {
        if (verbose>0)
            cout << "Reading query from file '" << file << "'." << endl;

        ifstream fin(file);
        if (!fin)
        {
            cerr << "Could not open '" << file << "': " << strerror(errno) << endl;
            return 1;
        }
        getline(fin, query, (char)fin.eof());
    }

    if (query.empty())
    {
        cerr << "No query specified." << endl;
        return 2;
    }

    // -------------------------- Check for file permssion ---------------------
    // Strictly speaking, checking for write permission and existance is not necessary,
    // but it is convenient that the user does not find out that it failed after
    // waiting long for the query result
    //
    // I am using root here instead of boost to be
    // consistent with the access pattern by TFile
    TString path(noout?"/dev/null":out.c_str());
    gSystem->ExpandPathName(path);

    if (!noout)
    {
        FileStat_t stat;
        const Int_t  exist = !gSystem->GetPathInfo(path, stat);
        const Bool_t write = !gSystem->AccessPathName(path,  kWritePermission) && R_ISREG(stat.fMode);

        if ((update && !exist) || (update && exist && !write) || (force && exist && !write))
        {
            cerr << "File '" << path << "' is not writable." << endl;
            return 3;
        }

        if (!update && !force && exist)
        {
            cerr << "File '" << path << "' already exists." << endl;
            return 4;
        }
    }
    // -------------------------------------------------------------------------

    if (query.back()!='\n')
        query += '\n';

    if (verbose>2)
        cout << '\n' << query << endl;

    if (verbose>0)
        cout << "Requesting data..." << endl;

    Time start2;

    // -------------------------- Request data from database -------------------
    const mysqlpp::StoreQueryResult res =
        Database(uri).query(query).store();
    // -------------------------------------------------------------------------

    if (verbose>0)
    {
        cout << res.size() << " rows received." << endl;
        cout << "Query time: " << Time().UnixTime()-start2.UnixTime() << "s" << endl;
    }

    if (res.empty())
    {
        cerr << "Nothing to write." << endl;
        return 5;
    }

    if (verbose>0)
        cout << "Opening file '" << path << "' [compression=" << compression << "]..." << endl;

    // ----------------------------- Open output file --------------------------
    TFile tfile(path, update?"UPDATE":(force?"RECREATE":"CREATE"), "Rootify SQL", compression);
    if (tfile.IsZombie())
        return 6;
    // -------------------------------------------------------------------------

    const mysqlpp::Row &r = res.front();

    if (verbose>0)
        cout << "Trying to setup " << r.size() << " branches..." << endl;

    if (verbose>1)
        cout << endl;

    const mysqlpp::FieldNames &l = *r.field_list().list;

    vector<double>  buf(l.size());
    vector<uint8_t> typ(l.size(),'n');

    UInt_t cols = 0;

    // -------------------- Configure branches of TTree ------------------------
    TTree *ttree = new TTree(tree.c_str(), query.c_str());
    for (size_t i=0; i<l.size(); i++)
    {
        const string t = r[i].type().sql_name();

        if (t.find("DATETIME")!=string::npos)
            typ[i] = 'd';
        else
            if (t.find("DATE")!=string::npos)
                typ[i] = 'D';
            else
                if (t.find("TIME")!=string::npos)
                    typ[i] = 'T';
                else
                    if (t.find("VARCHAR")!=string::npos)
                        typ[i] = 'V';
                    else
                        if (t.find("CHAR")!=string::npos)
                            typ[i] = 'C';

        const bool use = typ[i]!='V' && typ[i]!='C';

        if (verbose>1)
            cout << (use?" + ":" - ") << l[i].c_str() << " [" << t << "] {" << typ[i] << "}\n";

        if (use)
        {
            ttree->Branch(l[i].c_str(), buf.data()+i);
            cols++;
        }
    }
    // -------------------------------------------------------------------------

    if (verbose>1)
        cout << endl;
    if (verbose>0)
        cout << "Configured " << cols << " branches.\nFilling branches..." << endl;

    if (display)
    {
        cout << endl;
        cout << "#";
        for (size_t i=0; i<l.size(); i++)
            cout << ' ' << l[i].c_str();
        cout << endl;
    }

    // ---------------------- Fill TTree with DB data --------------------------
    size_t skip = 0;
    for (auto row=res.begin(); row<res.end(); row++)
    {
        ostringstream sout;

        size_t idx=0;
        for (auto col=row->begin(); col!=row->end(); col++, idx++)
        {
            if (display)
            {
                if (idx>0)
                    sout << (delimiter.empty()?"\t":delimiter);
                sout << col->c_str();
            }

            if (!ignore && col->is_null())
            {
                skip++;
                break;
            }

            switch (typ[idx])
            {
            case 'd':
                buf[idx] = time_t((mysqlpp::DateTime)(*col));
                break;

            case 'D':
                buf[idx] = time_t((mysqlpp::Date)(*col));
                break;

            case 'T':
                buf[idx] = time_t((mysqlpp::Time)(*col));
                break;

            case 'V':
            case 'C':
                break;

            default:
                buf[idx] = atof(col->c_str());
            }
        }

        if (idx==row->size())
        {
            ttree->Fill();
            if (display)
                cout << sout.str() << endl;
        }
    }
    // -------------------------------------------------------------------------

    if (display)
        cout << '\n' << endl;

    if (verbose>0)
    {
        if (skip>0)
            cout << skip << " rows skipped due to NULL field." << endl;

        cout << ttree->GetEntries() << " rows filled into tree." << endl;
    }

    ttree->Write();
    tfile.Close();

    if (verbose>0)
    {
        cout << "File closed.\n";
        cout << "Execution time: " << Time().UnixTime()-start.UnixTime() << "s\n";
        cout << "--------------------------------------------------------" << endl;
    }

    return 0;
}
