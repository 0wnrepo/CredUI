//
//  CUIPromptForCredentials.m
//  CredUI
//
//  Created by Luke Howard on 31/12/2013.
//  Copyright (c) 2013 PADL Software Pty Ltd. All rights reserved.
//

#import <CredUI/GSSPromptForCredentials.h>

Boolean
_CUIPromptForCredentials(CFTypeRef targetName,
                         CFTypeRef gssContextHandle,
                         CUICredUIContext *uiContext,
                         CFErrorRef authError,
                         CFDictionaryRef inCredAttributes,
                         CUICredentialRef *outCred,
                         Boolean *pfSave,
                         CUIFlags flags,
                         CFErrorRef *error)
{
    CUIIdentityPicker *identityPicker = [[CUIIdentityPicker alloc] initWithFlags:flags attributes:(__bridge NSDictionary *)inCredAttributes];
    CUICredential *selectedCredential;
    
    if (error != NULL)
        *error = NULL;
    if (identityPicker == nil)
        return false;
    
    identityPicker.targetName = (__bridge id)targetName;
    identityPicker.GSSContextHandle = (__bridge GSSContext *)gssContextHandle;
    identityPicker.credUIContext = uiContext;
    identityPicker.authError = (__bridge NSError *)authError;
    identityPicker.persist = *pfSave;
 
    [identityPicker runModal];
    
    selectedCredential = identityPicker.selectedCredential;
    
    *outCred = (CUICredentialRef)CFBridgingRetain(selectedCredential);
    
    *pfSave = identityPicker.persist;
    if (error)
        *error = (CFErrorRef)CFBridgingRetain(identityPicker.lastError);
    
    return [selectedCredential canSubmit];
}

CUI_EXPORT Boolean
CUIPromptForCredentials(CUICredUIContext *uiContext,
                        CFStringRef targetName,
                        CFTypeRef reserved,
                        CFErrorRef authError,
                        CFStringRef username,
                        CFStringRef password,
                        CUICredentialRef *outCred,
                        Boolean *pfSave,
                        CUIFlags flags,
                        CFErrorRef *error)
{
    NSDictionary *inCredAttributes = @{
                                       (__bridge id)kCUIAttrNameType : (__bridge NSString *)kCUIAttrNameTypeGSSUsername,
                                       (__bridge id)kCUIAttrName : (__bridge NSString *)username,
                                       (__bridge id)kCUIAttrCredentialPassword : (__bridge NSString *)password
                                       };
    
    return _CUIPromptForCredentials(targetName,
                                    reserved,
                                    uiContext,
                                    authError,
                                    (__bridge CFDictionaryRef)inCredAttributes,
                                    outCred,
                                    pfSave,
                                    flags,
                                    error);
}

CUI_EXPORT Boolean
CUIConfirmCredentials(CUICredentialRef credRef,
                      Boolean fSave,
                      CFErrorRef *error)
{
    CUICredential *credential = (__bridge CUICredential *)credRef;
    Boolean ret = true;
    NSError *nsError = nil;

    if (error)
        *error = NULL;
    
    if (fSave) {
        ret = [credential savePersisted:&nsError];
        if (error)
            *error = (CFErrorRef)CFBridgingRetain(nsError);
    }
    
    return ret;
}
