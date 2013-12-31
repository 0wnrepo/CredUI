//
//  GSSPromptForCredentials.h
//  CredUI
//
//  Created by Luke Howard on 31/12/2013.
//  Copyright (c) 2013 PADL Software Pty Ltd. All rights reserved.
//

#ifndef CredUI_GSSPromptForCredentials_h
#define CredUI_GSSPromptForCredentials_h

#include <GSS/GSS.h>
#include <CredUI/CUIPromptForCredentials.h>

#ifdef __cplusplus
extern "C" {
#endif
    
Boolean
GSSPromptForCredentials(gss_name_t targetName,
                        CFTypeRef gssContextHandle,
                        CUICredUIContext *uiContext,
                        CFErrorRef authError,
                        CFDictionaryRef inCredAttributes,
                        CFDictionaryRef *outCredAttributes,
                        Boolean *pfSave,
                        CUIFlags flags);

Boolean
GSSIsPromptingNeeded(CFErrorRef authError);

#ifdef __cplusplus
}
#endif

#endif
