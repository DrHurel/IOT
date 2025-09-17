#include "$FILENAME_BASE$.h"
namespace { // Anonymous namespace on purpose
using ET = $NAMESPACE$::$CLASS_NAME$;
common::utils::EnumMapper<$NAMESPACE$::$CLASS_NAME$> const __enum_mapper(
    {
        { "A_VALUE", ET::A_VALUE },
        { "ANOTHER_VALUE", ET::ANOTHER_VALUE }
    });
}
template<>
common::utils::EnumMapper<$NAMESPACE$::$CLASS_NAME$> const&common::utils::enum_mapper::get<$NAMESPACE$::$CLASS_NAME$>() {
    return __enum_mapper;
}
