#pragma once

#include <common/patterns/Singleton.h>

/*
 * Author: $AUTHOR$
 * Creation date: $DATE$
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
