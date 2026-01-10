#pragma once

#include "libs/plant_nanny/ui/IScreen.h"
#include <libs/common/ui/UI.h>

namespace plant_nanny::ui::screens
{
    /**
     * @brief Screen shown when user tries to pair an already paired device
     * Displays an error message asking for a factory reset
     */
    class AlreadyPairedScreen : public IScreen
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
                .addChild(TextBuilder("Error").fontSize(2).align(Align::CENTER).color(Color::Red).build())
                .addChild(TextBuilder("Already Paired").fontSize(2).align(Align::CENTER).color(Color::Yellow).build())
                .addChild(TextBuilder("Factory reset").fontSize(1).align(Align::CENTER).color(Color::White).build())
                .addChild(TextBuilder("required").fontSize(1).align(Align::CENTER).color(Color::White).build())
                .build();

            col->measure(w, h);
            col->layout(0, 0);
            display.fillScreen(static_cast<uint16_t>(Color::Black));
            RenderContext ctx(0, 0, w, h, display);
            col->render(ctx);
        }
    };

} // namespace plant_nanny::ui::screens
