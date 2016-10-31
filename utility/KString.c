
#include "KString.h"

#include <sys/types.h>
#include <regex.h>

#include "KTools.h"
#include "KLog.h"

#define KSTR_REBUILDER_MAX_INDEX_LEN (4)
#define KSTR_REBUILDER_ERR_BUF_LEN (256)
#define KSTR_REBUILDER_MAX_MATCH_COUNT (256)

struct KStrRebuilder {
    regex_t reg;
    char *format;
    size_t formatLen;
    char errBuf[KSTR_REBUILDER_ERR_BUF_LEN];
    int errCode;
};

KStrRebuilder_t *CreateKStrRebuilder(const char *pattern,
                                    const char *format) {
    if (pattern == NULL || format == NULL) {
        return NULL;
    }

    KStrRebuilder_t *rebuilder = ObjAlloc(KStrRebuilder_t);
    rebuilder->errCode = regcomp(&rebuilder->reg,pattern,REG_EXTENDED);
    if (rebuilder->errCode != 0) {
        regerror(rebuilder->errCode,
                 &rebuilder->reg,
                 rebuilder->errBuf,
                 sizeof(rebuilder->errBuf));

        KLogErr("RegEx compile Error : %s",rebuilder->errBuf);
        return NULL;
    }

    rebuilder->formatLen = strlen(format);
    rebuilder->format = ArrayAlloc(char,rebuilder->formatLen + 1);
    strncpy(rebuilder->format, format, rebuilder->formatLen);
    rebuilder->format[rebuilder->formatLen] = '\0';

    return rebuilder;
}

void DestoryKStrRebuilder(KStrRebuilder_t *rebuilder) {
    regfree(&rebuilder->reg);
    ArrayRelease(rebuilder->format);
    ObjRelease(rebuilder);
}


int KStrRebuilderBuild(KStrRebuilder_t *rebuilder,
                       const char *origin,
                       char *newStr,
                       size_t newStrLen) {
    uint32_t i;
    regmatch_t matches[KSTR_REBUILDER_MAX_MATCH_COUNT];
    regexec(&rebuilder->reg,origin,KSTR_REBUILDER_MAX_MATCH_COUNT,matches,0);
    size_t destIndex = 0;

    int matchesCount = 0;
    for (i = 0; i != KSTR_REBUILDER_MAX_MATCH_COUNT; i++) {
        if(matches[i].rm_so == -1) break;
        ++matchesCount;
    }

    char num[KSTR_REBUILDER_MAX_INDEX_LEN];
    for (i = 0; i != rebuilder->formatLen;) {

        //Get replacement substring index;
        if (rebuilder->format[i] == '$') {
            ++i;
            size_t numCur = 0;
            // Syntax error ; the length of {n} = 3
            if (i + 3 > rebuilder->formatLen) return KE_SYNTAX_ERROR;
            if (rebuilder->format[i++] != '{') {
                return KE_SYNTAX_ERROR;
            }

            for (; i != rebuilder->formatLen && rebuilder->format[i] != '}'; i++) {
                num[numCur++] = rebuilder->format[i];
            }
            ++i;
            num[numCur] = '\0';

            int groupIndex = atol(num);
            if (groupIndex >= matchesCount) {
                return KE_OUT_OF_RANGE;
            }

            uint32_t subStrLen = matches[groupIndex].rm_eo - matches[groupIndex].rm_so;
            if (destIndex + subStrLen > newStrLen) {
                return KE_OUT_OF_RANGE;
            }

            memcpy(newStr + destIndex,
                   origin + matches[groupIndex].rm_so,
                   subStrLen);
            destIndex += subStrLen;
            continue;
        }

        // Skip special character handling;
        if (rebuilder->format[i] == '\\') {
            ++i;
        }

        if (i < rebuilder->formatLen && i < newStrLen) {
            newStr[destIndex++] = rebuilder->format[i++];
        } else {
            return KE_OUT_OF_RANGE;
        }
    }
    newStr[destIndex] = '\0';
    return KE_OK;
}
