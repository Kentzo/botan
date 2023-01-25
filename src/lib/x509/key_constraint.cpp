/*
* (C) 1999-2007,2016 Jack Lloyd
* (C) 2016 René Korthaus, Rohde & Schwarz Cybersecurity
*
* Botan is released under the Simplified BSD License (see license.txt)
*/

#include <botan/pkix_enums.h>
#include <botan/pk_keys.h>
#include <vector>

namespace Botan {

std::string Key_Constraints::to_string() const
   {
   if(this->m_value == Key_Constraints::NO_CONSTRAINTS)
      return "no_constraints";

   std::vector<std::string> str;

   if(this->m_value & Key_Constraints::DIGITAL_SIGNATURE)
      str.push_back("digital_signature");

   if(this->m_value & Key_Constraints::NON_REPUDIATION)
      str.push_back("non_repudiation");

   if(this->m_value & Key_Constraints::KEY_ENCIPHERMENT)
      str.push_back("key_encipherment");

   if(this->m_value & Key_Constraints::DATA_ENCIPHERMENT)
      str.push_back("data_encipherment");

   if(this->m_value & Key_Constraints::KEY_AGREEMENT)
      str.push_back("key_agreement");

   if(this->m_value & Key_Constraints::KEY_CERT_SIGN)
      str.push_back("key_cert_sign");

   if(this->m_value & Key_Constraints::CRL_SIGN)
      str.push_back("crl_sign");

   if(this->m_value & Key_Constraints::ENCIPHER_ONLY)
      str.push_back("encipher_only");

   if(this->m_value & Key_Constraints::DECIPHER_ONLY)
      str.push_back("decipher_only");

   // Not 0 (checked at start) but nothing matched above!
   if(str.empty())
      return "other_unknown_constraints";

   if(str.size() == 1)
      return str[0];

   std::string out;
   for(size_t i = 0; i < str.size() - 1; ++i)
      {
      out += str[i];
      out += ',';
      }
   out += str[str.size() - 1];

   return out;
   }

/*
* Make sure the given key constraints are permitted for the given key type
*/
bool Key_Constraints::compatible_with(const Public_Key& pub_key) const
   {
   const std::string name = pub_key.algo_name();

   const bool can_agree = (name == "DH" || name == "ECDH" || name.starts_with("Kyber-"));
   const bool can_encrypt = (name == "RSA" || name == "ElGamal" || name.starts_with("Kyber-"));

   const bool can_sign =
      (name == "RSA" || name == "DSA" ||
       name == "ECDSA" || name == "ECGDSA" || name == "ECKCDSA" || name == "Ed25519" ||
       name == "GOST-34.10" || name == "GOST-34.10-2012-256" || name == "GOST-34.10-2012-512" ||
       name.starts_with("Dilithium-"));

   uint32_t permitted = 0;

   if(can_agree)
      {
      permitted |= Key_Constraints::KEY_AGREEMENT |
         Key_Constraints::ENCIPHER_ONLY |
         Key_Constraints::DECIPHER_ONLY;
      }

   if(can_encrypt)
      {
      permitted |= Key_Constraints::KEY_ENCIPHERMENT |
         Key_Constraints::DATA_ENCIPHERMENT;
      }

   if(can_sign)
      {
      permitted |= Key_Constraints::DIGITAL_SIGNATURE |
         Key_Constraints::NON_REPUDIATION |
         Key_Constraints::KEY_CERT_SIGN |
         Key_Constraints::CRL_SIGN;
      }

   if((m_value & permitted) != m_value)
      {
      return false;
      }

   return true;
   }

}
