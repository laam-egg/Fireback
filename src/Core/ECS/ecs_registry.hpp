#ifndef ecs_registry_INCLUDED
#define ecs_registry_INCLUDED

#include "Core/Component/ComponentDataArray_impl.hpp"
#include "Core/Component/ComponentManager_impl.hpp"
#include "Core/System/SystemManager_impl.hpp"
#include "Core/ECS/ECS_impl.hpp"

#define USING_COMPONENT(T) \
	ComponentDataArray_INSTANTIATE_TEMPLATE_FOR_COMPONENT(T); \
	ComponentManager_INSTANTIATE_TEMPLATE_FOR_COMPONENT(T); \
	ECS_INSTANTIATE_TEMPLATE_FOR_COMPONENT(T)

#define USING_SYSTEM(T) \
	SystemManager_INSTANTIATE_TEMPLATE_FOR_SYSTEM(T); \
	ECS_INSTANTIATE_TEMPLATE_FOR_SYSTEM(T)

#endif // ecs_registry_INCLUDED
