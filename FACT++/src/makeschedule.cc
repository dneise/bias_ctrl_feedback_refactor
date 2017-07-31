#include "externals/Prediction.h"

#include <boost/algorithm/string/join.hpp>

#include "Database.h"

#include "tools.h"
#include "Time.h"
#include "Configuration.h"

using namespace std;
using namespace Nova;

// -----------------------------------------------------------------------

void SetupConfiguration(Configuration &conf)
{
    po::options_description control("Makeschedule");
    control.add_options()
        ("date", var<string>(), "SQL time (UTC), e.g. '2016-12-24' (equiv. '2016-12-24 12:00:00' to '2016-12-25 11:59:59')")
        ("source-database", var<string>()->required(), "Database link as in\n\tuser:password@server[:port]/database.")
        ("schedule-database", var<string>(), "Database link as in\n\tuser:password@server[:port]/database.")
        ("max-current", var<double>(90), "Global maximum current limit in uA")
        ("max-zd", var<double>(75), "Global zenith distance limit in degree")
        ("source", vars<string>(), "List of all TeV sources to be included, names according to the database")
        ("setup.*", var<double>(), "Setup for the sources to be observed")
        ("preobs.*", vars<string>(), "Prescheduled observations")
        ("startup.offset", var<double>(15), "Determines how many minutes the startup is scheduled before data-taking.start [0;120]")
        ("data-taking.start", var<double>(-12), "Begin of data-taking in degree of sun below horizon")
        ("data-taking.end", var<double>(-13.75), "End of data-taking in degree of sun below horizon")
        ("enter-schedule-into-database", var<bool>(), "Enter schedule into database (required schedule-database, false: dry-run)")
        ;

    po::positional_options_description p;
    p.add("date", 1); // The first positional options

    conf.AddOptions(control);
    conf.SetArgumentPositions(p);
}

void PrintUsage()
{
    cout <<
        "makeschedule - Creates an automatic schedule\n"
        "\n"
        "Usage: makeschedule [yyyy-mm-dd]\n";
    cout << endl;
}

void PrintHelp()
{
#ifdef HAVE_ROOT
    cout <<
        "First for each minute of the night a list is calculated of all "
        "selected sources fulfiling all global and all source specific "
        "constraints, e.g. on the zenith distance or the current.\n"
        "\n"
        "The remaining source list is sorted by the relative threshold, "
        "while the threshold is weighted with a user defined source "
        "specific penalty. The first source of the list is taken to "
        "be observed.\n"
        "\n"
        "In a next step the first and last source of the resulting schedule "
        "are evaluated. If their observation time is below 40', it is tried "
        "to extend it to 40min. If this violates one of the criteria mentioned "
        "above or gives an observation time for the neighbouring source of "
        "less than 40min, try to replace it by the neighbouring source. "
        "If this also does not fulfil the requirements, the original "
        "schedule remains unchanged.\n"
        "\n"
        "Now a similar check is run for all intermediate sources. They are "
        "checked (from the beginning to the end, one by one), if they have "
        "an observation time of less than 40min. In this case, it is tried "
        "to remove them. The observation of the two neighbouring sources is "
        "extended to their penalized point of equal relative threshold. "
        "If this solution would not fulfil all criteria, no change is made.\n"
        "\n"
        "In a last step, all remaining sources with less than 5min "
        "observation time are replaced with sleep and sleep after startup "
        "or before shutdown are removed.\n"
        "\n"
        "\n"
        "Examples:\n"
        "\n"
        "  makeschedule 2016-12-24\n"
        "\n"
        "Calculate the Christmas schedule for 2016 using all TeV sources from the\n"
        "database. If the date is omitted the current date is used.\n"
        "\n"
        "  makeschedule --source='Mrk 421' --source='Mrk 501' --source='Crab'\n"
        "\n"
        "Use only the mentioned sources to calculate the schedule.\n"
        "\n"
        "  makeschedule --source=Crab --setup.Crab.max-zd=30\n"
        "\n"
        "Limit the zenith distance of Crab into the range [0;30]deg.\n"
        "\n"
        "  makeschedule --source=Crab --setup.Crab.max-current=50\n"
        "\n"
        "Limit the maximum estimated current of Crab at 50uA.\n"
        "\n"
        "  makeschedule --source='IC 310' '--setup.IC 310.penalty=1.2'\n"
        "\n"
        "Multiply IC310's estimated relative threshold by a factor 1.2\n"
        "\n";
    cout << endl;
#endif
}


struct MyDouble
{
    double val;
    bool valid;
    MyDouble(Configuration &conf, const string &str) : val(0)
    {
        valid = conf.Has(str);
        if (valid)
            val = conf.Get<double>(str);
    }
    MyDouble() : val(0), valid(false) {}
};

/*
struct MinMax
{
    MyDouble min;
    MyDouble max;
    MinMax(Configuration &conf, const string &str)
    {
        min = MyDouble(conf, str+".min");
        max = MyDouble(conf, str+".max");
    }
    MinMax() {}
};
*/

struct Source
{
    // Global limits
    static double max_current;
    static double max_zd;

    // Source descrition
    string name;
    uint16_t key;
    EquPosn equ;

    // Source specific limits
    MyDouble maxzd;
    MyDouble maxcurrent;
    double penalty;

    // Possible observation time
    double begin;
    double end;

    // Threshold (sorting reference)
    double threshold;

    double duration() const { return end-begin; };

    // Pre-observations (e.g. ratescan)
    vector<string> preobs;

    Source(const string &n="", uint16_t k=-1) : name(n), key(k), begin(0), threshold(std::numeric_limits<double>::max()) { }

    //bool IsSpecial() const { return threshold==std::numeric_limits<double>::max(); }

    double zd(const double &jd) const
    {
        return 90-GetHrzFromEqu(equ, jd).alt;
    }

    bool valid(const SolarObjects &so) const
    {
        const HrzPosn hrz     = GetHrzFromEqu(equ, so.fJD);
        const double  current = FACT::PredictI(so, equ);

        if (current>max_current)
            return false;

        if (hrz.alt<=0 || 90-hrz.alt>max_zd)
            return false;

        if (maxzd.valid && 90-hrz.alt>maxzd.val)
            return false;

        if (maxcurrent.valid && current>maxcurrent.val)
            return false;

        return true;
    }

    bool IsRangeValid(const double &jd_begin, const double &jd_end) const
    {
        const uint32_t n = nearbyint((jd_end-jd_begin)*24*60);
        for (uint32_t i=0; i<n; i++)
            if (!valid(SolarObjects(jd_begin+i/24./60.)))
                return false;

        return true;
    }

    double getThreshold(const SolarObjects &so) const
    {
        const HrzPosn hrz = GetHrzFromEqu(equ, so.fJD);
        const double  current = FACT::PredictI(so, equ);

        const double ratio = pow(cos((90-hrz.alt)*M_PI/180), -2.664);

        return penalty*ratio*pow(current/6.2, 0.394);
    }

    bool calcThreshold(const SolarObjects &so)
    {
        const HrzPosn hrz     = GetHrzFromEqu(equ, so.fJD);
        const double  current = FACT::PredictI(so, equ);

        if (current>max_current)
            return false;

        if (hrz.alt<=0 || 90-hrz.alt>max_zd)
            return false;

        if (maxzd.valid && 90-hrz.alt>maxzd.val)
            return false;

        if (maxcurrent.valid && current>maxcurrent.val)
            return false;

        const double ratio = pow(cos((90-hrz.alt)*M_PI/180), -2.664);
        threshold = penalty*ratio*pow(current/6.2, 0.394);

        return true;
    }
};

double Source::max_zd;
double Source::max_current;

bool SortByThreshold(const Source &i, const Source &j) { return i.threshold<j.threshold; }

bool RescheduleFirstSources(vector<Source> &obs)
{
    if (obs.size()<2 || obs[0].duration()>=40./24/60 || obs[0].name=="SLEEP" || obs[1].name=="SLEEP")
        return false;

    cout << "First source [" << obs[0].name << "] detected < 40min" << endl;

    const double obs1_duration = obs[1].end - obs[0].begin - 40./24/60;
    const double obs0_end      =              obs[0].begin + 40./24/60;

    // Check that:
    //  - the duration for the shrunken source obs[1] is still above 40min
    //  - obs[0] does not exceed 60deg at the end of its new window
    //  - obs[0] does not exceed any limit within the new window

    if (obs1_duration>=40./24/60 && obs[0].IsRangeValid(obs[0].end, obs0_end))
    {
        obs[0].end   = obs0_end;
        obs[1].begin = obs0_end;

        cout << "First source [" << obs[0].name << "] extended to 40min" << endl;

        return false;
    }

    // Try to remove the first source, check if second source fullfills all limits
    if (obs[1].IsRangeValid(obs[0].begin, obs[0].end))
    {
        cout << "First source [" << obs[0].name << "] removed" << endl;

        obs[1].begin = obs[0].begin;
        obs.erase(obs.begin());

        return true;
    }

    // Try to remove the second source, check if the first source fullfills all limits
    if (obs[0].IsRangeValid(obs[1].begin, obs[1].end))
    {
        cout << "Second source [" << obs[1].name << "] removed" << endl;

        obs[0].end = obs[1].end;
        obs.erase(obs.begin()+1);

        if (obs.size()==0 || obs[0].name!=obs[1].name)
            return true;

        obs[0].end = obs[1].end;
        obs.erase(obs.begin()+1);

        cout << "Combined first two indentical sources [" << obs[0].name << "] into one observation" << endl;

        return true;
    }

    cout << "No reschedule possible within limit." << endl;

    return false;
}

bool RescheduleLastSources(vector<Source> &obs)
{
    // If observation time is smaller than 40min for the first source
    // extend it to 40min if zenith angle will not go above 60deg.
    const int last = obs.size()-1;
    if (obs.size()<2 || obs[last].duration()>=40./24/60 || obs[last].name=="SLEEP" || obs[last-1].name=="SLEEP")
        return false;

    cout << "Last source [" << obs[last].name << "] detected < 40min" << endl;

    const double obs1_duration = obs[last].end - 40./24/60 - obs[last-1].begin;
    const double obs0_begin    = obs[last].end - 40./24/60;

    // Check that:
    //  - the duration for the shrunken source obs[1] is still above 40min
    //  - obs[0] does not exceed 60deg at the end of its new window
    //  - obs[0] does not exceed any limit within the new window

    if (obs1_duration>=40./24/60 && obs[last].IsRangeValid(obs0_begin, obs[last].begin))
    {
        obs[last].begin = obs0_begin;
        obs[last-1].end = obs0_begin;

        cout << "Last source [" << obs[last].name << "] extended to 40min" << endl;

        return false;
    }

    // Try to remove the last source, check if second source fullfills all limits
    if (obs[last-1].IsRangeValid(obs[last].begin, obs[last].end))
    {
        cout << "Last source [" << obs[last].name << "] removed" << endl;

        obs[last-1].end = obs[last].end;
        obs.pop_back();

        return true;
    }

    // Try to remove the second last source, check if the first source fullfills all limits
    if (obs[last].IsRangeValid(obs[last-1].begin, obs[last-1].end))
    {
        cout << "Second last source [" << obs[last-1].name << "] removed" << endl;

        obs[last].begin = obs[last-1].begin;
        obs.erase(obs.begin()+obs.size()-2);

        if (obs.size()==0 || obs[last-1].name!=obs[last-2].name)
            return true;

        obs[last-2].end = obs[last-1].end;
        obs.pop_back();

        cout << "Combined last two indentical sources [" << obs[last-1].name << "] into one observation" << endl;

        return true;
    }

    cout << "No reschedule possible within limit." << endl;

    return false;
}

bool RescheduleIntermediateSources(vector<Source> &obs)
{
    for (size_t i=1; i<obs.size()-1; i++)
    {
        if (obs[i].duration()>=40./24/60)
            continue;

        if (obs[i-1].name=="SLEEP" && obs[i+1].name=="SLEEP")
            continue;

        cout << "Intermediate source [" << obs[i].name << "] detected < 40min" << endl;

        double intersection = -1;

        if (obs[i-1].name=="SLEEP")
            intersection = obs[i].begin;

        if (obs[i+1].name=="SLEEP")
            intersection = obs[i].end;

        if (obs[i-1].name==obs[i+1].name)
            intersection = obs[i].begin;

        if (intersection<0)
        {
            const uint32_t n = nearbyint((obs[i].end-obs[i].begin)*24*60);
            for (uint32_t ii=0; ii<n; ii++)
            {
                const double jd = obs[i].begin+ii/24./60.;

                const SolarObjects so(jd);
                if (obs[i-1].getThreshold(so)>=obs[i+1].getThreshold(so))
                {
                    intersection = jd;
                    break;
                }
            }
        }

        if ((obs[i-1].name!="SLEEP" && !obs[i-1].IsRangeValid(obs[i-1].end, intersection)) ||
            (obs[i+1].name!="SLEEP" && !obs[i+1].IsRangeValid(intersection, obs[i+1].begin)))
        {
            cout << "No reschedule possible within limits." << endl;
            continue;
        }

        cout << "Intermediate source [" << obs[i].name << "] removed" << endl;

        const bool underflow = obs[i-1].duration()*24*60<40 || obs[i+1].duration()*24*60<40;

        obs[i-1].end   = intersection;
        obs[i+1].begin = intersection;
        obs.erase(obs.begin()+i);

        i--;

        if (obs.size()>1 && obs[i].name==obs[i+1].name)
        {
            obs[i].end = obs[i+1].end;
            obs.erase(obs.begin()+i+1);

            cout << "Combined two surrounding indentical sources [" << obs[i].name << "] into one observation" << endl;

            i--;

            continue;
        }

        if (underflow)
            cout << "WARNING - Neighbor source < 40min as well." << endl;
    }
    return false;
}

void RemoveMiniSources(vector<Source> &obs)
{
    for (size_t i=1; i<obs.size()-1; i++)
    {
        if (obs[i].duration()>=5./24/60)
            continue;

        if (obs[i-1].name=="SLEEP" && obs[i+1].name=="SLEEP")
            continue;

        cout << "Mini source [" << obs[i].name << "] detected < 5min" << endl;

        if (obs[i-1].name=="SLEEP" && obs[i+1].name=="SLEEP")
        {
            obs[i-1].end = obs[i+2].begin;

            obs.erase(obs.begin()+i+1);
            obs.erase(obs.begin()+i);

            i -= 2;

            cout << "Combined two surrounding sleep into one" << endl;

            continue;
        }

        if (obs[i-1].name=="SLEEP")
        {
            obs[i-1].end = obs[i+1].begin;
            obs.erase(obs.begin()+i);
            i--;

            cout << "Extended previous sleep" << endl;

            continue;
        }

        if (obs[i+1].name=="SLEEP")
        {
            obs[i+1].begin = obs[i-1].end;
            obs.erase(obs.begin()+i);

            cout << "Extended following sleep" << endl;

            i--;
            continue;
        }
    }
}

void CheckStartupAndShutdown(vector<Source> &obs)
{
    if (obs.front().name=="SLEEP")
    {
        obs.erase(obs.begin());
        cout << "Detected sleep after startup... removed." << endl;
    }

    if (obs.back().name=="SLEEP")
    {
        obs.pop_back();
        cout << "Detected sleep before shutdown... removed." << endl;
    }
}

void Print(const vector<Source> &obs, double startup_offset)
{
    cout << Time(obs[0].begin-startup_offset).GetAsStr() << "  STARTUP\n";
    for (const auto& src: obs)
    {
        string tm = Time(src.begin).GetAsStr();
        if (src.preobs.size()>0)
        {
            for (const auto& pre: src.preobs)
            {
                cout << tm << "  " << pre << "\n";
                tm = "                   ";
            }
        }

        cout << tm << "  " << src.name << " [";
        cout << src.duration()*24*60 << "'";
        if (src.name!="SLEEP")
            cout << Tools::Form("; %.1f/%.1f", src.zd(src.begin), src.zd(src.end));
        cout << "]";

        if (src.duration()*24*60<40)
            cout << " (!)";

        cout << "\n";
    }
    cout << Time(obs.back().end).GetAsStr() << "  SHUTDOWN" << endl;
}

int FillSql(Database &db, int enter, const vector<Source> &obs, double startup_offset)
{
    const string query0 = "SELECT COUNT(*) FROM Schedule WHERE DATE(ADDTIME(fStart, '-12:00')) = '"+Time(obs[0].begin).GetAsStr("%Y-%m-%d")+"'";

    const mysqlpp::StoreQueryResult res0 = db.query(query0).store();

    if (res0.num_rows()!=1)
    {
        cout << "Check for schedule size failed." << endl;
        return 10;
    }

    if (uint32_t(res0[0][0])!=0)
    {
        cout << "Schedule not empty." << endl;
        return 11;
    }

    const mysqlpp::StoreQueryResult res1 = db.query("SELECT fMeasurementTypeName, fMeasurementTypeKEY FROM MeasurementType").store();
    map<string, uint32_t> types;
    for (const auto &row: res1)
        types.insert(make_pair(string(row[0]), uint32_t(row[1])));

    ostringstream str;
    str << "INSERT INTO Schedule (fStart, fUser, fMeasurementID, fMeasurementTypeKEY, fSourceKEY) VALUES ";

    str << "('" << Time(obs[0].begin-startup_offset).GetAsStr() << "', 'auto', 0, " << types["Startup"] << ", NULL),\n"; // [Startup]\n";
    for (const auto& src: obs)
    {
        string tm = Time(src.begin).GetAsStr();

        /*
         if (src.preobs.size()>0)
         {
         for (const auto& pre: src.preobs)
         {
         str << tm << "  " << pre << "\n";
         tm = "                   ";
         }
         }*/

        if (src.name!="SLEEP")
            str << "('" << tm << "', 'auto', 0, " << types["Data"] << ", " << src.key << "),\n"; // [Data: " << src.name << "]\n";
        else
            str << "('" << tm << "', 'auto', 0, " << types["Sleep"] << ", NULL),\n"; // [Sleep]\n";
    }

    str << "('" << Time(obs.back().end).GetAsStr() << "', 'auto', 0, " << types["Shutdown"] << ", NULL)";// [Shutdown]";

    if (enter<0)
    {
        cout << str.str() << endl;
        return 0;
    }

    db.query(str.str()).exec();

    cout << "Schedule entered successfully into database." << endl;
    return 0;
}

int main(int argc, const char* argv[])
{
//    gROOT->SetBatch();

    Configuration conf(argv[0]);
    conf.SetPrintUsage(PrintUsage);
    SetupConfiguration(conf);

    if (!conf.DoParse(argc, argv, PrintHelp))
        return 127;

    // ------------------ Eval config ---------------------

    const int enter = conf.Has("enter-schedule-into-database") ? (conf.Get<bool>("enter-schedule-into-database") ? 1 : -1) : 0;
    if (enter && !conf.Has("schedule-database"))
        throw runtime_error("enter-schedule-into-database required schedule-database.");

    Time time;
    if (conf.Has("date"))
        time.SetFromStr(conf.Get<string>("date")+" 12:00:00");

    if (enter && floor(time.JD())<ceil(Time().JD()))
        throw runtime_error("Only future schedules can be entered into the database.");

    Source::max_current = conf.Get<double>("max-current");
    Source::max_zd      = conf.Get<double>("max-zd");

    const double startup_offset = conf.Get<double>("startup.offset")/60/24;

    const double angle_sun_set  = conf.Get<double>("data-taking.start");
    const double angle_sun_rise = conf.Get<double>("data-taking.end");

    if (startup_offset<0 || startup_offset>120)
        throw runtime_error("Only values [0;120] are allowed for startup.offset");

    if (angle_sun_set>-6)
        throw runtime_error("datataking.start not allowed before sun at -6deg");

    if (angle_sun_rise>-6)
        throw runtime_error("datataking.end not allowed after sun at -6deg");

    // -12: nautical
    // Sun set with the same date than th provided date
    // Sun rise on the following day
    const RstTime sun_set  = GetSolarRst(floor(time.JD())-0.5, angle_sun_set);
    const RstTime sun_rise = GetSolarRst(floor(time.JD())+0.5, angle_sun_rise);

    const double sunset  = ceil(sun_set.set*24*60)   /24/60 + 1e-9;
    const double sunrise = floor(sun_rise.rise*24*60)/24/60 + 1e-9;

    cout << "\n";

    cout << "Date: " << Time(floor(sunset)).GetAsStr() << "\n";
    cout << "Set:  " << Time(sunset).GetAsStr()  << "  [" << Time(sun_set.set)   << "]\n";
    cout << "Rise: " << Time(sunrise).GetAsStr() << "  [" << Time(sun_rise.rise) << "]\n";

    cout << "\n";

    cout << "Global maximum current:  " << Source::max_current << " uA/pix\n";
    cout << "Global zenith distance:  " << Source::max_zd << " deg\n";

    cout << "\n";

    // ------------- Get Sources from databasse ---------------------

    const vector<string> ourcenames = conf.Vec<string>("source");
    const vector<string> sourcenames = conf.Vec<string>("source");
    cout << "Nsources = " << sourcenames.size() << "\n";

    string query = "SELECT fSourceName, fSourceKEY, fRightAscension, fDeclination FROM Source WHERE fSourceTypeKEY=1";
    if (sourcenames.size()>0)
        query += " AND fSourceName IN ('" + boost::algorithm::join(sourcenames, "', '")+"')";

    const string sourcedb = conf.Get<string>("source-database");
    const mysqlpp::StoreQueryResult res =
        Database(sourcedb).query(query).store();

    // ------------------ Eval config ---------------------

    vector<Source> sources;
    for (const auto &row: res)
    {
        const string name = string(row[0]);

        Source src(name, row[1]);

        src.equ.ra  = double(row[2])*15;
        src.equ.dec = double(row[3]);

        src.maxzd = MyDouble(conf, "setup."+name+".max-zd");
        src.maxcurrent = MyDouble(conf, "setup."+name+".max-current");
        src.penalty = conf.Has("setup."+name+".penalty") ?
            conf.Get<double>("setup."+name+".penalty") : 1;

        src.preobs = conf.Vec<string>("preobs."+name);


        cout << "[" << name << "]";

        if (src.maxzd.valid)
            cout << " Zd<" << src.maxzd.val;
        if (src.penalty!=1)
            cout << " Penalty=" << src.penalty;

        cout << " " << boost::algorithm::join(src.preobs, "+") << endl;

        /*
         RstTime t1 = GetObjectRst(floor(sunset)-1, src.equ);
         RstTime t2 = GetObjectRst(floor(sunset),   src.equ);

         src.rst.transit = t1.transit<floor(sunset) ? t2.transit : t1.transit;
         src.rst.rise = t1.rise>src.rst.transit ? t2.rise : t1.rise;
         src.rst.set  = t1.set <src.rst.transit ? t2.set  : t1.set;
         */

        sources.emplace_back(src);
    }
    cout << endl;

    // -------------------------------------------------------------------------

    vector<Source> obs;

    const uint32_t n = nearbyint((sunrise-sunset)*24*60);
    for (uint32_t i=0; i<n; i++)
    {
        const double jd = sunset + i/24./60.;

        const SolarObjects so(jd);

        vector<Source> vis;
        for (auto& src: sources)
        {
            if (src.calcThreshold(so))
                vis.emplace_back(src);
        }

        // In case no source was found, add a sleep source
        Source src("SLEEP");
        vis.emplace_back(src);

        // Source has higher priority if minimum observation time not yet fullfilled
        sort(vis.begin(), vis.end(), SortByThreshold);

        if (obs.size()>0 && obs.back().name==vis[0].name)
            continue;

        vis[0].begin = jd;
        obs.emplace_back(vis[0]);
    }

    if (obs.size()==0)
    {
        cout << "No source found." << endl;
        return 1;
    }

    // -------------------------------------------------------------------------

    for (auto it=obs.begin(); it<obs.end()-1; it++)
        it[0].end = it[1].begin;
    obs.back().end = sunrise;

    // -------------------------------------------------------------------------

    Print(obs, startup_offset);
    cout << endl;

    // -------------------------------------------------------------------------

    while (RescheduleFirstSources(obs));
    while (RescheduleLastSources(obs));
    while (RescheduleIntermediateSources(obs));

    RemoveMiniSources(obs);
    CheckStartupAndShutdown(obs);

    // ---------------------------------------------------------------------

    cout << endl;
    Print(obs, startup_offset);
    cout << endl;

    // ---------------------------------------------------------------------

    if (!enter)
        return 0;

    const string scheduledb = conf.Get<string>("schedule-database");

    Database db(scheduledb);

    if (enter>0)
        db.query("LOCK TABLES Schedule WRITE");

    const int rc = FillSql(db, enter, obs, startup_offset);

    if (enter>0)
        db.query("UNLOCK TABLES");

    // ---------------------------------------------------------------------

    return rc;
}
