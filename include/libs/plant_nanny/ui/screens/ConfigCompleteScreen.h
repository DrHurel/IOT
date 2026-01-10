#pragma once

#include "libs/plant_nanny/ui/IScreen.h"
#include <libs/common/ui/UI.h>

namespace plant_nanny::ui::screens
{
    /**
     * @brief Configuration Complete screen shown after successful setup
     */
    class ConfigCompleteScreen : public IScreen
    {
    public:
        void render() override
        {
            using namespace common::ui;
            using namespace common::ui::components;

            auto& display = get_display();
            int w = get_screen_width();
            int h = get_screen_height();

            auto col = ColumnBuilder()
                .mainAxisAlignment(MainAxisAlignment::CENTER)
                .crossAxisAlignment(CrossAxisAlignment::CENTER)
                .spacing(15)
                .backgroundColor(Color::Black)
                .addChild(TextBuilder("Config").fontSize(3).align(Align::CENTER).color(Color::Green).build())
                .addChild(TextBuilder("Complete!").fontSize(3).align(Align::CENTER).color(Color::Green).build())
                .build();

            col->measure(w, h);
            col->layout(0, 0);
            display.fillScreen(static_cast<uint16_t>(Color::Black));
            RenderContext ctx(0, 0, w, h, display);
            col->render(ctx);
        }
    };

} // namespace plant_nanny::ui::screens
