#ifndef FACT_Database
#define FACT_Database

#include <exception>
#include <boost/regex.hpp>

#include <mysql++/mysql++.h>

struct DatabaseName
{
    std::string user;
    std::string passwd;
    std::string server;
    std::string db;
    int port;

    DatabaseName(const std::string &database)
    {
        static const boost::regex expr("(([[:word:].-]+)(:(.+))?@)?([[:word:].-]+)(:([[:digit:]]+))?(/([[:word:].-]+))");

        boost::smatch what;
        if (!boost::regex_match(database, what, expr, boost::match_extra))
            throw std::runtime_error("Couldn't parse database URI '"+database+"'.");

        if (what.size()!=10)
            throw std::runtime_error("Error parsing database URI '"+database+"'.");

        user   = what[2];
        passwd = what[4];
        server = what[5];
        db     = what[9];

        try
        {
            port = stoi(std::string(what[7]));
        }
        catch (...)
        {
            port = 0;
        }
    }

    std::string uri() const
    {
        std::string rc;
        if (!user.empty())
            rc += user+"@";
        rc += server;
        if (port)
            rc += ":"+std::to_string(port);
        if (!db.empty())
            rc += "/"+db;
        return rc;
    }
};

class Database : public DatabaseName, public mysqlpp::Connection
{
public:
    Database(const std::string &desc) : DatabaseName(desc),
        mysqlpp::Connection(db.c_str(), server.c_str(), user.c_str(), passwd.c_str(), port)
    {
    }
};

#endif
