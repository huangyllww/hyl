/*
 * Generated by util/mkerr.pl DO NOT EDIT
 * Copyright 1995-2017 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stdio.h>
#include <openssl/err.h>
#include <openssl/evp.h>

/* BEGIN ERROR CODES */
#ifndef OPENSSL_NO_ERR

# define ERR_FUNC(func) ERR_PACK(ERR_LIB_EVP,func,0)
# define ERR_REASON(reason) ERR_PACK(ERR_LIB_EVP,0,reason)

static ERR_STRING_DATA EVP_str_functs[] = {
    {ERR_FUNC(EVP_F_AESNI_INIT_KEY), "aesni_init_key"},
    {ERR_FUNC(EVP_F_AES_INIT_KEY), "aes_init_key"},
    {ERR_FUNC(EVP_F_AES_OCB_CIPHER), "aes_ocb_cipher"},
    {ERR_FUNC(EVP_F_AES_T4_INIT_KEY), "aes_t4_init_key"},
    {ERR_FUNC(EVP_F_AES_WRAP_CIPHER), "aes_wrap_cipher"},
    {ERR_FUNC(EVP_F_ALG_MODULE_INIT), "alg_module_init"},
    {ERR_FUNC(EVP_F_CAMELLIA_INIT_KEY), "camellia_init_key"},
    {ERR_FUNC(EVP_F_CHACHA20_POLY1305_CTRL), "chacha20_poly1305_ctrl"},
    {ERR_FUNC(EVP_F_CMLL_T4_INIT_KEY), "cmll_t4_init_key"},
    {ERR_FUNC(EVP_F_DES_EDE3_WRAP_CIPHER), "des_ede3_wrap_cipher"},
    {ERR_FUNC(EVP_F_DO_SIGVER_INIT), "do_sigver_init"},
    {ERR_FUNC(EVP_F_EVP_CIPHERINIT_EX), "EVP_CipherInit_ex"},
    {ERR_FUNC(EVP_F_EVP_CIPHER_CTX_COPY), "EVP_CIPHER_CTX_copy"},
    {ERR_FUNC(EVP_F_EVP_CIPHER_CTX_CTRL), "EVP_CIPHER_CTX_ctrl"},
    {ERR_FUNC(EVP_F_EVP_CIPHER_CTX_SET_KEY_LENGTH),
     "EVP_CIPHER_CTX_set_key_length"},
    {ERR_FUNC(EVP_F_EVP_DECRYPTFINAL_EX), "EVP_DecryptFinal_ex"},
    {ERR_FUNC(EVP_F_EVP_DECRYPTUPDATE), "EVP_DecryptUpdate"},
    {ERR_FUNC(EVP_F_EVP_DIGESTINIT_EX), "EVP_DigestInit_ex"},
    {ERR_FUNC(EVP_F_EVP_ENCRYPTFINAL_EX), "EVP_EncryptFinal_ex"},
    {ERR_FUNC(EVP_F_EVP_ENCRYPTUPDATE), "EVP_EncryptUpdate"},
    {ERR_FUNC(EVP_F_EVP_MD_CTX_COPY_EX), "EVP_MD_CTX_copy_ex"},
    {ERR_FUNC(EVP_F_EVP_MD_SIZE), "EVP_MD_size"},
    {ERR_FUNC(EVP_F_EVP_OPENINIT), "EVP_OpenInit"},
    {ERR_FUNC(EVP_F_EVP_PBE_ALG_ADD), "EVP_PBE_alg_add"},
    {ERR_FUNC(EVP_F_EVP_PBE_ALG_ADD_TYPE), "EVP_PBE_alg_add_type"},
    {ERR_FUNC(EVP_F_EVP_PBE_CIPHERINIT), "EVP_PBE_CipherInit"},
    {ERR_FUNC(EVP_F_EVP_PBE_SCRYPT), "EVP_PBE_scrypt"},
    {ERR_FUNC(EVP_F_EVP_PKCS82PKEY), "EVP_PKCS82PKEY"},
    {ERR_FUNC(EVP_F_EVP_PKEY2PKCS8), "EVP_PKEY2PKCS8"},
    {ERR_FUNC(EVP_F_EVP_PKEY_ASN1_ADD0), "EVP_PKEY_asn1_add0"},
    {ERR_FUNC(EVP_F_EVP_PKEY_COPY_PARAMETERS), "EVP_PKEY_copy_parameters"},
    {ERR_FUNC(EVP_F_EVP_PKEY_CTX_CTRL), "EVP_PKEY_CTX_ctrl"},
    {ERR_FUNC(EVP_F_EVP_PKEY_CTX_CTRL_STR), "EVP_PKEY_CTX_ctrl_str"},
    {ERR_FUNC(EVP_F_EVP_PKEY_CTX_DUP), "EVP_PKEY_CTX_dup"},
    {ERR_FUNC(EVP_F_EVP_PKEY_DECRYPT), "EVP_PKEY_decrypt"},
    {ERR_FUNC(EVP_F_EVP_PKEY_DECRYPT_INIT), "EVP_PKEY_decrypt_init"},
    {ERR_FUNC(EVP_F_EVP_PKEY_DECRYPT_OLD), "EVP_PKEY_decrypt_old"},
    {ERR_FUNC(EVP_F_EVP_PKEY_DERIVE), "EVP_PKEY_derive"},
    {ERR_FUNC(EVP_F_EVP_PKEY_DERIVE_INIT), "EVP_PKEY_derive_init"},
    {ERR_FUNC(EVP_F_EVP_PKEY_DERIVE_SET_PEER), "EVP_PKEY_derive_set_peer"},
    {ERR_FUNC(EVP_F_EVP_PKEY_ENCRYPT), "EVP_PKEY_encrypt"},
    {ERR_FUNC(EVP_F_EVP_PKEY_ENCRYPT_INIT), "EVP_PKEY_encrypt_init"},
    {ERR_FUNC(EVP_F_EVP_PKEY_ENCRYPT_OLD), "EVP_PKEY_encrypt_old"},
    {ERR_FUNC(EVP_F_EVP_PKEY_GET0_DH), "EVP_PKEY_get0_DH"},
    {ERR_FUNC(EVP_F_EVP_PKEY_GET0_DSA), "EVP_PKEY_get0_DSA"},
    {ERR_FUNC(EVP_F_EVP_PKEY_GET0_EC_KEY), "EVP_PKEY_get0_EC_KEY"},
    {ERR_FUNC(EVP_F_EVP_PKEY_GET0_HMAC), "EVP_PKEY_get0_hmac"},
    {ERR_FUNC(EVP_F_EVP_PKEY_GET0_RSA), "EVP_PKEY_get0_RSA"},
    {ERR_FUNC(EVP_F_EVP_PKEY_KEYGEN), "EVP_PKEY_keygen"},
    {ERR_FUNC(EVP_F_EVP_PKEY_KEYGEN_INIT), "EVP_PKEY_keygen_init"},
    {ERR_FUNC(EVP_F_EVP_PKEY_NEW), "EVP_PKEY_new"},
    {ERR_FUNC(EVP_F_EVP_PKEY_PARAMGEN), "EVP_PKEY_paramgen"},
    {ERR_FUNC(EVP_F_EVP_PKEY_PARAMGEN_INIT), "EVP_PKEY_paramgen_init"},
    {ERR_FUNC(EVP_F_EVP_PKEY_SET1_ENGINE), "EVP_PKEY_set1_engine"},
    {ERR_FUNC(EVP_F_EVP_PKEY_SIGN), "EVP_PKEY_sign"},
    {ERR_FUNC(EVP_F_EVP_PKEY_SIGN_INIT), "EVP_PKEY_sign_init"},
    {ERR_FUNC(EVP_F_EVP_PKEY_VERIFY), "EVP_PKEY_verify"},
    {ERR_FUNC(EVP_F_EVP_PKEY_VERIFY_INIT), "EVP_PKEY_verify_init"},
    {ERR_FUNC(EVP_F_EVP_PKEY_VERIFY_RECOVER), "EVP_PKEY_verify_recover"},
    {ERR_FUNC(EVP_F_EVP_PKEY_VERIFY_RECOVER_INIT),
     "EVP_PKEY_verify_recover_init"},
    {ERR_FUNC(EVP_F_EVP_SIGNFINAL), "EVP_SignFinal"},
    {ERR_FUNC(EVP_F_EVP_VERIFYFINAL), "EVP_VerifyFinal"},
    {ERR_FUNC(EVP_F_INT_CTX_NEW), "int_ctx_new"},
    {ERR_FUNC(EVP_F_PKCS5_PBE_KEYIVGEN), "PKCS5_PBE_keyivgen"},
    {ERR_FUNC(EVP_F_PKCS5_V2_PBE_KEYIVGEN), "PKCS5_v2_PBE_keyivgen"},
    {ERR_FUNC(EVP_F_PKCS5_V2_PBKDF2_KEYIVGEN), "PKCS5_v2_PBKDF2_keyivgen"},
    {ERR_FUNC(EVP_F_PKCS5_V2_SCRYPT_KEYIVGEN), "PKCS5_v2_scrypt_keyivgen"},
    {ERR_FUNC(EVP_F_PKEY_SET_TYPE), "pkey_set_type"},
    {ERR_FUNC(EVP_F_RC2_MAGIC_TO_METH), "rc2_magic_to_meth"},
    {ERR_FUNC(EVP_F_RC5_CTRL), "rc5_ctrl"},
    {0, NULL}
};

static ERR_STRING_DATA EVP_str_reasons[] = {
    {ERR_REASON(EVP_R_AES_KEY_SETUP_FAILED), "aes key setup failed"},
    {ERR_REASON(EVP_R_BAD_DECRYPT), "bad decrypt"},
    {ERR_REASON(EVP_R_BUFFER_TOO_SMALL), "buffer too small"},
    {ERR_REASON(EVP_R_CAMELLIA_KEY_SETUP_FAILED),
     "camellia key setup failed"},
    {ERR_REASON(EVP_R_CIPHER_PARAMETER_ERROR), "cipher parameter error"},
    {ERR_REASON(EVP_R_COMMAND_NOT_SUPPORTED), "command not supported"},
    {ERR_REASON(EVP_R_COPY_ERROR), "copy error"},
    {ERR_REASON(EVP_R_CTRL_NOT_IMPLEMENTED), "ctrl not implemented"},
    {ERR_REASON(EVP_R_CTRL_OPERATION_NOT_IMPLEMENTED),
     "ctrl operation not implemented"},
    {ERR_REASON(EVP_R_DATA_NOT_MULTIPLE_OF_BLOCK_LENGTH),
     "data not multiple of block length"},
    {ERR_REASON(EVP_R_DECODE_ERROR), "decode error"},
    {ERR_REASON(EVP_R_DIFFERENT_KEY_TYPES), "different key types"},
    {ERR_REASON(EVP_R_DIFFERENT_PARAMETERS), "different parameters"},
    {ERR_REASON(EVP_R_ERROR_LOADING_SECTION), "error loading section"},
    {ERR_REASON(EVP_R_ERROR_SETTING_FIPS_MODE), "error setting fips mode"},
    {ERR_REASON(EVP_R_EXPECTING_AN_HMAC_KEY), "expecting an hmac key"},
    {ERR_REASON(EVP_R_EXPECTING_AN_RSA_KEY), "expecting an rsa key"},
    {ERR_REASON(EVP_R_EXPECTING_A_DH_KEY), "expecting a dh key"},
    {ERR_REASON(EVP_R_EXPECTING_A_DSA_KEY), "expecting a dsa key"},
    {ERR_REASON(EVP_R_EXPECTING_A_EC_KEY), "expecting a ec key"},
    {ERR_REASON(EVP_R_FIPS_MODE_NOT_SUPPORTED), "fips mode not supported"},
    {ERR_REASON(EVP_R_ILLEGAL_SCRYPT_PARAMETERS),
     "illegal scrypt parameters"},
    {ERR_REASON(EVP_R_INITIALIZATION_ERROR), "initialization error"},
    {ERR_REASON(EVP_R_INPUT_NOT_INITIALIZED), "input not initialized"},
    {ERR_REASON(EVP_R_INVALID_DIGEST), "invalid digest"},
    {ERR_REASON(EVP_R_INVALID_FIPS_MODE), "invalid fips mode"},
    {ERR_REASON(EVP_R_INVALID_KEY), "invalid key"},
    {ERR_REASON(EVP_R_INVALID_KEY_LENGTH), "invalid key length"},
    {ERR_REASON(EVP_R_INVALID_OPERATION), "invalid operation"},
    {ERR_REASON(EVP_R_KEYGEN_FAILURE), "keygen failure"},
    {ERR_REASON(EVP_R_MEMORY_LIMIT_EXCEEDED), "memory limit exceeded"},
    {ERR_REASON(EVP_R_MESSAGE_DIGEST_IS_NULL), "message digest is null"},
    {ERR_REASON(EVP_R_METHOD_NOT_SUPPORTED), "method not supported"},
    {ERR_REASON(EVP_R_MISSING_PARAMETERS), "missing parameters"},
    {ERR_REASON(EVP_R_NO_CIPHER_SET), "no cipher set"},
    {ERR_REASON(EVP_R_NO_DEFAULT_DIGEST), "no default digest"},
    {ERR_REASON(EVP_R_NO_DIGEST_SET), "no digest set"},
    {ERR_REASON(EVP_R_NO_KEY_SET), "no key set"},
    {ERR_REASON(EVP_R_NO_OPERATION_SET), "no operation set"},
    {ERR_REASON(EVP_R_OPERATION_NOT_SUPPORTED_FOR_THIS_KEYTYPE),
     "operation not supported for this keytype"},
    {ERR_REASON(EVP_R_OPERATON_NOT_INITIALIZED), "operaton not initialized"},
    {ERR_REASON(EVP_R_PARTIALLY_OVERLAPPING),
     "partially overlapping buffers"},
    {ERR_REASON(EVP_R_PKEY_ASN1_METHOD_ALREADY_REGISTERED),
     "pkey asn1 method already registered"},
    {ERR_REASON(EVP_R_PRIVATE_KEY_DECODE_ERROR), "private key decode error"},
    {ERR_REASON(EVP_R_PRIVATE_KEY_ENCODE_ERROR), "private key encode error"},
    {ERR_REASON(EVP_R_PUBLIC_KEY_NOT_RSA), "public key not rsa"},
    {ERR_REASON(EVP_R_UNKNOWN_CIPHER), "unknown cipher"},
    {ERR_REASON(EVP_R_UNKNOWN_DIGEST), "unknown digest"},
    {ERR_REASON(EVP_R_UNKNOWN_OPTION), "unknown option"},
    {ERR_REASON(EVP_R_UNKNOWN_PBE_ALGORITHM), "unknown pbe algorithm"},
    {ERR_REASON(EVP_R_UNSUPPORTED_ALGORITHM), "unsupported algorithm"},
    {ERR_REASON(EVP_R_UNSUPPORTED_CIPHER), "unsupported cipher"},
    {ERR_REASON(EVP_R_UNSUPPORTED_KEYLENGTH), "unsupported keylength"},
    {ERR_REASON(EVP_R_UNSUPPORTED_KEY_DERIVATION_FUNCTION),
     "unsupported key derivation function"},
    {ERR_REASON(EVP_R_UNSUPPORTED_KEY_SIZE), "unsupported key size"},
    {ERR_REASON(EVP_R_UNSUPPORTED_NUMBER_OF_ROUNDS),
     "unsupported number of rounds"},
    {ERR_REASON(EVP_R_UNSUPPORTED_PRF), "unsupported prf"},
    {ERR_REASON(EVP_R_UNSUPPORTED_PRIVATE_KEY_ALGORITHM),
     "unsupported private key algorithm"},
    {ERR_REASON(EVP_R_UNSUPPORTED_SALT_TYPE), "unsupported salt type"},
    {ERR_REASON(EVP_R_WRAP_MODE_NOT_ALLOWED), "wrap mode not allowed"},
    {ERR_REASON(EVP_R_WRONG_FINAL_BLOCK_LENGTH), "wrong final block length"},
    {0, NULL}
};

#endif

int ERR_load_EVP_strings(void)
{
#ifndef OPENSSL_NO_ERR

    if (ERR_func_error_string(EVP_str_functs[0].error) == NULL) {
        ERR_load_strings(0, EVP_str_functs);
        ERR_load_strings(0, EVP_str_reasons);
    }
#endif
    return 1;
}
