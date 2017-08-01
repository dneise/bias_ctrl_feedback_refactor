#ifndef FACT_HeadersFeedback
#define FACT_HeadersFeedback

#include "HeadersBIAS.h"

namespace Feedback
{
    const int NumBiasChannels = 320;
    const int NumberSix = 6;
    const double DefaultOverVoltage = 1.1;
    const double AlsoDefaultOverVoltage = 1.4;
    const int NumberTwentyFive = 25;
    const int ABrokenBoard = 272;

    const double c1  =  1e-3 / 4096;
    const double c5  =  5e-3 / 4096;
    const double c10 = 10e-3 / 4096;

    const float DefaultOperationTemperature = 25.0;

    namespace State
    {
        enum states_t
        {
            kDimNetworkNA = 1,
            kDisconnected,
            kConnecting,
            kConnected,

            kCalibrating,
            kCalibrated,

            kWaitingForData,
            kInProgress,

            kWarning,
            kCritical,
            kOnStandby,
        };
    }

    struct DimCurrents_t
    {
        float I[BIAS::kNumChannels];
        float Iavg;
        float Irms;
        float Imed;
        float Idev;
        uint32_t N;
        float Tdiff;
        float Uov[BIAS::kNumChannels];
        float Unom;
        float dUtemp;

        DimCurrents_t()
        {
            memset(
                this,
                0,
                sizeof(DimCurrents_t)
            );
        }
    } __attribute__((__packed__));

    struct CalibData_t
    {
        uint32_t dac;
        float    U[BIAS::kNumChannels];
        float    Iavg[BIAS::kNumChannels];
        float    Irms[BIAS::kNumChannels];

        CalibData_t() { memset(this, 0, sizeof(CalibData_t)); }
    } __attribute__((__packed__));

    const std::vector< std::vector<int> > rings_of_order = {{
        {
             62,  63, 130, 131, 132, 133, 134,
            135, 222, 223, 292, 293, 294, 295,
        },
        {
             58,  59,  60,  61, 129, 138, 139, 140, 141, 142, 143, 218,
            219, 220, 221, 290, 291, 298, 299, 300, 301, 302, 303,
        },
        {
             42,  43,  44,  45,  55,  56,  57,  70,  71,  78,  79,
             96,  97,  98,  99, 102, 103, 128, 136, 137, 159, 202,
            203, 204, 205, 214, 216, 217, 228, 230, 231, 256, 257,
            258, 259, 262, 263, 288, 289, 296, 297, 310, 318
        },
    }};

    const std::vector<int> broken_channels = {{272}};

}

#endif
