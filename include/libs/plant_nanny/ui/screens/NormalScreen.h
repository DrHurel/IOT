#pragma once

#include "libs/plant_nanny/ui/IScreen.h"
#include <libs/common/ui/UI.h>

namespace plant_nanny::ui::screens
{
    /**
     * @brief Normal/home screen with button instructions
     */
    class NormalScreen : public IScreen
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
                .spacing(10)
                .backgroundColor(Color::Black)
                .addChild(TextBuilder("PlantNanny").fontSize(2).align(Align::CENTER).color(Color::Green).build())
                .addChild(TextBuilder("LEFT 3s: Pair").fontSize(1).align(Align::CENTER).color(Color::LightGray).build())
                .addChild(TextBuilder("RIGHT 3s: Reset").fontSize(1).align(Align::CENTER).color(Color::LightGray).build())
                .build();

            col->measure(w, h);
            col->layout(0, 0);
            display.fillScreen(static_cast<uint16_t>(Color::Black));
            RenderContext ctx(0, 0, w, h, display);
            col->render(ctx);
        }
    };

} // namespace plant_nanny::ui::screens
