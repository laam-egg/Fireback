#ifndef ComponentID_INCLUDED
#define ComponentID_INCLUDED

#include <cstdint>

using ComponentID = std::uint8_t;
using ComponentIDCount = std::uint8_t;
// TODO: Limit number of components when the overall structure perfects.
constexpr ComponentIDCount const MAX_COMPONENTS = (ComponentIDCount)(-1);
constexpr ComponentID const MAX_COMPONENT_ID = MAX_COMPONENTS - 1;

#endif // ComponentID_INCLUDED
