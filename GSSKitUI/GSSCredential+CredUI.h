//
//  GSSCredential+CredUI.h
//  CredUI
//
//  Created by Luke Howard on 1/01/2014.
//  Copyright (c) 2014 PADL Software Pty Ltd. All rights reserved.
//

#import <GSSKit/GSSKit.h>

@class CUICredential;

/*
 * This could go in CredUI.framework eventually, but I want to keep the dependencies
 * to a minimum for now.
 */

@interface GSSCredential (CredUI)

- (instancetype)initWithCUICredential:(CUICredential *)cuiCredential error:(NSError **)error;

@end
