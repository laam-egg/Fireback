#ifndef EntityID_INCLUDED
#define EntityID_INCLUDED

#include <cstdint>

using EntityID = std::uint32_t;
using EntityIDCount = EntityID;
// TODO: Limit max number of entity IDs when the overall structure perfects.
constexpr EntityIDCount const MAX_ENTITIES = 5000;
constexpr EntityID const MAX_ENTITY_ID = MAX_ENTITIES - 1;

#endif // EntityID_INCLUDED
