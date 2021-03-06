//
//  CUICredential.h
//  CredUI
//
//  Created by Luke Howard on 30/12/2013.
//  Copyright (c) 2013 PADL Software Pty Ltd. All rights reserved.
//

@class NSDictionary;
@class NSError;

__attribute__((visibility("default")))
@interface CUICredential : NSObject <NSCopying>

/*
 * Returns the set of CredUI attributes associated with this credential.
 * Attribute keys are specified in CredUICore/CUIAttributes.h.
 */
- (NSDictionary *)attributes;

/*
 * Acknowledges the application successfully used the credential and
 * it should be persisted. The application should only call this if
 * CUIFlagsExpectConfirmation was set, otherwise CredUI will do it.
 */
- (void)savePersisted:(void (^)(NSError *))replyBlock;

/*
 * If the credential was persisted, deletes it.
 */
- (void)deletePersisted:(void (^)(NSError *))replyBlock;

@end
