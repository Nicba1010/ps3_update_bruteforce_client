#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#ifndef WIN32

#include <unistd.h>

#endif

#include "stretchy.c"
#include <curl/curl.h>

const char *abPhys[] = {
//        "PE", // PS1 and PS2 CS disc for Europe, Australia & Gulf area
//        "PT", // PS1 and PS2 CS disc for Japan & Asia
//        "PU", // PS1 and PS2 CS disc for USA & Canada
};

const char *aPhys[] = {
        "B", // PS3 Bluray
//        "P", // PS4 Bluray
//        "S", // PS1, PS2, PS3 CD/DVD
//        "U", // PSP UMD
//        "V", // PSvita Card
        "X", // PS3 Bluray + Extras (PSvita crossbuy)
};

const char *bPhys[] = {
        "C",// Copyrighted by sony
        "L", // Licensed to sony
};

const char *cPhys[] = {
        "A", //Asia
        "E", //Europe
        "H", //Hong Kong
        "J", //Japan
        "K", //Korea
        "P", //Japan (PS1/PS2)
        "U", //USA
};

const char *dPhys[] = {
        "B", // Peripheral Software
        "C", // System Firmware
        "D", // Demo
        "M", // Malayan Release
        "S", // Retail release
        "T", // (closed) Betas
        "V", // Multi Region PS3 CS disc
        "X", // Install disc? (Demos, Store Videos)
        "Z", // Region locked PS3 CS disc
};

const char *abNet[] = {
        "NP", // Retail
};

const char *cNet[] = {
        "A", // Asia
        "E", // Europe
        "H", // Hong Kong
        "J", // Japan
        "K", // Korea
        "U", // USA
        "I", // Internal ? (Sony)
        "X", // Firmware/SDK Sample
};

const char *dNet[] = {
        "A", // First Party PS3 (Demo/Retail)
        "B", // Licensed PS3 (Demo/Retail)
        "C", // First Party PS2 Classic (Demo/Retail)
        "D", // Licensed PS2 Classic (Demo/Retail)
        "E", // Licensed PS1 Classic (PAL) (Demo/Retail)
        "F", // First Party PS1 Classic (PAL) (Demo/Retail)
        "G", // First Party PSP (Demo/Retail)
        "H", // Licensed PSP (Demo/Retail)
        "I", // First Party PS1 Classic (NTSC) (Demo/Retail)
        "J", // Licensed PS1 Classic (NTSC) (Demo/Retail)
        "K", // First Party Game related Content
        "L", // Licensed Game related Content
        "M", // Music
        "N", // Game Soundtracks
        "O", // Other
        "P", // ?
        "Q", // ?
        "R", // ?
        "S", // System
        "T", // ?
        "U", // ?
        "V", // ?
        "W", // First Party PSP Remasters
        "X", // First Party PSP Minis
        "Y", // Third Party PSP Remasters
        "Z", // Third Party PSP minis
};


char **list = 0;


#define ArrayCount(x) sizeof(x)/sizeof(*(x))

void generate_ids(uint64_t startPosition, uint64_t desiredCount) {
    uint64_t currentPosition = 0;

    for (const char **ab = abPhys; ab < abPhys + ArrayCount(abPhys); ++ab) {
        for (const char **c = cPhys; c < cPhys + ArrayCount(cPhys); ++c) {
            for (const char **d = dPhys; d < dPhys + ArrayCount(dPhys); ++d) {
                for (int n = 0; n < 100000 && stb_sb_count(list) < desiredCount; n++, currentPosition++) {
                    if (currentPosition >= startPosition) {
                        char *id = (char *) malloc(9);
                        sprintf(id, "%s%s%s%05d", *ab, *c, *d, n);
                        stb_sb_push(list, id);
                    }
                }
            }
        }
    }

    for (const char **a = aPhys; a < aPhys + ArrayCount(aPhys); ++a) {
        for (const char **b = bPhys; b < bPhys + ArrayCount(bPhys); ++b) {
            for (const char **c = cPhys; c < cPhys + ArrayCount(cPhys); ++c) {
                for (const char **d = dPhys; d < dPhys + ArrayCount(dPhys); ++d) {
                    for (int n = 0; n < 100000 && stb_sb_count(list) < desiredCount; n++, currentPosition++) {
                        if (currentPosition >= startPosition) {
                            char *id = (char *) malloc(9);
                            sprintf(id, "%s%s%s%s%05d", *a, *b, *c, *d, n);
                            stb_sb_push(list, id);
                        }
                    }
                }
            }
        }
    }

    for (const char **ab = abNet; ab < abNet + ArrayCount(abNet); ++ab) {
        for (const char **c = cNet; c < cNet + ArrayCount(cNet); ++c) {
            for (const char **d = dNet; d < dNet + ArrayCount(dNet); ++d) {
                for (int n = 0; n < 100000 && stb_sb_count(list) < desiredCount; n++, currentPosition++) {
                    if (currentPosition >= startPosition) {
                        char *id = (char *) malloc(9);
                        sprintf(id, "%s%s%s%05d", *ab, *c, *d, n);
                        stb_sb_push(list, id);
                    }
                }
            }
        }
    }

    printf("%lu ids generated, %lld used!\n", currentPosition, stb_sb_count(list));
}

static size_t write_cb(char *data, size_t n, size_t l, void *userp) {
    /* take care of the data here, ignored in this example */
    (void) data;
    (void) userp;
    return n * l;
}

static void add_transfer(CURLM *cm, int i) {
    CURL *eh = curl_easy_init();
    char *url = (char *) malloc(sizeof("https://a0.ww.np.dl.playstation.net/tpl/np/AAAA00000/AAAA00000-ver.xml"));
    sprintf(url, "https://a0.ww.np.dl.playstation.net/tpl/np/%s/%s-ver.xml", list[i], list[i]);
    curl_easy_setopt(eh, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(eh, CURLOPT_URL, url);
    curl_easy_setopt(eh, CURLOPT_SSL_VERIFYHOST, 0);
    curl_easy_setopt(eh, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(eh, CURLOPT_SSL_VERIFYSTATUS, 0);
    curl_easy_setopt(eh, CURLOPT_PRIVATE, list[i]);
    curl_multi_add_handle(cm, eh);
}

#define MAX_PARALLEL 2048
#define DEBUG 0

int main(void) {
    generate_ids(0, 200000);

    CURLM *cm;
    CURLMsg *msg;
    int transfers = 0;
    int msgs_left = -1;
    int still_alive = 1;

    curl_global_init(CURL_GLOBAL_ALL);
    cm = curl_multi_init();

    /* Limit the amount of simultaneous connections curl should allow: */
    curl_multi_setopt(cm, CURLMOPT_MAXCONNECTS, (long) MAX_PARALLEL);

    for (transfers = 0; transfers < MAX_PARALLEL; transfers++) {
        add_transfer(cm, transfers);
    }

    clock_t start_time, end_time;
    start_time = clock();
    do {
        curl_multi_perform(cm, &still_alive);

        while ((msg = curl_multi_info_read(cm, &msgs_left))) {
            if (msg->msg == CURLMSG_DONE) {
                CURL *e = msg->easy_handle;
#if DEBUG
                char *url;
                long http_code;
                curl_easy_getinfo(e, CURLINFO_PRIVATE, &url);
                curl_easy_getinfo(e, CURLINFO_RESPONSE_CODE, &http_code),
                        fprintf(
                                stderr,
                                "R: %d - %ld - %s <%s>\n",
                                msg->data.result,
                                http_code,
                                curl_easy_strerror(msg->data.result),
                                url
                        );
#endif
                curl_multi_remove_handle(cm, e);
                curl_easy_cleanup(e);
            } else {
                fprintf(stderr, "E: CURLMsg (%d)\n", msg->msg);
            }
            if (transfers < stb_sb_count(list)) {
                add_transfer(cm, transfers++);
            }
        }
        if (still_alive) {
            curl_multi_wait(cm, NULL, 0, 500, NULL);
        }
    } while (still_alive || (transfers < stb_sb_count(list)));
    end_time = clock();

    long time_taken = (long) (((float) (end_time - start_time) / CLOCKS_PER_SEC) * 1000);
    printf("Operation took %ld ms!\n", time_taken);
    int requestsPerSecond = (int) ((float) stb_sb_count(list) / ((float) time_taken / 1000));
    printf("Average requests per second is %d!\n", requestsPerSecond);

    curl_multi_cleanup(cm);
    curl_global_cleanup();

    return EXIT_SUCCESS;
}