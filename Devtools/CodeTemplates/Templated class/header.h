#pragma once

/*
 * Soft: Valorisation Engine
 * Author: $AUTHOR$
 * Creation date: $DATE$
 * This program is part of the Free Mobile product line,
 * All right reserved to Iliad.
 * Copyright (C) 2004-2011 Iliad,
 */

namespace $NAMESPACE$ {
template <typename Type> class $CLASS_NAME$ final {
public:
    $CLASS_NAME$() noexcept = default;
    ~$CLASS_NAME$() noexcept = default;
    $CLASS_NAME$($CLASS_NAME$ const&) noexcept = delete;
    $CLASS_NAME$($CLASS_NAME$&&) noexcept = delete;
    $CLASS_NAME$& operator=($CLASS_NAME$ const&) noexcept = delete;
    $CLASS_NAME$& operator=($CLASS_NAME$&&) noexcept = delete;
    void do_stuff() const { }
};
} // namespace $NAMESPACE$
