// Stub implementations for CryptoPP utility functions
// These functions are referenced by CryptoPP's exception-handling code
// but are not used in the SHA256 hashing required by Transaction.
// Providing empty implementations allows the build to succeed.

#include <string>
#include <typeinfo>

namespace CryptoPP {
    // Stub for AssignIntToInteger - used in parameter validation
    bool AssignIntToInteger(const std::type_info&, void*, const void*)
    {
        return false;
    }

    // Stub for IntToString template - used in error message formatting
    template<typename T>
    std::string IntToString(T, unsigned int)
    {
        return std::string();
    }

    // Explicit instantiation for unsigned long long (used by CryptoPP)
    template std::string IntToString<unsigned long long>(unsigned long long, unsigned int);
}
