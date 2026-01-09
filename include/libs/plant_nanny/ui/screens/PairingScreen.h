#pragma once

#include "libs/plant_nanny/ui/IScreen.h"
#include <libs/common/ui/UI.h>
#include <string>

namespace plant_nanny::ui::screens
{
    /**
     * @brief Pairing screen showing PIN code
     */
    class PairingScreen : public IScreen
    {
    private:
        std::string _pin;

    public:
        PairingScreen() : _pin("------") {}
        
        void setPin(const std::string& pin) { _pin = pin; }
        
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
                .addChild(TextBuilder("BT Pair").fontSize(2).align(Align::CENTER).color(Color::Cyan).build())
                .addChild(TextBuilder("PIN:").fontSize(1).align(Align::CENTER).color(Color::White).build())
                .addChild(TextBuilder(_pin).fontSize(4).align(Align::CENTER).color(Color::Yellow).build())
                .build();

            col->measure(w, h);
            col->layout(0, 0);
            display.fillScreen(static_cast<uint16_t>(Color::Black));
            RenderContext ctx(0, 0, w, h, display);
            col->render(ctx);
        }
    };

} // namespace plant_nanny::ui::screens
