/*
 * Copyright (c) 2017 Apple Inc. All Rights Reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 *
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 *
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 *
 * @APPLE_LICENSE_HEADER_END@
 */

#import <XCTest/XCTest.h>

@interface SecKeyTests : XCTestCase
@end

@implementation SecKeyTests

- (void)testSecKeyAttributesCanBeReadWithMatchingStringsAsKeys {
    CFMutableDictionaryRef keyParameters = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    CFDictionarySetValue(keyParameters, kSecAttrKeyType, kSecAttrKeyTypeECSECPrimeRandom);
    CFDictionarySetValue(keyParameters, kSecAttrKeySizeInBits, (__bridge CFNumberRef)@(384));
    CFDictionarySetValue(keyParameters, CFSTR("nleg"), kCFBooleanTrue);
    SecKeyRef secKey = SecKeyCreateRandomKey(keyParameters, nil);
    NSDictionary* attributes = (__bridge_transfer NSDictionary*)SecKeyCopyAttributes(secKey);
    XCTAssertEqualObjects(attributes[(__bridge NSString*)kSecAttrKeySizeInBits], attributes[@"bsiz"], @"the SecKey attributes dictionary value of 'kSecAttrKeySizeInBits' and 'bsiz' are not the same");
    XCTAssertNotNil(attributes[@"bsiz"], @"the SecKey attributes dictionary value for 'bsiz' is nil");
}

- (void)testECIESDecryptBadInputData {
    NSData *message = [@"message" dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error;
    id privKey = CFBridgingRelease(SecKeyCreateRandomKey((CFDictionaryRef)@{(id)kSecAttrKeyType: (id)kSecAttrKeyTypeECSECPrimeRandom, (id)kSecAttrKeySizeInBits: @256}, (void *)&error));
    XCTAssertNotNil(privKey, @"key generation failed: %@", error);
    id pubKey = CFBridgingRelease(SecKeyCopyPublicKey((SecKeyRef)privKey));
    XCTAssertNotNil(pubKey);
    NSData *ciphertext = CFBridgingRelease(SecKeyCreateEncryptedData((SecKeyRef)pubKey, kSecKeyAlgorithmECIESEncryptionStandardX963SHA256AESGCM, (CFDataRef)message, (void *)&error));
    XCTAssertNotNil(ciphertext, @"Encryption failed: %@", error);
    NSData *plaintext = CFBridgingRelease(SecKeyCreateDecryptedData((SecKeyRef)privKey, kSecKeyAlgorithmECIESEncryptionStandardX963SHA256AESGCM, (CFDataRef)ciphertext, (void *)&error));
    XCTAssertEqualObjects(message, plaintext, @"Decryption did not provide original message");

    // Strip tag from ciphertext
    NSData *strippedCiphertext = [ciphertext subdataWithRange:NSMakeRange(0, ciphertext.length - 16)];
    NSData *failedDecrypted = CFBridgingRelease(SecKeyCreateDecryptedData((SecKeyRef)privKey, kSecKeyAlgorithmECIESEncryptionStandardX963SHA256AESGCM, (CFDataRef)strippedCiphertext, (void *)&error));
    XCTAssertNil(failedDecrypted, @"Decryption of malformed data did not fail");
    XCTAssertEqual(error.code, errSecParam, @"Unexpected error code provided");
}

@end
