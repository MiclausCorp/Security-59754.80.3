/*
 * Copyright (c) 2019 Apple Inc. All Rights Reserved.
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

import Foundation
import SecurityFoundation

class RecoveryKey: NSObject {
    internal var recoveryKeys: RecoveryKeySet
    internal var secret: Data

    internal var peerKeys: OctagonSelfPeerKeys

    internal init(recoveryKeyString: String, recoverySalt: String) throws {
        self.secret = Data(bytes: Array(recoveryKeyString.utf8), count: recoveryKeyString.utf8.count)
        self.recoveryKeys = try RecoveryKeySet(secret: self.secret, recoverySalt: recoverySalt)

        let peerID = RecoveryKey.PeerID(signingPublicKeyData: self.recoveryKeys.signingKey.publicKey.keyData)

        try self.peerKeys = OctagonSelfPeerKeys(peerID: peerID, signingKey: self.recoveryKeys.signingKey, encryptionKey: self.recoveryKeys.encryptionKey)
    }

    static func PeerID(signingPublicKeyData: Data) -> String {
        let hash = RecoveryKeySet.hashRecoveryedSigningPublicKey(keyData: signingPublicKeyData)
        let peerID = "RK-" + hash

        return peerID
    }

    static func spki(publicKeyData: Data) throws -> Data {
        let key = try _SFECPublicKey(data: publicKeyData, specifier: _SFECKeySpecifier(curve: SFEllipticCurve.nistp384))
        return key.encodeSubjectPublicKeyInfo()
    }

    public static func asPeer(recoveryKeys: TPRecoveryKeyPair, viewList: Set<String>) throws -> TrustedPeersHelperPeer {
        return TrustedPeersHelperPeer(peerID: self.PeerID(signingPublicKeyData: recoveryKeys.signingKeyData),
                                      signingSPKI: try self.spki(publicKeyData: recoveryKeys.signingKeyData),
                                      encryptionSPKI: try self.spki(publicKeyData: recoveryKeys.encryptionKeyData),
                                      viewList: viewList)
    }
}

extension RecoveryKey {
    enum Error: Swift.Error {
        case OTErrorDeserializationFailure
        case OTErrorDecryptionFailure
        case OTErrorKeyInstantiation
        case OTErrorKeyMismatch
        case OTErrorRecoveryCreation
        case OTErrorAuthCipherTextCreation
        case OTErrorPrivateKeyCreation
        case OTErrorRecoveryKeyCreation
        case OTErrorEntropyCreation
        case OTErrorEntropyKeyMismatch
    }
}

extension RecoveryKey.Error: LocalizedError {
    public var errorDescription: String? {
        switch self {
        case .OTErrorDeserializationFailure:
            return "Failed to deserialize Recovery peer"
        case .OTErrorDecryptionFailure:
            return "could not decrypt Recovery contents"
        case .OTErrorKeyInstantiation:
            return "Failed to instantiate octagon peer keys"
        case .OTErrorKeyMismatch:
            return "public and private peer signing keys do not match"
        case .OTErrorRecoveryCreation:
            return "failed to create Recovery"
        case .OTErrorAuthCipherTextCreation:
            return "failed to create authenticated ciphertext"
        case .OTErrorPrivateKeyCreation:
            return "failed to create private key"
        case .OTErrorRecoveryKeyCreation:
            return "failed to create recovery keys"
        case .OTErrorEntropyCreation:
            return "failed to create entropy"
        case .OTErrorEntropyKeyMismatch:
            return "keys generated by the entropy+salt do not match the Recovery contents"
        }
    }
}
