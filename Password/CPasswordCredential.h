//
//  CPasswordCredential.h
//  CredUI
//
//  Created by Luke Howard on 29/12/2013.
//  Copyright (c) 2013 PADL Software Pty Ltd. All rights reserved.
//

#ifndef __CredUI__CPasswordCredential__
#define __CredUI__CPasswordCredential__

#include <CoreFoundation/CoreFoundation.h>
#include <libkern/OSAtomic.h>

#include "CredUICore.h"

class CPasswordCredential : public CUICredentialContext {
    
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
        if (CFEqual(interfaceID, kCUICredentialInterfaceID) ||
            CFEqual(interfaceID, IUnknownUUID)) {
            *ppv = this;
            AddRef();
            CFRelease(interfaceID);
            return S_OK;
        }
        CFRelease(interfaceID);
        return E_NOINTERFACE;
    }
    
    CFStringRef copyDescription(void) {
        return NULL;
    }
    
    CFArrayRef getFields(void) {
        return _fields;
    }
    
    CFDictionaryRef getAuthIdentity(void);
    
    Boolean initWithAuthIdentity(CFDictionaryRef authIdentity);
 
    CPasswordCredential() {
        _retainCount = 1;
    }

private:
    int32_t _retainCount;
    CFArrayRef _fields;
    CUIFieldRef _usernameField;
    CUIFieldRef _passwordField;
    CFMutableDictionaryRef _authIdentity;
    
protected:
    
    ~CPasswordCredential() {
        if (_fields)
            CFRelease(_fields);
        if (_usernameField)
            CFRelease(_usernameField);
        if (_passwordField)
            CFRelease(_passwordField);
        if (_authIdentity)
            CFRelease(_authIdentity);
    }
};

#endif /* defined(__CredUI__CPasswordCredential__) */