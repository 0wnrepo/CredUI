//
//  CUIIdentityPickerInternalWindowController.h
//  CredUI
//
//  Created by Luke Howard on 1/01/2014.
//  Copyright (c) 2014 PADL Software Pty Ltd. All rights reserved.
//

@interface CUIIdentityPickerInternal : NSWindowController  <NSWindowDelegate>

@property(nonatomic, copy) NSString *title;
@property(nonatomic, copy) NSString *message;
@property(nonatomic, copy) NSDictionary *attributes;
@property(nonatomic, copy) NSError *authError;
@property(nonatomic, assign) BOOL persist;
@property(nonatomic, assign) const void *context;
@property(nonatomic, copy) id targetName;

@property(nonatomic, readonly) CUIFlags flags;
@property(nonatomic, readonly) CUIUsageScenario usageScenario;
@property(nonatomic, readonly) CUIUsageFlags usageFlags;

@property(nonatomic, retain, readonly) CUICredential *selectedCredential;
@property(nonatomic, copy) NSError *lastError;

@property(nonatomic, retain, readonly) NSString *targetDisplayName;

@property(nonatomic, retain) NSPanel *identityPickerPanel;
@property(nonatomic, retain) NSCollectionView *collectionView;
@property(nonatomic, retain) NSTextField *messageTextField;
@property(nonatomic, retain) NSButton *persistCheckBox;
@property(nonatomic, retain) NSButton *submitButton;

- (instancetype)initWithFlags:(CUIFlags)flags
                usageScenario:(CUIUsageScenario)usageScenario
                   attributes:(NSDictionary *)attributes;

- (void)credentialFieldDidChange:(CUICredential *)cred;

- (void)didClickPersist:(id)sender;
- (void)willSubmitCredential:(id)sender;
- (void)didSubmitCredential;

- (void)setCredUIContext:(CUICredUIContext *)uic properties:(CUICredUIContextProperties)props;

- (NSInteger)runModal;
- (void)runModalForWindow:(NSWindow *)window modalDelegate:(id)delegate didEndSelector:(SEL)didEndSelector contextInfo:(void *)contextInfo;

@end
