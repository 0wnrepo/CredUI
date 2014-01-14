//
//  CUICFUtilities.cpp
//  CredUI
//
//  Created by Luke Howard on 14/01/2014.
//  Copyright (c) 2014 PADL Software Pty Ltd. All rights reserved.
//

#include <CoreFoundation/CoreFoundation.h>

#include "CUICFUtilities.h"

char *
CUICFStringToCString(CFStringRef string)
{
    char *s = NULL;
    
    if (string == NULL)
        return NULL;
    
    CFIndex len = CFStringGetLength(string);
    len = 1 + CFStringGetMaximumSizeForEncoding(len, kCFStringEncodingUTF8);
    s = (char *)malloc(len);
    if (s == NULL)
        return NULL;
    
    if (!CFStringGetCString(string, s, len, kCFStringEncodingUTF8)) {
        free(s);
        s = NULL;
    }
    
    return s;
}
