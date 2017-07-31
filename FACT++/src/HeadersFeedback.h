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
                sizeof(dim_data)
            );
        }
    } __attribute__((__packed__));

}

#endif
