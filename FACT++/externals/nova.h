#ifndef MARS_Nova
#define MARS_Nova

#include <map>
#include <string>
#include <algorithm>

#include <libnova/solar.h>
#include <libnova/lunar.h>
#include <libnova/rise_set.h>
#include <libnova/transform.h>
#include <libnova/angular_separation.h>

namespace Nova
{
    typedef ln_rst_time RstTime; // Note that times are in JD not Mjd

    struct ZdAzPosn;
    struct RaDecPosn;

#ifndef __CINT__
#define OBS(lat,lng) ={lat,lng}
#else
#define OBS(lat,lng)
#endif

    const static ln_lnlat_posn kORM  OBS(-(17.+53./60+26.525/3600), 28.+45./60+42.462/3600); // 2800m
    const static ln_lnlat_posn kHAWC OBS( -97.3084,                 18.9947               ); // 4100m;
    const static ln_lnlat_posn kSPM  OBS(-115.4637,                 31.0439               ); // 2800m;
    const static ln_lnlat_posn kRWTH OBS(   6.0657,                 50.7801               );

    struct LnLatPosn : public ln_lnlat_posn
    {
        LnLatPosn() { }
        LnLatPosn(const ln_lnlat_posn &pos) : ln_lnlat_posn(pos) { }
        LnLatPosn(std::string obs)
        {
#ifndef __CINT__
            static const std::map<std::string, ln_lnlat_posn> lut =
            {
                { "ORM",  kORM  },
                { "HAWC", kHAWC },
                { "SPM",  kSPM  },
                { "RWTH", kRWTH },
            };

            for (auto &c: obs)
                c = toupper(c);

            const auto it = lut.find(obs);
            *this = it==lut.end() ? ln_lnlat_posn({std::numeric_limits<double>::quiet_NaN(),std::numeric_limits<double>::quiet_NaN()}) : it->second;
#endif
        }
        bool isValid() const
        {
            return std::isfinite(lat) && std::isfinite(lng);
        }
    };

    // Warning: 0deg=South, 90deg=W
    struct HrzPosn : public ln_hrz_posn
    {
        HrzPosn() { }
        HrzPosn(const ZdAzPosn &);
    };

    struct ZdAzPosn
    {
        double zd; // [deg]
        double az; // [deg]

        ZdAzPosn(double z=0, double a=0) : zd(z), az(a) { }
        ZdAzPosn(const HrzPosn &hrz) : zd(90-hrz.alt), az(hrz.az-180) { }

        // This crahsed cint, but it could save up the dedicate structure HrzPosn :(
        //operator HrzPosn() const { const HrzPosn p = { az-180, 90-zd}; return p; }
    };

    HrzPosn::HrzPosn(const ZdAzPosn &za) { alt = 90-za.zd; az = za.az-180; }


    // Note that right ascension is stored in degree
    struct EquPosn : public ln_equ_posn
    {
        EquPosn() { }
        EquPosn(const RaDecPosn &);
    };

    struct RaDecPosn
    {
        double ra;  // [h]
        double dec; // [deg]

        RaDecPosn(double r=0, double d=0) : ra(r), dec(d) { }
        RaDecPosn(const EquPosn &equ) : ra(equ.ra/15), dec(equ.dec) { }

        // This crahsed cint, but it could save up the dedicate structure HrzPosn :(
        //operator HrzPosn() const { const HrzPosn p = { az-180, 90-zd}; return p; }
    };

    EquPosn::EquPosn(const RaDecPosn &rd) { ra = rd.ra*15; dec = rd.dec; }

    HrzPosn GetHrzFromEqu(const EquPosn &equ, const LnLatPosn &obs, double jd)
    {
        HrzPosn hrz;
        ln_get_hrz_from_equ(const_cast<EquPosn*>(&equ), const_cast<LnLatPosn*>(&obs), jd, &hrz);
        return hrz;
    }
    HrzPosn GetHrzFromEqu(const EquPosn &equ, double jd)
    {
        return GetHrzFromEqu(equ, kORM, jd);
    }

    EquPosn GetEquFromHrz(const HrzPosn &hrz, const LnLatPosn &obs, double jd)
    {
        EquPosn equ;
        ln_get_equ_from_hrz(const_cast<HrzPosn*>(&hrz), const_cast<LnLatPosn*>(&obs), jd, &equ);
        return equ;
    }
    EquPosn GetEquFromHrz(const HrzPosn &hrz, double jd)
    {
        return GetEquFromHrz(hrz, kORM, jd);
    }

    RstTime GetSolarRst(double jd, const LnLatPosn &obs, double hrz=LN_SOLAR_STANDART_HORIZON)
    {
        RstTime rst;
        ln_get_solar_rst_horizon(jd, const_cast<LnLatPosn*>(&obs), hrz, &rst);
        return rst;
    }
    RstTime GetSolarRst(double jd, double hrz=LN_SOLAR_STANDART_HORIZON)
    {
        return GetSolarRst(jd, kORM, hrz);
    }

    RstTime GetLunarRst(double jd, const LnLatPosn &obs=kORM)
    {
        RstTime rst;
        ln_get_lunar_rst(jd, const_cast<LnLatPosn*>(&obs), &rst);
        return rst;
    }
    EquPosn GetSolarEquCoords(double jd)
    {
        EquPosn equ;
        ln_get_solar_equ_coords(jd, &equ);
        return equ;
    }

    double GetLunarDisk(double jd)
    {
        return ln_get_lunar_disk(jd);
    }

    double GetLunarSdiam(double jd)
    {
        return ln_get_lunar_sdiam(jd);
    }

    double GetLunarPhase(double jd)
    {
        return ln_get_lunar_phase(jd);
    }

    EquPosn GetLunarEquCoords(double jd, double precision=0)
    {
        EquPosn equ;
        ln_get_lunar_equ_coords_prec(jd, &equ, precision);
        return equ;
    }

    double GetLunarEarthDist(double jd)
    {
        return ln_get_lunar_earth_dist(jd);
    }

    double GetAngularSeparation(const EquPosn &p1, const EquPosn &p2)
    {
        return ln_get_angular_separation(const_cast<EquPosn*>(&p1), const_cast<EquPosn*>(&p2));
    }

    double GetAngularSeparation(const HrzPosn &h1, const HrzPosn &h2)
    {
        EquPosn p1; p1.ra=h1.az; p1.dec=h1.alt;
        EquPosn p2; p2.ra=h2.az; p2.dec=h2.alt;
        return ln_get_angular_separation(&p1, &p2);
    }

    struct SolarObjects
    {
        double  fJD;

        EquPosn fSunEqu;
        HrzPosn fSunHrz;

        EquPosn fMoonEqu;
        HrzPosn fMoonHrz;
        double  fMoonDisk;

        double fEarthDist;

        SolarObjects(const double &jd, const LnLatPosn &obs=Nova::kORM)
        {
            fJD = jd;

            // Sun properties
            fSunEqu    = GetSolarEquCoords(jd);
            fSunHrz    = GetHrzFromEqu(fSunEqu, obs, jd);

            // Moon properties
            fMoonEqu   = GetLunarEquCoords(jd, 0.01);
            fMoonHrz   = GetHrzFromEqu(fMoonEqu, obs, jd);

            fMoonDisk  = GetLunarDisk(jd);
            fEarthDist = GetLunarEarthDist(jd);
        }
    };

}

#endif
