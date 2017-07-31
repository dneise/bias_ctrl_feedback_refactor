#ifndef FACT_Tools
#define FACT_Tools

#include <map>
#include <string>
#include <vector>

namespace Tools
{
    std::string Format(const char *fmt, va_list &ap);
    std::string Form(const char *fmt, ...);
    std::string Trim(const std::string &str);
    std::string TrimQuotes(const std::string &str);
    std::string Wrap(std::string &str, size_t width=78);
    std::string Scientific(uint64_t val);

    std::map<std::string,std::string> Split(std::string &, bool = false);
    std::vector<std::string> Split(const std::string &, const std::string &);
    std::string Uncomment(const std::string &opt);

    std::vector<std::string> WordWrap(std::string, const uint16_t & = 80);

    template<typename T>
        uint16_t Fletcher16(const T *t, size_t cnt)
    {
        const uint8_t *data = reinterpret_cast<const uint8_t*>(t);

        size_t bytes = cnt*sizeof(T);

        uint16_t sum1 = 0xff;
        uint16_t sum2 = 0xff;

        while (bytes) 
        {
            size_t tlen = bytes > 20 ? 20 : bytes;
            bytes -= tlen;

            do {
                sum2 += sum1 += *data++;
            } while (--tlen);

            sum1 = (sum1 & 0xff) + (sum1 >> 8);
            sum2 = (sum2 & 0xff) + (sum2 >> 8);
        }

        // Second reduction step to reduce sums to 8 bits
        sum1 = (sum1 & 0xff) + (sum1 >> 8);
        sum2 = (sum2 & 0xff) + (sum2 >> 8);

        return sum2 << 8 | sum1;
    }

}

#endif
