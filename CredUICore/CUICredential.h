//
//  CUICredential.h
//  CredUI
//
//  Created by Luke Howard on 28/12/2013.
//  Copyright (c) 2013 PADL Software Pty Ltd. All rights reserved.
//

#ifndef __CredUI__CUICredential__
#define __CredUI__CUICredential__

#ifdef __cplusplus
extern "C" {
#endif
    
typedef struct __CUICredential *CUICredentialRef;

extern CFTypeID
CUICredentialGetTypeID(void);    
    
extern CFArrayRef
CUICredentialGetFields(CUICredentialRef cred);

extern CUIFieldRef
CUICredentialFindFirstFieldWithClass(CUICredentialRef cred, CUIFieldClass fieldClass);
    
extern CFArrayRef
CUICredentialCopyFieldsWithPredicate(CUICredentialRef cred,
                                     Boolean (^predicate)(CUIFieldRef field));

extern CFDictionaryRef
CUICredentialGetAttributes(CUICredentialRef cred);
    
extern void
CUICredentialDidBecomeSelected(CUICredentialRef cred, Boolean *pbAutoLogin);

extern void
CUICredentialDidBecomeDeselected(CUICredentialRef cred);

/*
 * Call this once the user has selected a credential and you want to use it
 */
extern void
CUICredentialWillSubmit(CUICredentialRef cred);

/*
 * Call CUICredentialCanSubmit after CUICredentialWillSubmit to validate
 * the credential has any mandatory attributes.
 */
extern Boolean
CUICredentialCanSubmit(CUICredentialRef cred);

/*
 * Call CUICredentialDidSubmit after the credential has been submitted to
 * notify the provider.
 */
extern void
CUICredentialDidSubmit(CUICredentialRef cred);
    
extern void
CUICredentialFieldsApplyBlock(CUICredentialRef cred, void (^cb)(CUIFieldRef, Boolean *stop));

extern Boolean
CUICredentialConfirm(CUICredentialRef cred, CFErrorRef *error);
    
#ifdef __cplusplus
}
#endif

#endif /* defined(__CredUI__CUICredential__) */
