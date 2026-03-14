#include <assert.h>
#include <stddef.h>

#include "wl80211_mac.h"

#define WG wl80211_glb

const char *wl80211_default_country = "CN";

const struct ieee80211_dot_d country_list[] = {
    /*First Country is the default country*/
    {
        .code = "CN",
        .channel24G_num = 13,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 },
        .channel5G_num = 13,
        .channel5G_chan = { 36, 40, 44, 48, 52, 56, 60, 64, 149, 153, 157, 161, 165 },
    },
    /* CN country code for SRRC */
    {
        .code = "CS",
        .channel24G_num = 13,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 },
        .channel5G_num = 13,
        .channel5G_chan = { 36, 40, 44, 48, 52, 56, 60, 64, 149, 153, 157, 161, 165 },
    },
    {
        .code = "JP",
        .channel24G_num = 14,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 },
        .channel5G_num = 20,
        .channel5G_chan = { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 144 },
    },
    {
        .code = "US",
        .channel24G_num = 11,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
        .channel5G_num = 24,
        .channel5G_chan = { 36,  40,  44,  48,  52,  56,  60,  64,  100, 104, 108, 112,
                            116, 120, 124, 128, 132, 136, 140, 149, 153, 157, 161, 165 },
    },
    {
        .code = "EU",
        .channel24G_num = 13,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 },
        .channel5G_num = 19,
        .channel5G_chan = { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140 },
    },

    /* FCC Regulation (11 2.4G channels, 24 5G channels) */
    {
        .code = "CA",
        .channel24G_num = 11,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
        .channel5G_num = 24,
        .channel5G_chan = { 36,  40,  44,  48,  52,  56,  60,  64,  100, 104, 108, 112,
                            116, 120, 124, 128, 132, 136, 140, 149, 153, 157, 161, 165 },
    },
    {
        .code = "TW",
        .channel24G_num = 11,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
        .channel5G_num = 24,
        .channel5G_chan = { 36,  40,  44,  48,  52,  56,  60,  64,  100, 104, 108, 112,
                            116, 120, 124, 128, 132, 136, 140, 149, 153, 157, 161, 165 },
    },
    {
        .code = "KR",
        .channel24G_num = 11,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
        .channel5G_num = 24,
        .channel5G_chan = { 36,  40,  44,  48,  52,  56,  60,  64,  100, 104, 108, 112,
                            116, 120, 124, 128, 132, 136, 140, 149, 153, 157, 161, 165 },
    },
    {
        .code = "TH",
        .channel24G_num = 11,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
        .channel5G_num = 24,
        .channel5G_chan = { 36,  40,  44,  48,  52,  56,  60,  64,  100, 104, 108, 112,
                            116, 120, 124, 128, 132, 136, 140, 149, 153, 157, 161, 165 },
    },
    {
        .code = "IN",
        .channel24G_num = 11,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
        .channel5G_num = 24,
        .channel5G_chan = { 36,  40,  44,  48,  52,  56,  60,  64,  100, 104, 108, 112,
                            116, 120, 124, 128, 132, 136, 140, 149, 153, 157, 161, 165 },
    },
    {
        .code = "AU",
        .channel24G_num = 11,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
        .channel5G_num = 24,
        .channel5G_chan = { 36,  40,  44,  48,  52,  56,  60,  64,  100, 104, 108, 112,
                            116, 120, 124, 128, 132, 136, 140, 149, 153, 157, 161, 165 },
    },
    {
        .code = "NZ",
        .channel24G_num = 11,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
        .channel5G_num = 24,
        .channel5G_chan = { 36,  40,  44,  48,  52,  56,  60,  64,  100, 104, 108, 112,
                            116, 120, 124, 128, 132, 136, 140, 149, 153, 157, 161, 165 },
    },
    {
        .code = "BR",
        .channel24G_num = 11,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
        .channel5G_num = 24,
        .channel5G_chan = { 36,  40,  44,  48,  52,  56,  60,  64,  100, 104, 108, 112,
                            116, 120, 124, 128, 132, 136, 140, 149, 153, 157, 161, 165 },
    },

    /* ETSI Regulation (13 2.4G channels, 19 5G channels) */
    {
        .code = "GB",
        .channel24G_num = 13,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 },
        .channel5G_num = 19,
        .channel5G_chan = { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140 },
    },
    {
        .code = "CH",
        .channel24G_num = 13,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 },
        .channel5G_num = 19,
        .channel5G_chan = { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140 },
    },
    {
        .code = "AT",
        .channel24G_num = 13,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 },
        .channel5G_num = 19,
        .channel5G_chan = { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140 },
    },
    {
        .code = "BE",
        .channel24G_num = 13,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 },
        .channel5G_num = 19,
        .channel5G_chan = { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140 },
    },
    {
        .code = "BG",
        .channel24G_num = 13,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 },
        .channel5G_num = 19,
        .channel5G_chan = { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140 },
    },
    {
        .code = "HR",
        .channel24G_num = 13,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 },
        .channel5G_num = 19,
        .channel5G_chan = { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140 },
    },
    {
        .code = "CY",
        .channel24G_num = 13,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 },
        .channel5G_num = 19,
        .channel5G_chan = { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140 },
    },
    {
        .code = "CZ",
        .channel24G_num = 13,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 },
        .channel5G_num = 19,
        .channel5G_chan = { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140 },
    },
    {
        .code = "DK",
        .channel24G_num = 13,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 },
        .channel5G_num = 19,
        .channel5G_chan = { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140 },
    },
    {
        .code = "EE",
        .channel24G_num = 13,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 },
        .channel5G_num = 19,
        .channel5G_chan = { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140 },
    },
    {
        .code = "FI",
        .channel24G_num = 13,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 },
        .channel5G_num = 19,
        .channel5G_chan = { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140 },
    },
    {
        .code = "FR",
        .channel24G_num = 13,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 },
        .channel5G_num = 19,
        .channel5G_chan = { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140 },
    },
    {
        .code = "DE",
        .channel24G_num = 13,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 },
        .channel5G_num = 19,
        .channel5G_chan = { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140 },
    },
    {
        .code = "GR",
        .channel24G_num = 13,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 },
        .channel5G_num = 19,
        .channel5G_chan = { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140 },
    },
    {
        .code = "HU",
        .channel24G_num = 13,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 },
        .channel5G_num = 19,
        .channel5G_chan = { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140 },
    },
    {
        .code = "IE",
        .channel24G_num = 13,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 },
        .channel5G_num = 19,
        .channel5G_chan = { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140 },
    },
    {
        .code = "IT",
        .channel24G_num = 13,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 },
        .channel5G_num = 19,
        .channel5G_chan = { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140 },
    },
    {
        .code = "LV",
        .channel24G_num = 13,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 },
        .channel5G_num = 19,
        .channel5G_chan = { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140 },
    },
    {
        .code = "LT",
        .channel24G_num = 13,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 },
        .channel5G_num = 19,
        .channel5G_chan = { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140 },
    },
    {
        .code = "LU",
        .channel24G_num = 13,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 },
        .channel5G_num = 19,
        .channel5G_chan = { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140 },
    },
    {
        .code = "MT",
        .channel24G_num = 13,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 },
        .channel5G_num = 19,
        .channel5G_chan = { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140 },
    },
    {
        .code = "NL",
        .channel24G_num = 13,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 },
        .channel5G_num = 19,
        .channel5G_chan = { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140 },
    },
    {
        .code = "PL",
        .channel24G_num = 13,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 },
        .channel5G_num = 19,
        .channel5G_chan = { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140 },
    },
    {
        .code = "PT",
        .channel24G_num = 13,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 },
        .channel5G_num = 19,
        .channel5G_chan = { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140 },
    },
    {
        .code = "RO",
        .channel24G_num = 13,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 },
        .channel5G_num = 19,
        .channel5G_chan = { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140 },
    },
    {
        .code = "SK",
        .channel24G_num = 13,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 },
        .channel5G_num = 19,
        .channel5G_chan = { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140 },
    },
    {
        .code = "SI",
        .channel24G_num = 13,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 },
        .channel5G_num = 19,
        .channel5G_chan = { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140 },
    },
    {
        .code = "ES",
        .channel24G_num = 13,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 },
        .channel5G_num = 19,
        .channel5G_chan = { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140 },
    },
    {
        .code = "SE",
        .channel24G_num = 13,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 },
        .channel5G_num = 19,
        .channel5G_chan = { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140 },
    },

    /* ROW (Rest of World) - 11 2.4G channels, 25 5G channels */
    {
        .code = "WW",
        .channel24G_num = 11,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
        .channel5G_num = 25,
        .channel5G_chan = { 36,  40,  44,  48,  52,  56,  60,  64,  100, 104, 108, 112, 116,
                            120, 124, 128, 132, 136, 140, 144, 149, 153, 157, 161, 165 },
    },

    /* FCC Regulation - Colombia, Dominican Republic, Guatemala, Mexico, Panama, Puerto Rico */
    {
        .code = "CO",
        .channel24G_num = 11,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
        .channel5G_num = 24,
        .channel5G_chan = { 36,  40,  44,  48,  52,  56,  60,  64,  100, 104, 108, 112,
                            116, 120, 124, 128, 132, 136, 140, 149, 153, 157, 161, 165 },
    },
    {
        .code = "DO",
        .channel24G_num = 11,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
        .channel5G_num = 24,
        .channel5G_chan = { 36,  40,  44,  48,  52,  56,  60,  64,  100, 104, 108, 112,
                            116, 120, 124, 128, 132, 136, 140, 149, 153, 157, 161, 165 },
    },
    {
        .code = "GT",
        .channel24G_num = 11,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
        .channel5G_num = 24,
        .channel5G_chan = { 36,  40,  44,  48,  52,  56,  60,  64,  100, 104, 108, 112,
                            116, 120, 124, 128, 132, 136, 140, 149, 153, 157, 161, 165 },
    },
    {
        .code = "MX",
        .channel24G_num = 11,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
        .channel5G_num = 24,
        .channel5G_chan = { 36,  40,  44,  48,  52,  56,  60,  64,  100, 104, 108, 112,
                            116, 120, 124, 128, 132, 136, 140, 149, 153, 157, 161, 165 },
    },
    {
        .code = "PA",
        .channel24G_num = 11,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
        .channel5G_num = 24,
        .channel5G_chan = { 36,  40,  44,  48,  52,  56,  60,  64,  100, 104, 108, 112,
                            116, 120, 124, 128, 132, 136, 140, 149, 153, 157, 161, 165 },
    },
    {
        .code = "PR",
        .channel24G_num = 11,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
        .channel5G_num = 24,
        .channel5G_chan = { 36,  40,  44,  48,  52,  56,  60,  64,  100, 104, 108, 112,
                            116, 120, 124, 128, 132, 136, 140, 149, 153, 157, 161, 165 },
    },

    /* ETSI Regulation - Uzbekistan, Israel */
    {
        .code = "UZ",
        .channel24G_num = 13,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 },
        .channel5G_num = 19,
        .channel5G_chan = { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140 },
    },
    {
        .code = "IL",
        .channel24G_num = 13,
        .channel24G_chan = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 },
        .channel5G_num = 19,
        .channel5G_chan = { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140 },
    },
};

int wl80211_set_country_code(const char *country_code)
{
    int ret, i;

    if (!country_code) {
        return -1;
    }

    for (i = 0; i < sizeof(country_list) / sizeof(country_list[0]); i++) {
        if (0 == strcmp(country_list[i].code, country_code)) {
            break;
        }
    }

    if (i == sizeof(country_list) / sizeof(country_list[0])) {
        return -1;
    }

    WG.country = &country_list[i];

    ret = wl80211_mac_chan_config_update(country_list[i].channel24G_num, &country_list[i].channel24G_chan[0],
                                         country_list[i].channel5G_num, &country_list[i].channel5G_chan[0]);
    if (ret != 0) {
        wl80211_printf("%s channel config failed\r\n", __func__);
        return -1;
    }

    return 0;
}
