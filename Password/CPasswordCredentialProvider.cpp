//
//  PasswordCredentialProvider.cpp
//  CredUI
//
//  Created by Luke Howard on 29/12/2013.
//  Copyright (c) 2013 PADL Software Pty Ltd. All rights reserved.
//

#include "CPasswordCredentialProvider.h"
#include "CPasswordCredential.h"

#include <libkern/OSAtomic.h>

extern "C" {
    void *CPasswordCredentialProviderFactory(CFAllocatorRef allocator, CFUUIDRef typeID);
};

class CPasswordCredentialProvider : public CUIProvider {
    
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
  
    CUICredentialContext *createCredentialWithAttributes(CFDictionaryRef attributes) {
        CPasswordCredential *passwordCred = new CPasswordCredential();
        
        if (!passwordCred->initWithAttributes(attributes)) {
            passwordCred->Release();
            return NULL;
        }
        
        return passwordCred;
    }

    Boolean initWithController(CUIControllerRef controller) {
        _controller = (CUIControllerRef)CFRetain(controller);
        return true;
    }
    
    CFArrayRef createOtherCredentials(void) {
        return NULL;
    }

    CPasswordCredentialProvider() {
        CFPlugInAddInstanceForFactory(kCUIProviderFactoryID);
        _retainCount = 1;
    }

protected:
    
    ~CPasswordCredentialProvider() {
        if (_controller)
            CFRelease(_controller);
        CFPlugInRemoveInstanceForFactory(kCUIProviderFactoryID);
    }
    
private:
    
};

void *
CPasswordCredentialProviderFactory(CFAllocatorRef allocator, CFUUIDRef typeID)
{
    if (CFEqual(typeID, kCUIProviderTypeID))
        return new CPasswordCredentialProvider;
    
    return NULL;
}
