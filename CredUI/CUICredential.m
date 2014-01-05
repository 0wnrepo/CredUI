//
//  CUICredential.m
//  CredUI
//
//  Created by Luke Howard on 30/12/2013.
//  Copyright (c) 2013 PADL Software Pty Ltd. All rights reserved.
//

#include "CFBridgeHelper.h"
#include "CUIProviderUtilities.h"

extern CFArrayRef
GSSItemCopyMatching(CFDictionaryRef, CFErrorRef *);

struct GSSItem;

extern struct GSSItem *
GSSItemAdd(CFDictionaryRef attributes, CFErrorRef *error);

@interface CUICFCredential : CUICredential
@end

@implementation CUICFCredential

+ (BOOL)automaticallyNotifiesObserversForKey:(NSString *)theKey
{
    return NO;
}

+ (id)allocWithZone:(NSZone *)zone
{
    static CUICFCredential *placeholderCred;
    static dispatch_once_t onceToken;
    
    dispatch_once(&onceToken, ^{
        if (placeholderCred == nil)
            placeholderCred = [super allocWithZone:zone];
    });

    return placeholderCred;
}

CF_CLASSIMPLEMENTATION(CUICFCredential)

- (instancetype)initWithContext:(IUnknownVTbl *)context
{
    CUICredentialRef credentialRef = CUICredentialCreate(kCFAllocatorDefault, context);
    
    self = (id)credentialRef;
    
    return NSMakeCollectable(self);
}

@end

@implementation CUICredential

+ (BOOL)supportsSecureCoding
{
    return YES;
}

- (CUICredentialRef)_credentialRef
{
    return (CUICredentialRef)self;
}

- (CFTypeID)_cfTypeID
{
    return CUICredentialGetTypeID();
}

- (instancetype)init
{
    return [self initWithContext:NULL];
}

- (instancetype)initWithContext:(IUnknownVTbl *)context
{
    return [super init];
}

- (void)encodeWithCoder:(NSCoder *)coder
{
    NSSet *codeableKeys = [self.attributes keysOfEntriesPassingTest:^ BOOL (id key, id obj, BOOL *stop) {
        return [obj respondsToSelector:@selector(encodeWithCoder:)];
    }];
    
    NSDictionary *codeableAttrs =
        [[NSDictionary alloc] initWithObjects:[self.attributes objectsForKeys:codeableKeys.allObjects notFoundMarker:[NSNull null]]
                                      forKeys:codeableKeys.allObjects];
    [coder encodeObject:codeableAttrs];
    
    [codeableAttrs release];

}

- (id)initWithCoder:(NSCoder *)coder
{
    NSDictionary *credAttributes;
    CUICredentialRef credentialRef;
    
    credAttributes = [coder decodeObject];
    if (credAttributes == nil)
        return nil;
        
    credentialRef = CUICredentialCreateProxy(kCFAllocatorDefault, (CFDictionaryRef)credAttributes);
    
    return (id)credentialRef;
}

- (CUIField *)firstFieldWithClass:(CUIFieldClass)fieldClass
{
    return (CUIField *)CUICredentialFindFirstFieldWithClass([self _credentialRef], fieldClass);
}

- (NSDictionary *)attributes
{
    return (NSDictionary *)CUICredentialGetAttributes([self _credentialRef]);
}

- (void)didBecomeSelected:(BOOL *)pbAutoLogin
{
    Boolean bAutoLogin = false;
    
    CUICredentialDidBecomeSelected([self _credentialRef], &bAutoLogin);
    
    *pbAutoLogin = bAutoLogin;
}

- (void)didBecomeDeselected
{
    CUICredentialDidBecomeDeselected([self _credentialRef]);
}

- (BOOL)canSubmit
{
    return CUICredentialCanSubmit([self _credentialRef]);
}

- (void)willSubmit
{
    CUICredentialWillSubmit([self _credentialRef]);
}

- (void)didSubmit
{
    CUICredentialDidSubmit([self _credentialRef]);
}

- (id)GSSName
{
    gss_name_t name = GSS_C_NO_NAME;
    gss_const_OID oid = GSS_C_NO_OID;

    id type = self.attributes[(NSString *)kCUIAttrNameType];
    id value = self.attributes[(NSString *)kCUIAttrName];

    if (type == nil ||
        [type isEqual:(NSString *)kCUIAttrNameTypeGSSUsername])
        oid = GSS_C_NT_USER_NAME;
    else if ([type isEqual:(NSString *)kCUIAttrNameTypeGSSHostBasedService])
        oid = GSS_C_NT_HOSTBASED_SERVICE;
    else if ([type isEqual:(NSString *)kCUIAttrNameTypeGSSExportedName])
        oid = GSS_C_NT_EXPORT_NAME;
    
    if (oid != GSS_C_NO_OID)
        name = GSSCreateName(value, oid, NULL);
    
    return [NSMakeCollectable(name) autorelease];
}

- (NSDictionary *)attributesWithClass:(CUIAttributeClass)attrClass
{
    NSMutableDictionary *transformedDict = [NSMutableDictionary dictionary];

    [self.attributes enumerateKeysAndObjectsUsingBlock:^(id key, id obj, BOOL *stop) {
        NSString *transformedKey;

        if (attrClass == CUIAttributeClassGSSItem) {
            transformedKey = [key stringByReplacingOccurrencesOfString:@"kCUI" withString:@"kGSS"];
        } else if (attrClass == CUIAttributeClassGSSInitialCred) {
            if ([key isEqualToString:(__bridge NSString *)kCUIAttrCredentialSecIdentity])
                transformedKey = (__bridge id)kGSSICCertificate; // special case
            else
                transformedKey = [key stringByReplacingOccurrencesOfString:@"kCUIAttrCredential" withString:@"kGSSIC"];
        } else {
            transformedKey = key;
        }

        transformedDict[transformedKey] = obj;
    }];

    if (attrClass == CUIAttributeClassGSSInitialCred) {
        // we only emit initiator creds
        transformedDict[(NSString *)kGSSCredentialUsage] = (NSString *)kGSS_C_INITIATE;
    }
    
    return transformedDict;
}

- (BOOL)didConfirm:(NSError * __autoreleasing *)error
{
    BOOL ret;
    
    if (error)
        *error = nil;
    
    ret = CUICredentialDidConfirm([self _credentialRef], (CFErrorRef *)error);
    if (error)
        [NSMakeCollectable(*error) autorelease];
    
    return ret;
}

- (NSArray *)fields
{
    return (NSArray *)CUICredentialGetFields([self _credentialRef]);
}

- (id)GSSItem:(BOOL)addIfAbsent error:(NSError * __autoreleasing *)error;
{
    id item;
    
    if (error)
        *error = nil;
    
    item = self.attributes[(__bridge NSString *)kCUIAttrGSSItemRef];
    if (item == nil) {
        NSDictionary *itemAttrs = [self attributesWithClass:CUIAttributeClassGSSItem];
        NSArray *matchingItems = [NSMakeCollectable(GSSItemCopyMatching((CFDictionaryRef)itemAttrs, NULL)) autorelease];
        
        item = matchingItems.count ? matchingItems[0] : nil;
        if (item == nil && addIfAbsent) {
            item = [NSMakeCollectable(GSSItemAdd((CFDictionaryRef)itemAttrs, (CFErrorRef *)error)) autorelease];
        }
    } else {
        item = [[item retain] autorelease]; // in case the credentials dict goes away
    }
    
    if (error)
        [NSMakeCollectable(*error) autorelease];
    
    return item;
}

@end
