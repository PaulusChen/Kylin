
#include <inttypes.h>
#include <curl/curl.h>

#include "KHttpHelper.h"
#include "Tools.h"
#include "KLog.h"
#include "KylinTypes.h"

struct KHttpHelper{
    char url[MAX_URL_LEN];
    CURL *pCurl;
};

KHttpHelper_t *KCreateHttpHelper(const char *url) {

    KHttpHelper_t *newHelper = ObjAlloc(KHttpHelper_t);
    uint64_t urlLen = strlen(url);
    if (urlLen > MAX_URL_LEN) {
        return NULL;
    }

    strcpy(newHelper->url,url);
    newHelper->pCurl = curl_easy_init();
    if (newHelper->pCurl == NULL) {
        DistoryHttpHelper(newHelper);
        return NULL;
    }

    CURL *hCurl = newHelper->pCurl;
    curl_easy_setopt(hCurl,CURLOPT_URL,newHelper->url);
    curl_easy_setopt(hCurl,CURLOPT_CONNECTTIMEOUT,15);
    curl_easy_setopt(hCurl,CURLOPT_TIMEOUT,5);
    curl_easy_setopt(hCurl,CURLOPT_FOLLOWLOCATION,true);
    curl_easy_setopt(hCurl,CURLOPT_NOSIGNAL,true);
    curl_easy_setopt(hCurl,CURLOPT_NOPROGRESS,true);

    return newHelper;
}

void KDistoryHttpHelper(KHttpHelper_t *helper) {
    assert(helper);
    curl_easy_cleanup(helper->pCurl);
    ObjRelease(helper);
}

int64_t KHttpHelperGetContentLen(KHttpHelper_t *helper) {

    CURLcode reval = CURLE_OK;

    //curl_easy_setopt(hCurl,CURLOPT_HEADER,true);
    //curl_easy_setopt(hCurl,CURL_HEADERFUNCTION,_httpHelperHeaderParser);
    //curl_easy_setopt(hCurl,CURL_HEADERDATA,helper);

    CURL *hCurl = helper->pCurl;
    reval = curl_easy_perform(hCurl);

    long responseCode = 0;
    curl_easy_getinfo(hCurl,CURLINFO_RESPONSE_CODE,&responseCode);
    if (responseCode != 200) {
        return  KE_UNKNOW;
    }

    double contentLen;
    curl_easy_getinfo(hCurl,CURLINFO_CONTENT_LENGTH_DOWNLOAD,&contentLen);

    return (int64_t)contentLen;
}

typedef struct {
    uint8_t *dataBuf;
    uint64_t writenLen;
    uint64_t bufLen;
} DownloadParam;

static size_t _httpHelperDataDownload(char *buf,
                                      size_t size,
                                      size_t nitem,
                                      void *userdata) {
    DownloadParam *param = (DownloadParam *)userdata;
    size_t getLen = size *nitem;
    if (param->writenLen + getLen > param->bufLen) {
        KLogErr("There is too many data to be save.");
        return getLen;
    }

    memcpy(param->dataBuf + param->writenLen,buf,getLen);
    param->writenLen += getLen;

    return getLen;
}

int KHttpHelperDownloadRange(KHttpHelper_t *helper,
                            uint8_t *buf,
                            uint64_t offset,
                            uint64_t *len) {
    CURL *hCurl = helper->pCurl;
    double contentLen;
    CURLcode reval = CURLE_OK;

    reval = curl_easy_getinfo(hCurl,CURLINFO_CONTENT_LENGTH_DOWNLOAD,&contentLen);

    int64_t totalLen = 0;
    if (contentLen > 0.0) {
        totalLen = contentLen;
    } else {
        totalLen = KHttpHelperGetContentLen(helper);
        if (totalLen < 0) {
            return totalLen;
        }
    }

    if ((uint64_t)totalLen < offset) {
        return KE_OUT_OF_RANGE;
    }

    if (offset + *len > (uint64_t)totalLen) {
        *len = totalLen - offset;
    }

    char rangStr[256];
    sprintf(rangStr,"%"PRIu64"-%"PRIu64,offset,offset + *len);
    curl_easy_setopt(hCurl,CURLOPT_HEADER,false);
    curl_easy_setopt(hCurl,CURLOPT_RANGE,rangStr);
    curl_easy_setopt(hCurl,CURLOPT_WRITEFUNCTION,_httpHelperDataDownload);

    DownloadParam param;
    param.dataBuf = buf;
    param.writenLen = 0;
    param.bufLen = *len;
    curl_easy_setopt(hCurl,CURLOPT_WRITEDATA,&param);
    reval = curl_easy_perform(hCurl);

    return KE_OK;
}