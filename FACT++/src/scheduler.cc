#include <vector>

#include <boost/regex.hpp>

#include <mysql++/mysql++.h>

#include "Dim.h"
#include "Time.h"
#include "Event.h"
#include "Connection.h"
#include "LocalControl.h"
#include "Configuration.h"
#include "StateMachineDim.h"

#include "tools.h"

using namespace std;
using namespace boost::gregorian;
using namespace boost::posix_time;

// things to be done/checked/changed
// * --schedule-database should be required
// * move definition of config parameters to AutoScheduler class
//   + read in from config
// * in some (all?) loops iterator over vector can be replaced by counter

// other things to do
//
// define what to transmit as info/warn/error


// config parameters:
//   mintime
//   runtimec
//   runtimep
//   repostime

// missing:
//
// calculate time for std obs
// calculate sun set/rise
//
// return errors and other otherput from sendcommand to webinterface

// in which cases should the scheduler go in error state?
//   when db is unavailable
// does one also need a 'set scheduler to ready' function then?
// do we want any error state at all?


// =========================================================================

template <class T>
class AutoScheduler : public T
{
    bool fNextIsPreview;
public:
    enum states_t
    {
        kSM_Scheduling=1,
        kSM_Comitting,
    };

    struct ObservationParameters
    {
        int obskey;
        int obsmode;
        int obstype;
        int splitflag;
        int telsetup;
        float fluxweight;
        float slope;
        float flux;
        float ra;
        float dec;
        ptime start;
        ptime stop;
        time_duration duration_db;
        string sourcename;
        int sourcekey;
    };

    struct FixedObs
    {
        int obskey;
        int sourcekey;
        string sourcename;
        int obsmode;
        int obstype;
        int telsetup;
        float ra;
        float dec;
        ptime start;
        ptime stop;
    };

    // will need other types of obs
    // FloatingObs (duration < stop-start + splitflag no)
    // FloatingSplittedObs (duration < stop-start + splitflag yes)
    // FixedSlot, i.e. just block a time slot

    struct StdObs
    {
        int obskey_std;
        int sourcekey_std;
        string sourcename_std;
        int obsmode_std;
        int obstype_std;
        int telsetup_std;
        float fluxweight_std;
        float slope_std;
        float flux_std;
        float ra_std;
        float dec_std;
        ptime obsstdstart;
        ptime obsstdstop;
    };

    struct ScheduledObs
    {
        int obskey_obs;
        int sourcekey_obs;
        string sourcename_obs;
        int obsmode_obs;
        int obstype_obs;
        int telsetup_obs;
        ptime obsstart;
        ptime obsstop;
    };

    struct ScheduledRun
    {
        //int runnumber; // to be seen, if runnumber is needed
        int obskey_run;
        int runtype;
        int sourcekey_run;
        string sourcename_run;//for convenience
        int obsmode_run;
        int obstype_run;
        int telsetup_run;
        ptime runstart;
        ptime runstop;
    };

    string fDatabase;
    string fDBName;
    int fDurationCalRun; //unit: minutes
    int fDurationPedRun; //unit: minutes
    int fDurationRepos; //unit: minutes

    int Schedule()
    {
        bool error = false;

        time_duration runtimec(0, fDurationCalRun, 0);
        time_duration runtimep(0, fDurationPedRun, 0);
        time_duration repostime(0, fDurationRepos, 0);
        time_duration mintime(1, 0, 0);

        const ptime startsched(microsec_clock::local_time());
        const ptime stopsched=startsched+years(1);

        ostringstream str;
        str << "Scheduling for the period from " << startsched << " to " << stopsched;
        T::Message(str);

        static const boost::regex expr("([[:word:].-]+):(.+)@([[:word:].-]+)(:([[:digit:]]+))?/([[:word:].-]+)");
        // 2: user
        // 4: pass
        // 5: server
        // 7: port
        // 9: db

        boost::smatch what;
        if (!boost::regex_match(fDatabase, what, expr, boost::match_extra))
        {
            ostringstream msg;
            msg << "Regex to parse database '" << fDatabase << "' empty.";
            T::Error(msg);
            return T::kSM_Error;
        }

        if (what.size()!=7)
        {
            ostringstream msg;
            msg << "Parsing database name failed: '" << fDatabase << "'";
            T::Error(msg);
            return T::kSM_Error;
        }

        const string user   = what[1];
        const string passwd = what[2];
        const string server = what[3];
        const string db     = fDBName.empty() ? what[6] : fDBName;
        const int    port   = stoi(what[5]);

        ostringstream dbnamemsg;
        dbnamemsg << "Scheduling started -> using database " << db << ".";
        T::Message(dbnamemsg);

        str.str("");
        str << "Connecting to '";
        if (!user.empty())
            str << user << "@";
        str << server;
        if (port)
            str << ":" << port;
        if (!db.empty())
            str << "/" << db;
        str << "'";
        T::Info(str);

        mysqlpp::Connection conn(db.c_str(), server.c_str(), user.c_str(), passwd.c_str(), port);
        /* throws exceptions
        if (!conn.connected())
        {
            ostringstream msg;
            msg << "MySQL connection error: " << conn.error();
            T::Error(msg);
            return T::kSM_Error;
        }*/

        // get observation parameters from DB
        // maybe order by priority?
        const mysqlpp::StoreQueryResult res =
            conn.query("SELECT fObservationKEY, fStartTime, fStopTime, fDuration, fSourceName, fSourceKEY, fSplitFlag, fFluxWeight, fSlope, fFlux, fRightAscension, fDeclination, fObservationModeKEY, fObservationTypeKEY , fTelescopeSetupKEY FROM ObservationParameters LEFT JOIN Source USING(fSourceKEY) ORDER BY fStartTime").store();
        // FIXME: Maybe we have to check for a successfull
        //        query but an empty result
        /* thorws exceptions?
        if (!res)
        {
            ostringstream msg;
            msg << "MySQL query failed: " << query.error();
            T::Error(msg);
            return T::kSM_Error;
        }*/

        str.str("");
        str << "Found " << res.num_rows() << " Observation Parameter sets.";
        T::Debug(str);

        ObservationParameters olist[res.num_rows()];
        vector<FixedObs>     obsfixedlist;
        vector<StdObs>       obsstdlist;
        vector<ScheduledObs> obslist;
        vector<ScheduledRun> runlist;

        // loop over observation parameters from DB
        // fill these parameters into FixedObs and StdObs
        int counter=0;
        int counter2=0;
        int counter3=0;
        cout << "Obs: <obskey> <sourcename>(<sourcekey>, <fluxweight>) from <starttime> to <stoptime>" << endl;
        for (vector<mysqlpp::Row>::const_iterator v=res.begin(); v<res.end(); v++)
        {
            cout << "  Obs: " << (*v)[0].c_str() << " " << (*v)[4].c_str() << "(" << (*v)[5].c_str() << flush;
            cout << ", " << (*v)[7].c_str() << ")" << flush;
            cout << " from " << (*v)[1].c_str() << " to " << (*v)[2].c_str() << endl;

            //0: obskey
            //1: startime
            //2: stoptime
            //3: duration
            //4: sourcename
            //5: sourcekey
            //6: splitflag
            //7: fluxweight
            //8: slope
            //9: flux
            //10: ra
            //11: dec
            //12: obsmode
            //13: obstype
            //14: telsetup
            stringstream t1;
            stringstream t2;
            stringstream t3;
            t1 << (*v)[1].c_str();
            t2 << (*v)[2].c_str();
            t3 << (*v)[3].c_str();

            //boost::posix_time::time_duration mintime(0,conf.Get<int>("mintime"), 0);
            t1 >> Time::sql >> olist[counter].start;
            t2 >> Time::sql >> olist[counter].stop;
            t3 >> olist[counter].duration_db;
            const time_period period(olist[counter].start, olist[counter].stop);

            olist[counter].sourcename=(*v)[4].c_str();
            olist[counter].sourcekey=(*v)[5];

            if (!(*v)[0].is_null())
                olist[counter].obskey=(*v)[0];
            if (!(*v)[12].is_null())
                olist[counter].obsmode=(*v)[12];
            if (!(*v)[13].is_null())
                olist[counter].obstype=(*v)[13];
            if (!(*v)[14].is_null())
                olist[counter].telsetup=(*v)[14];
            if (!(*v)[6].is_null())
                olist[counter].splitflag=(*v)[6];
            if (!(*v)[7].is_null())
                olist[counter].fluxweight=(*v)[7];
            else
                olist[counter].fluxweight=0;//set fluxweight to 0 for check below
            if (!(*v)[8].is_null())
                olist[counter].slope=(*v)[8];
            if (!(*v)[9].is_null())
                olist[counter].flux=(*v)[9];
            if (!(*v)[10].is_null())
                olist[counter].ra=(*v)[10];
            if (!(*v)[11].is_null())
                olist[counter].dec=(*v)[11];

            // time_duration cannot be used, as only up to 99 hours are handeled
            // const time_duration duration = period.length();

            /*
            if (olist[counter].stoptime < olist[counter].starttime+mintime)
                cout << "  ====> WARN: Observation too short. " << endl;

            if (olist[counter].starttime.is_not_a_date_time())
                cout << "  WARN: starttime not a date_time. " << endl;
            else
                cout << "  start:   " << Time::sql << olist[counter].starttime << endl;
            if (olist[counter].stoptime.is_not_a_date_time())
                cout << "  WARN: stoptime not a date_time. " << endl;
            else
                cout << "  stop:   " << Time::sql << olist[counter].stoptime << endl;
            if (!(olist[counter].starttime.is_not_a_date_time() || olist[counter].stoptime.is_not_a_date_time()))
                cout << "  diff:   " << period << endl;
            if (olist[counter].stoptime < olist[counter].starttime)
                cout << "  ====> WARN: stop time (" << olist[counter].stoptime << ") < start time (" << olist[counter].starttime << "). " << endl;
            cout << "diff:   " << duration << flush;
            cout << "dur_db:   " << olist[counter].duration_db << endl;
            */

            // always filled: obstype
            //
            // fixed observations:
            //   filled: starttime, stoptime
            //   not filled: fluxweight
            //   maybe filled: obsmode, telsetup, source (not filled for FixedSlotObs)
            //   maybe filled: duration (filled for FloatingObs and FloatingSplittedObs)
            //   maybe filled: splitflag (filled for FloatingSplittedObs)
            //
            // std observations:
            //   filled: fluxweight, telsetup, obsmore, source
            //   not filled: starttime, stoptime, duration

            // fixed observations
            if (!(olist[counter].stop.is_not_a_date_time()
                  && olist[counter].start.is_not_a_date_time())
                && olist[counter].fluxweight==0
               )
            {
                obsfixedlist.resize(counter2+1);
                obsfixedlist[counter2].start=olist[counter].start;
                obsfixedlist[counter2].stop=olist[counter].stop;
                obsfixedlist[counter2].sourcename=olist[counter].sourcename;
                obsfixedlist[counter2].obskey=olist[counter].obskey;
                obsfixedlist[counter2].obstype=olist[counter].obstype;
                obsfixedlist[counter2].obsmode=olist[counter].obsmode;
                obsfixedlist[counter2].telsetup=olist[counter].telsetup;
                obsfixedlist[counter2].sourcekey=olist[counter].sourcekey;
                obsfixedlist[counter2].ra=olist[counter].ra;
                obsfixedlist[counter2].dec=olist[counter].dec;
                counter2++;
            }

            // std obs
            if (olist[counter].stop.is_not_a_date_time()
                  && olist[counter].start.is_not_a_date_time()
                && olist[counter].fluxweight>0
               )
            {
                obsstdlist.resize(counter3+1);
                obsstdlist[counter3].sourcename_std=olist[counter].sourcename;
                obsstdlist[counter3].obskey_std=olist[counter].obskey;
                obsstdlist[counter3].obsmode_std=olist[counter].obsmode;
                obsstdlist[counter3].obstype_std=olist[counter].obstype;
                obsstdlist[counter3].telsetup_std=olist[counter].telsetup;
                obsstdlist[counter3].sourcekey_std=olist[counter].sourcekey;
                obsstdlist[counter3].fluxweight_std=olist[counter].fluxweight;
                obsstdlist[counter3].flux_std=olist[counter].flux;
                obsstdlist[counter3].slope_std=olist[counter].slope;
                obsstdlist[counter3].ra_std=olist[counter].ra;
                obsstdlist[counter3].dec_std=olist[counter].dec;
                counter3++;
            }

            counter++;
        }
        ostringstream fixedobsmsg;
        fixedobsmsg << obsfixedlist.size() << " fixed observations found. ";
        T::Message(fixedobsmsg);
        cout << obsfixedlist.size() << " fixed observations found. " << endl;

        ostringstream stdobsmsg;
        stdobsmsg << obsstdlist.size() << " standard observations found. ";
        T::Message(stdobsmsg);
        cout << obsstdlist.size() << " standard observations found. " << endl;

        // loop to add the fixed observations to the ScheduledObs list
        // performed checks:
        //   * overlap of fixed observations: the overlap is split half-half
        //   * check for scheduling time range: only take into account fixed obs within the range
        // missing checks and evaluation
        //   * check for mintime (pb with overlap checks)
        //   * check for sun
        //   * check for moon
        counter2=0;
        int skipcounter=0;
        ptime finalobsfixedstart;
        ptime finalobsfixedstop;
        time_duration delta0(0,0,0);

        cout << "Fixed Observations: " << endl;
        for (struct vector<FixedObs>::const_iterator vobs=obsfixedlist.begin(); vobs!=obsfixedlist.end(); vobs++)
        {
            if (obsfixedlist[counter2].start < startsched
                || obsfixedlist[counter2].stop > stopsched)
            {
                ostringstream skipfixedobsmsg;
                skipfixedobsmsg << "Skip 1 fixed observation (obskey ";
                skipfixedobsmsg << obsfixedlist[counter2].obskey;
                skipfixedobsmsg << ") as it is out of scheduling time range.";
                T::Message(skipfixedobsmsg);

                counter2++;
                skipcounter++;
                continue;
            }
            counter3=0;

            time_duration delta1=delta0;
            time_duration delta2=delta0;

            finalobsfixedstart=obsfixedlist[counter2].start;
            finalobsfixedstop=obsfixedlist[counter2].stop;

            for (struct vector<FixedObs>::const_iterator vobs5=obsfixedlist.begin(); vobs5!=obsfixedlist.end(); vobs5++)
            {
                if (vobs5->start < obsfixedlist[counter2].stop
                    && obsfixedlist[counter2].stop <= vobs5->stop
                    && obsfixedlist[counter2].start <= vobs5->start
                    && counter2!=counter3)
                {
                    delta1=(obsfixedlist[counter2].stop-vobs5->start)/2;
                    finalobsfixedstop=obsfixedlist[counter2].stop-delta1;

                    ostringstream warndelta1;
                    warndelta1 << "Overlap between two fixed observations (";
                    warndelta1 << obsfixedlist[counter2].obskey << " ";
                    warndelta1 << vobs5->obskey << "). The stoptime of ";
                    warndelta1 << obsfixedlist[counter2].obskey << " has been changed.";
                    T::Warn(warndelta1);
                }
                if (vobs5->start <= obsfixedlist[counter2].start
                    && obsfixedlist[counter2].start < vobs5->stop
                    && obsfixedlist[counter2].stop >= vobs5->stop
                    && counter2!=counter3)
                {
                    delta2=(vobs5->stop-obsfixedlist[counter2].start)/2;
                    finalobsfixedstart=obsfixedlist[counter2].start+delta2;

                    ostringstream warndelta2;
                    warndelta2 << "Overlap between two fixed observations (";
                    warndelta2 << obsfixedlist[counter2].obskey << " ";
                    warndelta2 << vobs5->obskey << "). The starttime of ";
                    warndelta2 << obsfixedlist[counter2].obskey << " has been changed.";

                    T::Warn(warndelta2);
                }
                counter3++;
            }

            const int num=counter2-skipcounter;
            obslist.resize(num+1);
            obslist[num].obsstart=finalobsfixedstart;
            obslist[num].obsstop=finalobsfixedstop;
            obslist[num].sourcename_obs=obsfixedlist[counter2].sourcename;
            obslist[num].obsmode_obs=obsfixedlist[counter2].obsmode;
            obslist[num].obstype_obs=obsfixedlist[counter2].obstype;
            obslist[num].telsetup_obs=obsfixedlist[counter2].telsetup;
            obslist[num].sourcekey_obs=obsfixedlist[counter2].sourcekey;
            obslist[num].obskey_obs=obsfixedlist[counter2].obskey;
            counter2++;

            cout << "  " << vobs->sourcename <<  " " << vobs->start;
            cout << " - " << vobs->stop << endl;
        }
        ostringstream obsmsg;
        obsmsg << "Added " << obslist.size() << " fixed observations to ScheduledObs. ";
        T::Message(obsmsg);
        cout << "Added " << obslist.size() << " fixed observations to ScheduledObs. " << endl;

        for (int i=0; i<(int)obsstdlist.size(); i++)
        {
            for (int j=0; j<(int)obsstdlist.size(); j++)
            {
                if (obsstdlist[i].sourcekey_std == obsstdlist[j].sourcekey_std && i!=j)
                {
                    cout << "One double sourcekey in std observations: " << obsstdlist[j].sourcekey_std << endl;
                    ostringstream errdoublestd;
                    errdoublestd << "One double sourcekey in std observations: " << obsstdlist[j].sourcekey_std << " (" << obsstdlist[j].sourcename_std << ").";
                    T::Error(errdoublestd);
                    T::Message("Scheduling stopped.");
                    return error ? T::kSM_Error : T::kSM_Ready;
                }
            }
        }

        // loop over nights
        //   calculate sunset and sunrise
        //   check if there is already scheduled obs in that night
        //

        // in this loop the standard observations shall be
        // checked, evaluated
        // the observation times shall be calculated
        // and the observations added to the ScheduledObs list
        cout << "Standard Observations: " << endl;
        for (struct vector<StdObs>::const_iterator vobs2=obsstdlist.begin(); vobs2!=obsstdlist.end(); vobs2++)
        {
            cout << "  " << vobs2->sourcename_std << endl;
        }

        // in this loop the ScheduledRuns are filled
        //  (only data runs -> no runtype yet)
        // might be merged with next loop
        counter2=0;
        for (struct vector<ScheduledObs>::const_iterator vobs3=obslist.begin(); vobs3!=obslist.end(); vobs3++)
        {
            runlist.resize(counter2+1);
            runlist[counter2].runstart=obslist[counter2].obsstart;
            runlist[counter2].runstop=obslist[counter2].obsstop;
            runlist[counter2].sourcename_run=obslist[counter2].sourcename_obs;
            runlist[counter2].obsmode_run=obslist[counter2].obsmode_obs;
            runlist[counter2].obstype_run=obslist[counter2].obstype_obs;
            runlist[counter2].telsetup_run=obslist[counter2].telsetup_obs;
            runlist[counter2].sourcekey_run=obslist[counter2].sourcekey_obs;
            runlist[counter2].obskey_run=obslist[counter2].obskey_obs;
            counter2++;
            //cout << (*vobs3).sourcename_obs << endl;
        }

        //delete old scheduled runs from the DB
        const mysqlpp::SimpleResult res0 =
            conn.query("DELETE FROM ScheduledRun").execute();
        // FIXME: Maybe we have to check for a successfull
        //        query but an empty result
        /* throws exceptions
        if (!res0)
        {
            ostringstream msg;
            msg << "MySQL query failed: " << query0.error();
            T::Error(msg);
            return T::kSM_Error;
        }*/

        // in this loop the ScheduledRuns are inserted to the DB
        //   before the runtimes are adapted according to
        //   duration of P-Run, C-Run and repositioning
        counter3=0;
        int insertcount=0;
        ptime finalstarttime;
        ptime finalstoptime;
        for (struct vector<ScheduledRun>::const_iterator vobs4=runlist.begin(); vobs4!=runlist.end(); vobs4++)
        {
            for (int i=2; i<5; i++)
            {
                switch(i)
                {
                case 2:
                    finalstarttime=runlist[counter3].runstart+repostime+runtimec+runtimep;
                    finalstoptime=runlist[counter3].runstop;
                    break;
                case 3:
                    finalstarttime=runlist[counter3].runstart+repostime;
                    finalstoptime=runlist[counter3].runstart+runtimep+repostime;
                    break;
                case 4:
                    finalstarttime=runlist[counter3].runstart+runtimep+repostime;
                    finalstoptime=runlist[counter3].runstart+repostime+runtimep+runtimec;
                    break;
                }
                ostringstream q1;
                //cout << (*vobs4).sourcename_run << endl;
                q1 << "INSERT ScheduledRun set fStartTime='" << Time::sql << finalstarttime;
                q1 << "', fStopTime='" << Time::sql << finalstoptime;
                q1 << "', fSourceKEY='" << (*vobs4).sourcekey_run;
                q1 << "', fObservationKEY='" << (*vobs4).obskey_run;
                q1 << "', fRunTypeKEY='" << i;
                q1 << "', fTelescopeSetupKEY='" << (*vobs4).telsetup_run;
                q1 << "', fObservationTypeKEY='" << (*vobs4).obstype_run;
                q1 << "', fObservationModeKEY='" << (*vobs4).obsmode_run;
                q1 << "'";

                //cout << "executing query: " << q1.str() << endl;

                const mysqlpp::SimpleResult res1 = conn.query(q1.str()).execute();
                // FIXME: Maybe we have to check for a successfull
                //        query but an empty result
                /* throws exceptions
                if (!res1)
                {
                    ostringstream msg;
                    msg << "MySQL query failed: " << query1.error();
                    T::Error(str);
                    return T::kSM_Error;
                }*/
                insertcount++;
            }
            counter3++;
        }
        ostringstream insertmsg;
        insertmsg << "Inserted " << insertcount << " runs into the DB.";
        T::Message(insertmsg);
        //usleep(3000000);
        T::Message("Scheduling done.");

        return error;
    }

    /*
    // commit probably done by webinterface
    int Commit()
    {
        ostringstream str;
        str << "Comitting preview (id=" << fSessionId << ")";
        T::Message(str);

        usleep(3000000);
        T::Message("Comitted.");

        fSessionId = -1;

        bool error = false;
        return error ? T::kSM_Error : T::kSM_Ready;
    }
    */

    AutoScheduler(ostream &out=cout) : T(out, "SCHEDULER"), fNextIsPreview(true), fDBName("")
    {
        AddStateName(kSM_Scheduling, "Scheduling", "Scheduling in progress.");

        AddEvent(kSM_Scheduling, "SCHEDULE", "C", T::kSM_Ready)
            ("FIXME FIXME FIXME (explanation for the command)"
             "|database[string]:FIXME FIXME FIMXE (meaning and format)");

        AddEvent(T::kSM_Ready, "RESET", T::kSM_Error)
            ("Reset command to get out of the error state");

        //AddEvent(kSM_Comitting,  "COMMIT",   T::kSM_Ready);

        T::PrintListOfEvents();
    }

    int Execute()
    {
        switch (T::GetCurrentState())
        {
        case kSM_Scheduling:
            try
            {
                return Schedule() ? T::kSM_Error : T::kSM_Ready;
            }
            catch (const mysqlpp::Exception &e)
            {
                T::Error(string("MySQL: ")+e.what());
                return T::kSM_Error;
            }

            // This does an autmatic reset (FOR TESTING ONLY)
        case T::kSM_Error:
            return T::kSM_Ready;
        }
        return T::GetCurrentState();
    }

    int Transition(const Event &evt)
    {
        switch (evt.GetTargetState())
        {
        case kSM_Scheduling:
            if (evt.GetSize()>0)
                fDBName = evt.GetText();
            break;
        }

        return evt.GetTargetState();
    }

    int EvalOptions(Configuration &conf)
    {
        fDatabase       = conf.Get<string>("schedule-database");
        fDurationCalRun = conf.Get<int>("duration-cal-run");
        fDurationPedRun = conf.Get<int>("duration-ped-run");
        fDurationRepos  = conf.Get<int>("duration-repos");

        if (!conf.Has("schedule"))
            return -1;

        fDBName = conf.Get<string>("schedule");
        return Schedule();
    }

};


// ------------------------------------------------------------------------
#include "Main.h"

template<class T, class S>
int RunShell(Configuration &conf)
{
    return Main::execute<T, AutoScheduler<S>>(conf);
}

void SetupConfiguration(Configuration &conf)
{
    po::options_description control("Scheduler options");
    control.add_options()
        ("no-dim",    po_switch(),    "Disable dim services")
        ("schedule-database", var<string>()
#if BOOST_VERSION >= 104200
         ->required()
#endif
                                           ,  "Database link as in\n\tuser:password@server[:port]/database\nOverwrites options from the default configuration file.")
        ("schedule",          var<string>(),  "")
        ("mintime",           var<int>(),     "minimum observation time")
        ("duration-cal-run",  var<int>()
#if BOOST_VERSION >= 104200
         ->required()
#endif
                                           ,     "duration of calibration run [min]")
        ("duration-ped-run",  var<int>()
#if BOOST_VERSION >= 104200
         ->required()
#endif
                                           ,     "duration of pedestal run [min]")
        ("duration-repos",    var<int>()
#if BOOST_VERSION >= 104200
         ->required()
#endif
                                           ,     "duration of repositioning [min]")
        ;

    po::positional_options_description p;
    p.add("schedule", 1); // The first positional options

    conf.AddOptions(control);
    conf.SetArgumentPositions(p);
}

void PrintUsage()
{
    cout <<
        "The scheduler... TEXT MISSING\n"
        "\n"
        "The default is that the program is started without user intercation. "
        "All actions are supposed to arrive as DimCommands. Using the -c "
        "option, a local shell can be initialized. With h or help a short "
        "help message about the usuage can be brought to the screen.\n"
        "\n"
        "Usage: scheduler [-c type] [OPTIONS] <schedule-database>\n"
        "  or:  scheduler [OPTIONS] <schedule-database>\n";
    cout << endl;
}

void PrintHelp()
{
    /* Additional help text which is printed after the configuration
     options goes here */
}

int main(int argc, const char* argv[])
{
    Configuration conf(argv[0]);
    conf.SetPrintUsage(PrintUsage);
    Main::SetupConfiguration(conf);
    SetupConfiguration(conf);

    po::variables_map vm;
    try
    {
        vm = conf.Parse(argc, argv);
    }
#if BOOST_VERSION > 104000
    catch (po::multiple_occurrences &e)
    {
        cerr << "Program options invalid due to: " << e.what() << " of '" << e.get_option_name() << "'." << endl;
        return -1;
    }
#endif
    catch (exception& e)
    {
        cerr << "Program options invalid due to: " << e.what() << endl;
        return -1;
    }

//    try
    {
        // No console access at all
        if (!conf.Has("console"))
        {
            if (conf.Get<bool>("no-dim"))
                return RunShell<LocalStream, StateMachine>(conf);
            else
                return RunShell<LocalStream, StateMachineDim>(conf);
        }
        // Cosole access w/ and w/o Dim
        if (conf.Get<bool>("no-dim"))
        {
            if (conf.Get<int>("console")==0)
                return RunShell<LocalShell, StateMachine>(conf);
            else
                return RunShell<LocalConsole, StateMachine>(conf);
        }
        else
        {
            if (conf.Get<int>("console")==0)
                return RunShell<LocalShell, StateMachineDim>(conf);
            else
                return RunShell<LocalConsole, StateMachineDim>(conf);
        }
    }
/*    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }*/

    return 0;
}
