#ifndef NDS_BANNER_H
#define NDS_BANNER_H

#include <stdint.h>

// From GBATEK

struct NDSBanner
{
    uint16_t version;
    uint16_t crc16[4];
    uint8_t reserved1[22];
    uint8_t bitmap[512];
    uint16_t palette[16];

    char16_t jp_title[128];
    char16_t en_title[128];
    char16_t fr_title[128];
    char16_t de_title[128];
    char16_t it_title[128];
    char16_t es_title[128];
    char16_t cn_title[128];
    char16_t kr_title[128];

    uint8_t reserved2[2048];
};

static_assert(sizeof(NDSBanner) == 4672, "NDSBanner is not 4672 bytes!");


#endif //NDS_HEADER_H