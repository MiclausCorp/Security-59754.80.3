// This file was automatically generated by protocompiler
// DO NOT EDIT!
// Compiled from SecDbKeychainSerializedMetadata.proto

#import "SecDbKeychainSerializedMetadata.h"
#import <ProtocolBuffer/PBConstants.h>
#import <ProtocolBuffer/PBHashUtil.h>
#import <ProtocolBuffer/PBDataReader.h>

#if !__has_feature(objc_arc)
# error This generated file depends on ARC but it is not enabled; turn on ARC, or use 'objc_use_arc' option to generate non-ARC code.
#endif

@implementation SecDbKeychainSerializedMetadata

@synthesize ciphertext = _ciphertext;
@synthesize wrappedKey = _wrappedKey;
@synthesize tamperCheck = _tamperCheck;

- (NSString *)description
{
    return [NSString stringWithFormat:@"%@ %@", [super description], [self dictionaryRepresentation]];
}

- (NSDictionary *)dictionaryRepresentation
{
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    if (self->_ciphertext)
    {
        [dict setObject:self->_ciphertext forKey:@"ciphertext"];
    }
    if (self->_wrappedKey)
    {
        [dict setObject:self->_wrappedKey forKey:@"wrappedKey"];
    }
    if (self->_tamperCheck)
    {
        [dict setObject:self->_tamperCheck forKey:@"tamperCheck"];
    }
    return dict;
}

BOOL SecDbKeychainSerializedMetadataReadFrom(__unsafe_unretained SecDbKeychainSerializedMetadata *self, __unsafe_unretained PBDataReader *reader) {
    while (PBReaderHasMoreData(reader)) {
        uint32_t tag = 0;
        uint8_t aType = 0;

        PBReaderReadTag32AndType(reader, &tag, &aType);

        if (PBReaderHasError(reader))
            break;

        if (aType == TYPE_END_GROUP) {
            break;
        }

        switch (tag) {

            case 1 /* ciphertext */:
            {
                NSData *new_ciphertext = PBReaderReadData(reader);
                self->_ciphertext = new_ciphertext;
            }
            break;
            case 2 /* wrappedKey */:
            {
                NSData *new_wrappedKey = PBReaderReadData(reader);
                self->_wrappedKey = new_wrappedKey;
            }
            break;
            case 3 /* tamperCheck */:
            {
                NSString *new_tamperCheck = PBReaderReadString(reader);
                self->_tamperCheck = new_tamperCheck;
            }
            break;
            default:
                if (!PBReaderSkipValueWithTag(reader, tag, aType))
                    return NO;
                break;
        }
    }
    return !PBReaderHasError(reader);
}

- (BOOL)readFrom:(PBDataReader *)reader
{
    return SecDbKeychainSerializedMetadataReadFrom(self, reader);
}
- (void)writeTo:(PBDataWriter *)writer
{
    /* ciphertext */
    {
        assert(nil != self->_ciphertext);
        PBDataWriterWriteDataField(writer, self->_ciphertext, 1);
    }
    /* wrappedKey */
    {
        assert(nil != self->_wrappedKey);
        PBDataWriterWriteDataField(writer, self->_wrappedKey, 2);
    }
    /* tamperCheck */
    {
        assert(nil != self->_tamperCheck);
        PBDataWriterWriteStringField(writer, self->_tamperCheck, 3);
    }
}

- (void)copyTo:(SecDbKeychainSerializedMetadata *)other
{
    other.ciphertext = _ciphertext;
    other.wrappedKey = _wrappedKey;
    other.tamperCheck = _tamperCheck;
}

- (id)copyWithZone:(NSZone *)zone
{
    SecDbKeychainSerializedMetadata *copy = [[[self class] allocWithZone:zone] init];
    copy->_ciphertext = [_ciphertext copyWithZone:zone];
    copy->_wrappedKey = [_wrappedKey copyWithZone:zone];
    copy->_tamperCheck = [_tamperCheck copyWithZone:zone];
    return copy;
}

- (BOOL)isEqual:(id)object
{
    SecDbKeychainSerializedMetadata *other = (SecDbKeychainSerializedMetadata *)object;
    return [other isMemberOfClass:[self class]]
    &&
    ((!self->_ciphertext && !other->_ciphertext) || [self->_ciphertext isEqual:other->_ciphertext])
    &&
    ((!self->_wrappedKey && !other->_wrappedKey) || [self->_wrappedKey isEqual:other->_wrappedKey])
    &&
    ((!self->_tamperCheck && !other->_tamperCheck) || [self->_tamperCheck isEqual:other->_tamperCheck])
    ;
}

- (NSUInteger)hash
{
    return 0
    ^
    [self->_ciphertext hash]
    ^
    [self->_wrappedKey hash]
    ^
    [self->_tamperCheck hash]
    ;
}

- (void)mergeFrom:(SecDbKeychainSerializedMetadata *)other
{
    if (other->_ciphertext)
    {
        [self setCiphertext:other->_ciphertext];
    }
    if (other->_wrappedKey)
    {
        [self setWrappedKey:other->_wrappedKey];
    }
    if (other->_tamperCheck)
    {
        [self setTamperCheck:other->_tamperCheck];
    }
}

@end

