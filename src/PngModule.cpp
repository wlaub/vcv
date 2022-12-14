#include "PngModule.hpp"
#include "TechTechTechnologies.hpp"


void PngModuleWidget::panel_select_menu(Menu* menu, PngModule* module)
{

    menu->addChild(createMenuLabel("Panel"));

    struct PanelItem : MenuItem {
        PngModule* module;
        std::string panel_path;
        void onAction(const event::Action& e) override {
            module->panel_path = panel_path;
        }
    };

    for(const auto& [label, path] : panel_options)
    {
        PanelItem* item = createMenuItem<PanelItem>(label);
        item->module = module;
        item->panel_path = path;
        item->rightText = CHECKMARK(png_path==path);
        menu->addChild(item);
    }


}

void PngModuleWidget::set_panels(const std::string svg_path, 
        const std::vector<std::pair<std::string, std::string>> panels)
{
    panel_options.push_back({"Default", svg_path});
    panel_options.insert(panel_options.end(), panels.begin(), panels.end());

    this->svg_path = svg_path;
    //Set the box size from the svg panel
    svg_panel = createPanel(asset::plugin(pluginInstance, svg_path));
    box.size.x = std::round(svg_panel->box.size.x / RACK_GRID_WIDTH) * RACK_GRID_WIDTH;

    show_svg = 1;
    png_path = svg_path;
    if(!module)
    {
        return;
    }
    
    PngModule* mod = dynamic_cast<PngModule*>(this->module);

    mod->panel_path = png_path;

}

void PngModuleWidget::draw(const DrawArgs& args)
{
   if(module){
        PngModule* mod = dynamic_cast<PngModule*>(this->module);
        if(mod->panel_path != png_path)
        {
            png_handle = 0;
            png_path = mod->panel_path;
        }
        show_svg = 0;
        if(png_path == svg_path)
        {
            show_svg = 1;
        }
    }
    if(png_handle == 0)
    {
        png_handle = nvgCreateImage(
            args.vg,
            asset::plugin(pluginInstance, png_path).c_str(),
            0
            );
    }
    if(show_svg && svg_panel)
    {
        svg_panel->draw(args);
    }
    else
    {
        nvgSave(args.vg);
        nvgBeginPath(args.vg);
        float w = box.size.x;
        float h = box.size.y;
        NVGpaint png_paint = nvgImagePattern(args.vg, 0, 0, w,h, 0, png_handle, 1.0f);
        nvgRect(args.vg, 0, 0, w,h);
        nvgFillPaint(args.vg, png_paint);
        nvgFill(args.vg);
        nvgClosePath(args.vg);
        nvgRestore(args.vg);
    }

    ModuleWidget::draw(args);
}


