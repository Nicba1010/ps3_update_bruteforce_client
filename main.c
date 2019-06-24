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


char **id_list = 0;


#define ArrayCount(x) sizeof(x)/sizeof(*(x))

void generate_ids(uint64_t startPosition, uint64_t desiredCount)
{
    uint64_t currentPosition = 0;

    for (const char **ab = abPhys; ab < abPhys + ArrayCount(abPhys); ++ab)
    {
        for (const char **c = cPhys; c < cPhys + ArrayCount(cPhys); ++c)
        {
            for (const char **d = dPhys; d < dPhys + ArrayCount(dPhys); ++d)
            {
                for (int n = 0; n < 100000 && stb_sb_count(id_list) < desiredCount; n++, currentPosition++)
                {
                    if (currentPosition >= startPosition)
                    {
                        char *id = (char *) malloc(9);
                        sprintf(id, "%s%s%s%05d", *ab, *c, *d, n);
                        stb_sb_push(id_list, id);
                    }
                }
            }
        }
    }

    for (const char **a = aPhys; a < aPhys + ArrayCount(aPhys); ++a)
    {
        for (const char **b = bPhys; b < bPhys + ArrayCount(bPhys); ++b)
        {
            for (const char **c = cPhys; c < cPhys + ArrayCount(cPhys); ++c)
            {
                for (const char **d = dPhys; d < dPhys + ArrayCount(dPhys); ++d)
                {
                    for (int n = 0; n < 100000 && stb_sb_count(id_list) < desiredCount; n++, currentPosition++)
                    {
                        if (currentPosition >= startPosition)
                        {
                            char *id = (char *) malloc(9);
                            sprintf(id, "%s%s%s%s%05d", *a, *b, *c, *d, n);
                            stb_sb_push(id_list, id);
                        }
                    }
                }
            }
        }
    }

    for (const char **ab = abNet; ab < abNet + ArrayCount(abNet); ++ab)
    {
        for (const char **c = cNet; c < cNet + ArrayCount(cNet); ++c)
        {
            for (const char **d = dNet; d < dNet + ArrayCount(dNet); ++d)
            {
                for (int n = 0; n < 100000 && stb_sb_count(id_list) < desiredCount; n++, currentPosition++)
                {
                    if (currentPosition >= startPosition)
                    {
                        char *id = (char *) malloc(9);
                        sprintf(id, "%s%s%s%05d", *ab, *c, *d, n);
                        stb_sb_push(id_list, id);
                    }
                }
            }
        }
    }

    printf("%lu ids generated, %lld used!\n", currentPosition, stb_sb_count(id_list));
}

typedef struct
{
    const char *id;
    const char *data;
    uint32_t data_length;
    uint16_t http_status_code;
} http_response;

char *make_url_for_id(const char *id)
{
    char *url = (char *) malloc(sizeof("https://a0.ww.np.dl.playstation.net/tpl/np/AAAA00000/AAAA00000-ver.xml"));
    sprintf(url, "https://a0.ww.np.dl.playstation.net/tpl/np/%s/%s-ver.xml", id, id);
    return url;
}

static size_t write_cb(const char *data, size_t n, size_t l, void *curl_easy_handle)
{
    http_response *response;
    curl_easy_getinfo(curl_easy_handle, CURLINFO_PRIVATE, &response);

    long http_status_code;
    curl_easy_getinfo(curl_easy_handle, CURLINFO_RESPONSE_CODE, &http_status_code);
    response->http_status_code = (uint16_t) http_status_code;

    size_t total_size = n * l;
    response->data = data;
    response->data_length = (uint32_t) total_size;

    return total_size;
}

static void add_transfer(CURLM *curl_multi_handle, const char *id)
{
    CURL *curl_easy_handle = curl_easy_init();
//    curl_easy_setopt(curl_easy_handle, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl_easy_handle, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl_easy_handle, CURLOPT_WRITEDATA, curl_easy_handle);
    curl_easy_setopt(curl_easy_handle, CURLOPT_URL, make_url_for_id(id));
    curl_easy_setopt(curl_easy_handle, CURLOPT_SSL_VERIFYHOST, 0);
    curl_easy_setopt(curl_easy_handle, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl_easy_handle, CURLOPT_SSL_VERIFYSTATUS, 0);
    http_response *response = (http_response *) malloc(sizeof(http_response));
    response->id = id;
    response->http_status_code = 0;
    response->data = NULL;
    response->data_length = 0;
    curl_easy_setopt(curl_easy_handle, CURLOPT_PRIVATE, (void *) response);
    curl_multi_add_handle(curl_multi_handle, curl_easy_handle);
}

#define MAX_PARALLEL 2048
#define DEBUG 0

int main(void)
{
    // Open file
    FILE *f = fopen("result.bin", "wb");

    if(f == NULL)
    {
        fprintf(stderr, "Error opening result.bin!");
        return EXIT_FAILURE;
    }

    // Generate IDs
    generate_ids(0, 524288);

    CURLM *curl_multi_handle;
    CURLMsg *curl_message;
    int list_processing_position = 0;
    int messages_left = -1;
    int still_alive = 1;

    curl_global_init(CURL_GLOBAL_ALL);
    curl_multi_handle = curl_multi_init();

    /* Limit the amount of simultaneous connections curl should allow: */
    curl_multi_setopt(curl_multi_handle, CURLMOPT_MAXCONNECTS, (long) MAX_PARALLEL);

    /* Initialize tasks */
    for (list_processing_position = 0; list_processing_position < MAX_PARALLEL; list_processing_position++)
    {
        add_transfer(curl_multi_handle, id_list[list_processing_position]);
    }

    // Read start time
    clock_t start_time = clock();
    do
    {
        // Perform CURL requests
        curl_multi_perform(curl_multi_handle, &still_alive);

        while ((curl_message = curl_multi_info_read(curl_multi_handle, &messages_left)))
        {
            // Acquire CURL easy handle
            CURL *curl_easy_handle = curl_message->easy_handle;

            // Acquire api response struct
            http_response *response;
            curl_easy_getinfo(curl_easy_handle, CURLINFO_PRIVATE, &response);

            // Check if request succeeded
            if (curl_message->msg == CURLMSG_DONE)
            {
                if (response->http_status_code != 0)
                {
#if DEBUG
                    fprintf(
                            stdout,
                            "ID: %s\n"
                            "\tResponse code: %hu\n"
                            "\tResponse length: %d\n"
                            "\tResponse data: %s\n",
                            response->id,
                            response->http_status_code,
                            response->data_length,
                            response->data
                    );
#endif
                } else
                {
#if DEBUG
                    const char *url = NULL;
                    fprintf(
                            stderr,
                            "%s - CURL Internal error %d - %s - %s\n",
                            response->id,
                            curl_message->data.result,
                            curl_easy_strerror(curl_message->data.result),
                            make_url_for_id(response->id)
                    );
#endif
                }
                fwrite(response->id, 9, 1, f);
//                fwrite(&(response->http_status_code), sizeof(response->http_status_code), 1, f);
//                fwrite(&(response->data_length), sizeof(response->data_length), 1, f);
                // Cleanup
                curl_multi_remove_handle(curl_multi_handle, curl_easy_handle);
                curl_easy_cleanup(curl_easy_handle);
            } else
            {
                fprintf(stderr, "E: Internal CURL error (%d)\n", curl_message->msg);
                // Retry
                add_transfer(curl_multi_handle, response->id);
            }

            // Add new request if ids available
            if (list_processing_position < stb_sb_count(id_list))
            {
                add_transfer(curl_multi_handle, id_list[++list_processing_position]);
            }
        }
        if (still_alive)
        {
            curl_multi_wait(curl_multi_handle, NULL, 0, 500, NULL);
        }
    } while (still_alive || (list_processing_position < stb_sb_count(id_list)));

    // Read end time
    clock_t end_time = clock();

    // Calculate time taken
    long time_taken = (long) (((float) (end_time - start_time) / CLOCKS_PER_SEC) * 1000);
    printf("Operation took %ld ms!\n", time_taken);

    // Calculate requests per second
    int requestsPerSecond = (int) ((float) stb_sb_count(id_list) / ((float) time_taken / 1000));
    printf("Average requests per second is %d!\n", requestsPerSecond);

    // Cleanup
    printf("Closing file handle!");
    fclose(f);
    curl_multi_cleanup(curl_multi_handle);
    curl_global_cleanup();

    return EXIT_SUCCESS;
}