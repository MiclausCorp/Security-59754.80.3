// This file was automatically generated by protocompiler
// DO NOT EDIT!
// Compiled from OTPrivateKey.proto

#import <Foundation/Foundation.h>
#import <ProtocolBuffer/PBCodable.h>

typedef NS_ENUM(int32_t, OTPrivateKey_KeyType) {
    /** kSecAttrKeyTypeEC */
    OTPrivateKey_KeyType_EC_NIST_CURVES = 1,
};
#ifdef __OBJC__
NS_INLINE NSString *OTPrivateKey_KeyTypeAsString(OTPrivateKey_KeyType value)
{
    switch (value)
    {
        case OTPrivateKey_KeyType_EC_NIST_CURVES: return @"EC_NIST_CURVES";
        default: return [NSString stringWithFormat:@"(unknown: %i)", value];
    }
}
#endif /* __OBJC__ */
#ifdef __OBJC__
NS_INLINE OTPrivateKey_KeyType StringAsOTPrivateKey_KeyType(NSString *value)
{
    if ([value isEqualToString:@"EC_NIST_CURVES"]) return OTPrivateKey_KeyType_EC_NIST_CURVES;
    return OTPrivateKey_KeyType_EC_NIST_CURVES;
}
#endif /* __OBJC__ */

#ifdef __cplusplus
#define OTPRIVATEKEY_FUNCTION extern "C" __attribute__((visibility("hidden")))
#else
#define OTPRIVATEKEY_FUNCTION extern __attribute__((visibility("hidden")))
#endif

__attribute__((visibility("hidden")))
@interface OTPrivateKey : PBCodable <NSCopying>
{
    NSData *_keyData;
    OTPrivateKey_KeyType _keyType;
}


@property (nonatomic) OTPrivateKey_KeyType keyType;
- (NSString *)keyTypeAsString:(OTPrivateKey_KeyType)value;
- (OTPrivateKey_KeyType)StringAsKeyType:(NSString *)str;

@property (nonatomic, retain) NSData *keyData;

// Performs a shallow copy into other
- (void)copyTo:(OTPrivateKey *)other;

// Performs a deep merge from other into self
// If set in other, singular values in self are replaced in self
// Singular composite values are recursively merged
// Repeated values from other are appended to repeated values in self
- (void)mergeFrom:(OTPrivateKey *)other;

OTPRIVATEKEY_FUNCTION BOOL OTPrivateKeyReadFrom(__unsafe_unretained OTPrivateKey *self, __unsafe_unretained PBDataReader *reader);

@end

