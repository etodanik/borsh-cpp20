<p align="center">
  <img src="https://github.com/israelidanny/borsh-cpp20/assets/1970424/ff975fe3-7c2a-4b24-aa1f-946d11a055ad" />
</p>

# Borsh for C++20
`borsh-cpp20` is an implementation of the borsh serialization specification for C++20.

## Motivation
Basically, at the time of writing there was no feature complete borsh serializer / deserializer implementation available for C++ at all, so this code is an attempt to fill that gap.

## Current state
The library isn't ready for production, and the code is published just for building in public.

Below is a list of types specified in the Rust specification, with the ones implemented checked:
- [x] Integers
- [ ] Floats
- [ ] Unit (std::monostate)
- [ ] Fixed sized arrays
- [ ] Dynamic sized array (std::vector)
- [x] Struct
- [x] Named fields
- [ ] Enum
- [ ] HashMap (std::unordered_map)
- [ ] HashSet (std::unordered_set)
- [ ] Option (std::optional)
- [x] String (std::string)

The following types don't have a direct equivalent in C++:
- Unnamed fields
