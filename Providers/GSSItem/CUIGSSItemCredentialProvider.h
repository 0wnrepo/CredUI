//
//  GSSItemCredentialProvider.h
//  CredUI
//
//  Created by Luke Howard on 2/01/2014.
//  Copyright (c) 2014 PADL Software Pty Ltd. All rights reserved.
//

#ifndef __CredUI__GSSItemCredentialProvider__
#define __CredUI__GSSItemCredentialProvider__

#include "CUIPersistedCredential.h"

class CUIGSSItemCredentialProvider : public CUIProvider, public CUICredentialPersistenceEx {
    
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
            CFEqual(interfaceID, IUnknownUUID))
            *ppv = (CUIProvider *)this;
        else if (CFEqual(interfaceID, kCUIPersistenceInterfaceID))
            *ppv = (CUICredentialPersistence *)this;
        else
            *ppv = NULL;
        
        if (*ppv != NULL)
            AddRef();
        
        CFRelease(interfaceID);
        
        return *ppv ? S_OK : E_NOINTERFACE;
    }
    
    Boolean initWithController(CUIControllerRef controller, CFErrorRef *error);
    
    CFArrayRef copyMatchingCredentials(CFDictionaryRef attributes,
                                       CUIUsageFlags usageFlags,
                                       CFIndex *defaultCredentialIndex,
                                       CFErrorRef *error);

    CFSetRef getWhitelistedAttributeKeys(void) {
        return NULL;
    }
    
    void addCredentialWithAttributes(CFDictionaryRef attributes, void (^completionHandler)(CFErrorRef));
    void updateCredential(CUICredentialRef credential, void (^completionHandler)(CFErrorRef));
    void deleteCredential(CUICredentialRef credential, void (^completionHandler)(CFErrorRef));
    CFTypeRef extractPassword(CFDictionaryRef attributes, CFErrorRef *error) {
        return NULL;
    }

    static CFMutableDictionaryRef createCUIAttributesFromGSSItemAttributes(CFDictionaryRef attributes);
    static CFMutableDictionaryRef createGSSItemAttributesFromCUIAttributes(CFDictionaryRef attributes);

    CUIGSSItemCredentialProvider() {
        CFPlugInAddInstanceForFactory(kGSSItemCredentialProviderFactoryID);
        _retainCount = 1;
        _controller = NULL;
    }
    
protected:
    
    ~CUIGSSItemCredentialProvider() {
        if (_controller)
            CFRelease(_controller);
        CFPlugInRemoveInstanceForFactory(kGSSItemCredentialProviderFactoryID);
    }
    
};

#endif /* defined(__CredUI__GSSItemCredentialProvider__) */
