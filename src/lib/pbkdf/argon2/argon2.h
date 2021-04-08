/**
* (C) 2018,2019 Jack Lloyd
*
* Botan is released under the Simplified BSD License (see license.txt)
*/

#ifndef BOTAN_ARGON2_H_
#define BOTAN_ARGON2_H_

#include <botan/pwdhash.h>
#include <botan/exceptn.h>

#if defined(BOTAN_HAS_ARGON2_FMT)
  #include <botan/argon2fmt.h>
#endif

//BOTAN_FUTURE_INTERNAL_HEADER(argon2.h)

namespace Botan {

class RandomNumberGenerator;

/**
* Argon2 key derivation function
*/
class BOTAN_PUBLIC_API(2,11) Argon2 final : public PasswordHash
   {
   public:
      Argon2(uint8_t family, size_t M, size_t t, size_t p);

      Argon2(const Argon2& other) = default;
      Argon2& operator=(const Argon2&) = default;

      /**
      * Derive a new key under the current Argon2 parameter set
      */
      void derive_key(uint8_t out[], size_t out_len,
                      const char* password, size_t password_len,
                      const uint8_t salt[], size_t salt_len) const override;

      void derive_key(uint8_t out[], size_t out_len,
                      const char* password, size_t password_len,
                      const uint8_t salt[], size_t salt_len,
                      const uint8_t ad[], size_t ad_len,
                      const uint8_t key[], size_t key_len) const override;

      std::string to_string() const override;

      size_t M() const { return m_M; }
      size_t t() const { return m_t; }
      size_t p() const { return m_p; }

      size_t iterations() const override { return t(); }

      size_t parallelism() const override { return p(); }

      size_t memory_param() const override { return M(); }

      size_t total_memory_usage() const override { return M() * 1024; }

   private:

      void argon2(uint8_t output[], size_t output_len,
                  const char* password, size_t password_len,
                  const uint8_t salt[], size_t salt_len,
                  const uint8_t key[], size_t key_len,
                  const uint8_t ad[], size_t ad_len) const;

      uint8_t m_family;
      size_t m_M, m_t, m_p;
   };

class BOTAN_PUBLIC_API(2,11) Argon2_Family final : public PasswordHashFamily
   {
   public:
      Argon2_Family(uint8_t family);

      std::string name() const override;

      std::unique_ptr<PasswordHash> tune(size_t output_length,
                                         std::chrono::milliseconds msec,
                                         size_t max_memory) const override;

      std::unique_ptr<PasswordHash> default_params() const override;

      std::unique_ptr<PasswordHash> from_iterations(size_t iter) const override;

      std::unique_ptr<PasswordHash> from_params(
         size_t M, size_t t, size_t p) const override;
   private:
      const uint8_t m_family;
   };

/**
* Argon2 key derivation function
*
* @param output the output will be placed here
* @param output_len length of output
* @param password the user password
* @param password_len the length of password
* @param salt the salt
* @param salt_len length of salt
* @param key an optional secret key
* @param key_len the length of key
* @param ad an optional additional input
* @param ad_len the length of ad
* @param y the Argon2 variant (0 = Argon2d, 1 = Argon2i, 2 = Argon2id)
* @param p the parallelization parameter
* @param M the amount of memory to use in Kb
* @param t the number of iterations to use
*/
inline void argon2(uint8_t output[], size_t output_len,
                   const char* password, size_t password_len,
                   const uint8_t salt[], size_t salt_len,
                   const uint8_t key[], size_t key_len,
                   const uint8_t ad[], size_t ad_len,
                   uint8_t y, size_t p, size_t M, size_t t)
   {
   std::unique_ptr<PasswordHashFamily> pwdhash_fam;

   if(y == 0)
      pwdhash_fam = PasswordHashFamily::create_or_throw("Argon2d");
   else if(y == 1)
      pwdhash_fam = PasswordHashFamily::create_or_throw("Argon2i");
   else if(y == 2)
      pwdhash_fam = PasswordHashFamily::create_or_throw("Argon2id");
   else
      throw Not_Implemented("Unknown Argon2 family type");

   auto pwdhash = pwdhash_fam->from_params(M, t, p);

   pwdhash->derive_key(output, output_len,
                       password, password_len,
                       salt, salt_len,
                       ad, ad_len,
                       key, key_len);
   }

}

#endif
