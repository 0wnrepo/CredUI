//
//  CUICredentialLogin.h
//  CredUI
//
//  Created by Luke Howard on 13/01/2014.
//  Copyright (c) 2014 PADL Software Pty Ltd. All rights reserved.
//

#ifndef CredUICore_CUICredentialLogin_h
#define CredUICore_CUICredentialLogin_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Performs a local PAM login for the selected credential user.
 */    
CUI_EXPORT Boolean
CUICredentialAuthenticateForLoginScenario(CUICredentialRef credential);

#ifdef __cplusplus
}
#endif

#endif /* CredUICore_CUICredentialLogin_h */
