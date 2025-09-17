#pragma once

/*
 * Soft: Valorisation Engine
 * Author: $AUTHOR$
 * Creation date: $DATE$
 * This program is part of the Free Mobile product line,
 * All right reserved to Iliad.
 * Copyright (C) 2004-2011 Iliad,
 */

#include <common/software_architecture/Library.h>

namespace $NAMESPACE$ {
class $CLASS_NAME$ final: public common::software_architecture::Library<$CLASS_NAME$, dependency_1, dependency_2,...> {
   public:
    $CLASS_NAME$() = delete;
    ~$CLASS_NAME$() = delete;
    $CLASS_NAME$($CLASS_NAME$ const&) noexcept = delete;
    $CLASS_NAME$($CLASS_NAME$&&) noexcept = delete;
    $CLASS_NAME$& operator=($CLASS_NAME$ const&) noexcept = delete;
    $CLASS_NAME$& operator=($CLASS_NAME$&&) noexcept = delete;
    static common::Result<void> initialize(const common::software_architecture::Context&);
    static common::Result<void> finalize();
};
} // namespace $NAMESPACE$
