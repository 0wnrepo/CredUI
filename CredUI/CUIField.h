//
//  CUIField.h
//  CredUI
//
//  Created by Luke Howard on 30/12/2013.
//  Copyright (c) 2013 PADL Software Pty Ltd. All rights reserved.
//

@interface CUIField : NSObject <NSTextFieldDelegate>
{
    @private
    void *_internal;
}

@property(nonatomic, readonly, assign) CUIFieldClass fieldClass;
@property(nonatomic, readonly, copy) NSString *title;
@property(nonatomic, readonly, copy) NSString *defaultValue;

- initWithClass:(CUIFieldClass)fieldClass
          title:(NSString *)title
   defaultValue:(id)defaultValue
       delegate:(void(^)(CUIFieldRef, CFTypeRef))fieldDidChange;

- (CUIFieldRef)_fieldRef;
- (void)setValue:(id)aValue;

- (void)didSubmit:(id)sender;

@end
