#pragma once

#include "entityx/Entity.h"

enum class PathType
{
	PT_Small,
};

struct Path : public entityx::Component<Path>
{
	Path(PathType type = PathType::PT_Small);

	PathType m_type;
};