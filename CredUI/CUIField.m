//
//  CUIField.m
//  CredUI
//
//  Created by Luke Howard on 30/12/2013.
//  Copyright (c) 2013 PADL Software Pty Ltd. All rights reserved.
//

@interface CUICFField : CUIField
@end

@implementation CUICFField

+ (void)load
{
    _CFRuntimeBridgeClasses(CUIFieldGetTypeID(), "CUICFField");
}

CF_CLASSIMPLEMENTATION(CUICFField)

@end

@implementation CUIField

+ (id)allocWithZone:(NSZone *)zone
{
    static CUIField *placeholderField;
    static dispatch_once_t onceToken;
    
    if ([self class] == [CUICFField class]) {
        dispatch_once(&onceToken, ^{
            if (placeholderField == nil)
                placeholderField = [super allocWithZone:zone];
        });
        return placeholderField;
	} else {
        return [super allocWithZone:zone];
	}
}

- (CUIFieldRef)_fieldRef
{
    if ([self class] == [CUICFField class])
        return (CUIFieldRef)self;
    else
        return _internal;
}

- (NSString *)description
{
    return [NSMakeCollectable(CFCopyDescription([self _fieldRef])) autorelease];
}

- (CFTypeID)_cfTypeID
{
    return CUIFieldGetTypeID();
}

- initWithClass:(CUIFieldClass)fieldClass
          title:(NSString *)title
   defaultValue:(id)defaultValue
       delegate:(void(^)(CUIFieldRef, CFTypeRef))fieldDidChange
{
    CUIFieldRef fieldRef;
    NSString *titleCopy = [title copy];
    id defaultValueCopy = [defaultValue copy];

    fieldRef = CUIFieldCreate(kCFAllocatorDefault,
                              fieldClass,
                              (CFStringRef)titleCopy,
                              (CFTypeRef)defaultValueCopy,
                              fieldDidChange);
    
    [titleCopy release];
    [defaultValueCopy release];

    if ([self class] == [CUICFField class]) {
        self = (id)fieldRef;
    } else {
        self = [super init];
        _internal = fieldRef;
    }
    
    return NSMakeCollectable(self);
}

- init
{
    return [self initWithClass:kCUIFieldClassInvalid
                         title:nil
                  defaultValue:nil
                      delegate:nil];
}

- (void)dealloc
{
    if ([self class] != [CUICFField class])
        CFRelease(_internal);
    
    [super dealloc];
}

- (CUIFieldClass)fieldClass
{
    return CUIFieldGetClass([self _fieldRef]);
}

- (NSString *)title
{
    return (NSString *)CUIFieldGetTitle([self _fieldRef]);
}

- (id)defaultValue
{
    return (id)CUIFieldGetDefaultValue([self _fieldRef]);
}

- (void)setValue:(id)aValue
{
    id aValueCopy = [aValue copy];
    
    CUIFieldSetValue([self _fieldRef], (CFTypeRef)aValueCopy);
    
    [aValueCopy release];
}

- (void)didSubmit:(id)sender
{
    [self setValue:(id)kCFBooleanTrue];
}

- (void)controlTextDidChange:(NSNotification *)notification
{
    NSTextView *textView = notification.userInfo[@"NSFieldEditor"];
    [self setValue:textView.string];
}

@end