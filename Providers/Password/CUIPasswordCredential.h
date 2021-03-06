//
//  CUIPasswordCredential.h
//  CredUI
//
//  Created by Luke Howard on 29/12/2013.
//  Copyright (c) 2013 PADL Software Pty Ltd. All rights reserved.
//

#ifndef __CredUI__CUIPasswordCredential__
#define __CredUI__CUIPasswordCredential__

#include <CoreFoundation/CoreFoundation.h>
#include <libkern/OSAtomic.h>

#include <CredUICore/CredUICore.h>
#include "CUIProviderUtilities.h"

class CUIPasswordCredential : public CUICredentialContext {
    
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
        CFStringRef desc;
        
        desc = CFStringCreateWithFormat(kCFAllocatorDefault, NULL,
                                        CFSTR("<CUIPasswordCredential %p{name = \"%@\"}>"), this,
                                        CFDictionaryGetValue(_attributes, kCUIAttrName));
        
        return desc;
    }
    
    CFArrayRef getFields(void) {
        return _fields;
    }
    
    CFDictionaryRef getAttributes(void) {
        return _attributes;
    }
    
    Boolean initWithControllerAndAttributes(CUIControllerRef controller,
                                            CUIUsageFlags usageFlags,
                                            CFDictionaryRef attributes,
                                            CFErrorRef *error);
 
    void updateCredentialStatus(void);

    void didBecomeSelected(void) {
    }
    
    void didBecomeDeselected(void) {
    }
    
    void didSubmit(void) {
    }
    
    static Boolean hasPlaceholderPassword(CFDictionaryRef attributes) {
        CFTypeRef password = CFDictionaryGetValue(attributes, kCUIAttrCredentialPassword);
       
        return password && CFEqual(password, kCFBooleanTrue);
    }
 
    Boolean hasPlaceholderPassword(void) {
        return hasPlaceholderPassword(_attributes);
    }

    static Boolean hasNonPlaceholderPassword(CFDictionaryRef attributes) {
        CFTypeRef password = CFDictionaryGetValue(attributes, kCUIAttrCredentialPassword);

        if (password == NULL)
            return false;

        /* we're going to allow passwords to be CFDataRefs for future use */
        return (CFGetTypeID(password) == CFStringGetTypeID() && CFStringGetLength((CFStringRef)password)) ||
               (CFGetTypeID(password) == CFDataGetTypeID()   && CFDataGetLength((CFDataRef)password));
    }

    Boolean hasNonPlaceholderPassword(void) {
        return hasNonPlaceholderPassword(_attributes);
    }

    static Boolean hasPassword(CFDictionaryRef attributes) {
        return hasPlaceholderPassword(attributes) || hasNonPlaceholderPassword(attributes);
    }

    Boolean hasPassword(void) {
        return hasPassword(_attributes);
    }
 
    void savePersisted(void (^completionHandler)(CFErrorRef));
    
    void deletePersisted(void (^completionHandler)(CFErrorRef)) {
        completionHandler(NULL);
    }
   
    Boolean isLoginUsageScenario(void) {
        return CUIControllerGetUsageScenario(_controller) == kCUIUsageScenarioLogin;
    }

    CUIPasswordCredential() {
        _retainCount = 1;
        _fields = NULL;
        _attributes = NULL;
        _controller = NULL;
    }

private:
    int32_t _retainCount;
    CFArrayRef _fields;
    CFMutableDictionaryRef _attributes;
    CUIControllerRef _controller;
    
protected:
    
    ~CUIPasswordCredential() {
        if (_fields)
            CFRelease(_fields);
        if (_attributes)
            CFRelease(_attributes);
        if (_controller)
            CFRelease(_controller);
    }
};

#endif /* defined(__CredUI__CUIPasswordCredential__) */
