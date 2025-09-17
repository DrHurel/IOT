#pragma once

/*
 * Soft: Valorisation Engine
 * Author: $AUTHOR$
 * Creation date: $DATE$
 * This program is part of the Free Mobile product line,
 * All right reserved to Iliad.
 * Copyright (C) 2004-2011 Iliad,
 */

#include <common/utils/EnumMapper.h>
namespace $NAMESPACE$ {
enum class $CLASS_NAME$ {
    A_VALUE,
    ANOTHER_VALUE
};
}
template<>
common::utils::EnumMapper<$NAMESPACE$::$CLASS_NAME$> const&common::utils::enum_mapper::get<$NAMESPACE$::$CLASS_NAME$>();
template <> constexpr std::string_view common::utils::enum_mapper::get_enum_name<$NAMESPACE$::$CLASS_NAME$>() { return "$NAMESPACE$::$CLASS_NAME$"; }

