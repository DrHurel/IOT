#pragma once

#include <common/patterns/Singleton.h>

/*
 * Soft: Valorisation Engine
 * Author: $AUTHOR$
 * Creation date: $DATE$
 * This program is part of the Free Mobile product line,
 * All right reserved to Iliad.
 * Copyright (C) 2004-2011 Iliad,
 */

namespace $NAMESPACE$ {
class $CLASS_NAME$ final : public common::patterns::Singleton<$CLASS_NAME$> {
public:
    using Parent = common::patterns::Singleton<$CLASS_NAME$>;
    ~$CLASS_NAME$() noexcept = default;
    $CLASS_NAME$($CLASS_NAME$ const&) noexcept = delete;
    $CLASS_NAME$($CLASS_NAME$&&) noexcept = delete;
    $CLASS_NAME$& operator=($CLASS_NAME$ const&) noexcept = delete;
    $CLASS_NAME$& operator=($CLASS_NAME$&&) noexcept = delete;
    void do_stuff() const;

private:
    $CLASS_NAME$() noexcept = default;
    friend Parent;
};
} // namespace $NAMESPACE$
