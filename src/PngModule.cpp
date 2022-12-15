#include "PngModule.hpp"
#include "TechTechTechnologies.hpp"

MyPanel::MyPanel(PanelInfo config) {
    label = std::get<0>(config);
    path = std::get<1>(config);

    if(path.find("svg") != std::string::npos)
    {
        type = ImageType::SVG;
        svg_panel = createPanel(asset::plugin(pluginInstance, path));
    }
    else
    {
        type = ImageType::PNG;
    }
}

float MyPanel::get_width()
{
    if(type == ImageType::SVG)
    {
        return svg_panel->box.size.x;
    }
    else
    {
        return 12.f;
    }
}

void MyPanel::draw(const ModuleWidget::DrawArgs& args, float w, float h)
{
    if(type == ImageType::SVG)
    {
        svg_panel->draw(args);
    }
    else
    {
        if(png_handle == 0)
        {
            png_handle = nvgCreateImage(
                args.vg,
                asset::plugin(pluginInstance, path).c_str(),
                0
            );
        }
    
        nvgSave(args.vg);
        nvgBeginPath(args.vg);
        NVGpaint png_paint = nvgImagePattern(args.vg, 0, 0, w,h, 0, png_handle, 1.0f);
        nvgRect(args.vg, 0, 0, w,h);
        nvgFillPaint(args.vg, png_paint);
        nvgFill(args.vg);
        nvgClosePath(args.vg);
        nvgRestore(args.vg);
        
 
    }

}


struct PanelMenu : MenuItem {

    PngModuleWidget* widget;
    MyPanel* current;
    int mode; //0 for current, 1 for default

    Menu* createChildMenu() override {
    
        Menu* menu = new Menu;
        struct PanelItem : MenuItem {
            PngModule* module;
            MyPanel* panel;
            int mode;
            void onAction(const event::Action& e) override {
                if(mode == 0)
                {
                    module->current_panel = panel;
                }
                else
                {
                    module->default_panel = panel;
                }
            }
        };

        for(auto& option : widget->panel_options)
        {
            PanelItem* item = createMenuItem<PanelItem>(option->label);
            item->mode = mode;
            item->module = (PngModule*)widget->module;
            item->panel = option;
            item->rightText = CHECKMARK(current==option);
            menu->addChild(item);
        }

        return menu;

    }

};

void PngModuleWidget::panel_select_menu(Menu* menu, PngModule* module)
{


    PanelMenu* panel_menu = createMenuItem<PanelMenu>("Panel", RIGHT_ARROW);
    panel_menu->widget = this;
    panel_menu->current = current_panel;
    panel_menu->mode = 0;
    menu->addChild(panel_menu);

    panel_menu = createMenuItem<PanelMenu>("Default Panel", RIGHT_ARROW);
    panel_menu->widget = this;
    panel_menu->current = default_panel;
    panel_menu->mode = 1;
    menu->addChild(panel_menu);


}

void PngModuleWidget::set_panels(const std::vector<PanelInfo> panels)
{
    for (const PanelInfo& config : panels)
    {
        panel_options.push_back(new struct MyPanel(config));
        MyPanel* new_panel = panel_options.back();

        panel_map.emplace(new_panel->label, new_panel);
        panel_map.emplace(new_panel->path, new_panel);

    }

    if(default_panel == 0)
    {
        try {
            default_panel = panel_map.at("Default");
        } catch (const std::out_of_range& e) {
            default_panel = panel_options[0];
        }
    }

    box.size.x = std::round(default_panel->get_width() / RACK_GRID_WIDTH) * RACK_GRID_WIDTH;

    current_panel = default_panel;
    if(!module)
    {
        return;
    }
    
    PngModule* mod = dynamic_cast<PngModule*>(this->module);

    mod->default_panel = default_panel;
    mod->current_panel = current_panel;

}

void PngModuleWidget::draw(const DrawArgs& args)
{
   if(module){
        PngModule* mod = dynamic_cast<PngModule*>(this->module);
        if(mod->current_panel != current_panel)
        {
            current_panel = mod->current_panel;
        }
    }
    current_panel->draw(args, box.size.x, box.size.y);

    ModuleWidget::draw(args);
}


