//
//  CertificateCredentialProvider.h
//  CredUI
//
//  Created by Luke Howard on 29/12/2013.
//  Copyright (c) 2013 PADL Software Pty Ltd. All rights reserved.
//

#ifndef __CredUI__CertificateCredentialProvider__
#define __CredUI__CertificateCredentialProvider__

#include <Security/Security.h>
#include <libkern/OSAtomic.h>

#include "CredUICore.h"

extern "C" {
    CFStringRef
    _CSCopyAppleIDAccountForAppleIDCertificate(SecCertificateRef, CFErrorRef *);
    
    CFStringRef
    _CSCopyKerberosPrincipalForCertificate(SecCertificateRef);
    
    SecIdentityRef
    _CSCopySecIdentityForAppleID(CFStringRef appleId, CFErrorRef *error);
};

// A9AB1DDF-974F-4883-BAC7-DBF1BE391D65
#define kCertificateCredentialProviderFactoryID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0xA9, 0xAB, 0x1D, 0xDF, 0x97, 0x4F, 0x48, 0x83, 0xBA, 0xC7, 0xDB, 0xF1, 0xBE, 0x39, 0x1D, 0x65)

class CUICertificateCredentialProvider : public CUIProvider {
    
private:
    int32_t _retainCount;
    CUIControllerRef _controller;
    
public:
    ULONG AddRef(void) {
        return OSAtomicIncrement32Barrier(&_retainCount);
    }
    
    ULONG Release(void) {
        int32_t retainCount = OSAtomicDecrement32Barrier(&_retainCount);
        
        if (retainCount <= 0) {
            delete this;
            return 0;
        }
        return retainCount;
    }
    
    HRESULT QueryInterface(REFIID riid, void **ppv) {
        CFUUIDRef interfaceID = CFUUIDCreateFromUUIDBytes(kCFAllocatorDefault, riid);
        
        if (CFEqual(interfaceID, kCUIProviderInterfaceID) ||
            CFEqual(interfaceID, IUnknownUUID)) {
            *ppv = this;
            AddRef();
            CFRelease(interfaceID);
            return S_OK;
        }
        CFRelease(interfaceID);
        return E_NOINTERFACE;
    }
    
    CFArrayRef copyMatchingCredentials(CFDictionaryRef attributes,
                                       CUIUsageFlags usageFlags,
                                       CFIndex *defaultCredentialIndex,
                                       CFErrorRef *error);
    
    CFSetRef getWhitelistedAttributeKeys(void) {
        return NULL;
    }
    
    CFMutableDictionaryRef createCertificateAttributesFromCUIAttributes(CFDictionaryRef attributes, CFTypeRef targetName);
    CFArrayRef copyMatchingIdentities(CFDictionaryRef attributes, CFTypeRef targetName, CFErrorRef *error);
    CUICredentialRef createCredentialWithIdentity(SecIdentityRef identity, CUIUsageFlags usageFlags);
    
    Boolean initWithController(CUIControllerRef controller, CFErrorRef *error) {
        if (CUIControllerGetUsageFlags(controller) & (kCUIUsageFlagsExcludeCertificates | kCUIUsageFlagsConsole)) 
            return false;

        _controller = (CUIControllerRef)CFRetain(controller);

        return true;
    }
    
    CUICertificateCredentialProvider() {
        CFPlugInAddInstanceForFactory(kCertificateCredentialProviderFactoryID);
        _retainCount = 1;
        _controller = NULL;
    }
    
protected:
    
    ~CUICertificateCredentialProvider() {
        if (_controller)
            CFRelease(_controller);
        CFPlugInRemoveInstanceForFactory(kCertificateCredentialProviderFactoryID);
    }
    
private:
    
};

#endif /* defined(__CredUI__CertificateCredentialProvider__) */
