//
//  main.c
//  CredUIEnumerate
//
//  Created by Luke Howard on 29/12/2013.
//  Copyright (c) 2013 PADL Software Pty Ltd. All rights reserved.
//

#include <stdio.h>
#include <readpassphrase.h>

#include <Foundation/Foundation.h>
#include <CredUICore/CredUICore.h>

#include "GSSItem.h"

static NSString *readFromConsole(NSString *prompt, NSString *defaultValue, BOOL echo)
{
    char buf[BUFSIZ], *s;
    NSString *p;
    
    if (defaultValue && echo)
        p = [NSString stringWithFormat:@"%@(%@): ", prompt, defaultValue];
    else
        p = [NSString stringWithFormat:@"%@: ", prompt];
    
    s = readpassphrase([p UTF8String], buf, sizeof(buf), echo ? RPP_ECHO_ON : RPP_ECHO_OFF);

    if (s == NULL)
        return nil;
    
    return [NSString stringWithUTF8String:s];
}

int main(int argc, const char * argv[])
{
    CUIControllerRef controller;
    NSDictionary *attributes = @{
                                 (__bridge id)kGSSAttrClass: (__bridge id)kGSSAttrClassKerberos,
                                 (__bridge id)kGSSAttrStatusTransient: @YES,
                                 (__bridge id)kGSSAttrStatusPersistant: @NO,
                                 (__bridge id)kGSSAttrNameType: (__bridge id)kGSSAttrNameTypeGSSUsername,
                                 (__bridge id)kGSSAttrName: @"lhoward@ATHENA.MIT.EDU"
                                };
    controller = CUIControllerCreate(kCFAllocatorDefault, kCUIUsageScenarioNetwork, kCUIUsageFlagsNoUI);
    if (controller == NULL) {
        NSLog(@"failed to create controller");
        exit(1);
    }
    
    CUIControllerSetAttributes(controller, (__bridge CFDictionaryRef)attributes);
    
    NSLog(@"Controller is: %@", (__bridge id)controller);
    NSLog(@"Attributes are: %@", attributes);
    
    NSMutableArray *creds = [NSMutableArray array];
    
    CUIControllerEnumerateCredentials(controller, ^(CUICredentialRef cred) {
        [creds addObject:(__bridge id)cred];
    });

    [creds enumerateObjectsUsingBlock:^(id cred, NSUInteger idx, BOOL *stop) {
        CUIFieldRef field = CUICredentialFindFirstFieldWithClass((__bridge CUICredentialRef)cred, kCUIFieldClassLargeText);
        CUIFieldRef field2 = CUICredentialFindFirstFieldWithClass((__bridge CUICredentialRef)cred, kCUIFieldClassEditText);
        
        if (field) {
            NSLog(@"[%lu] %@(%@)", (unsigned long)idx, (__bridge NSString *)CUIFieldGetTitle(field), (__bridge NSString *)CUIFieldGetDefaultValue(field2));
        }
        
        NSLog(@"Attrs: %@", CUICredentialGetAttributes((__bridge CUICredentialRef)cred));
    }];
    
    CUICredentialRef cred = NULL;
    
    if ([creds count] > 1) {
        NSString *which = readFromConsole(@"Select credential", nil, true);
        cred = (__bridge CUICredentialRef)[creds objectAtIndex:[which integerValue]];
    } else if ([creds count] == 1) {
        cred = (__bridge CUICredentialRef)[creds objectAtIndex:0];
    } else {
        NSLog(@"No credentials");
        exit(1);
    }
    
    Boolean stop = false;
    
    CUICredentialDidBecomeSelected(cred);
    
    CUICredentialFieldsApplyBlock(cred, ^(CUIFieldRef field, Boolean *stop) {
        NSString *title = (__bridge NSString *)CUIFieldGetTitle(field);
        NSString *value = nil;
        NSString *defaultValue = (__bridge NSString *)CUIFieldGetDefaultValue(field);

        switch (CUIFieldGetClass(field)) {
            case kCUIFieldClassLargeText:
                break;
            case kCUIFieldClassSmallText:
                printf("%s\n", [title UTF8String]);
                break;
            case kCUIFieldClassEditText:
                value = readFromConsole(title, defaultValue, true);
                break;
            case kCUIFieldClassPasswordText:
                value = readFromConsole(title, defaultValue, false);
                break;
            default:
                NSLog(@"Ignoring field %@", (__bridge id)field);
                break;
        }
        
        if (value && [value length])
            CUIFieldSetValue(field, (__bridge CFTypeRef)value);
    }, &stop);

    CFDictionaryRef credAttributes = CUICredentialGetAttributes(cred);
    
    NSLog(@"Credential attributes: %@", credAttributes);
    
    CFErrorRef error = NULL;
    GSSItemRef item = CUICredentialCreateGSSItem(cred, true, &error);
    
    if (item) {
        dispatch_queue_t q = dispatch_queue_create("com.padl.CredUIEnumerate", NULL);
        dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);

        GSSItemOperation(item, kGSSOperationAcquire, credAttributes, q, ^(CFTypeRef result, CFErrorRef error) {
            if (error)
                NSLog(@"Acquiring credential error: %@", error);
            else
                NSLog(@"Acquired credential %@", result);
            dispatch_semaphore_signal(semaphore);
        });
        dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
        CFRelease(item);
    } else if (error) {
        NSLog(@"Failed to add item for attributes: %@", error);
    } else {
        NSLog(@"Failed to add item");
    }
    
    CFRelease(controller);
    
    exit(0);
}