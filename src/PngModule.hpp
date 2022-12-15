#include "rack.hpp"
#include <vector>

using namespace rack;

enum class ImageType {SVG, PNG};

typedef std::tuple<std::string, std::string> PanelInfo;

struct MyPanel {
    std::string path;
    std::string label;
    ImageType type;

    SvgPanel* svg_panel = 0;
    int png_handle = 0;

    MyPanel(PanelInfo config);

    void draw(const ModuleWidget::DrawArgs& args, float w, float h);

    float get_width();

};


struct PngModule : Module {

    MyPanel* current_panel;
    MyPanel* default_panel;
    bool show_panel_labels = false;

};

struct PngModuleWidget : ModuleWidget {

    /* 1536000013 58% 17 90 2.1-768 */
    /* TODO:
        save/load panel options from json
        make defaults do anything
        save/load panel selection
    */

    MyPanel* current_panel = 0;

    /*  TODO: make these static somehow so they don't have to be loaded from 
        scratch all the time and so the default can be set globally.
    */
    MyPanel* default_panel = 0;
    std::vector<struct MyPanel*> panel_options;
    std::map<std::string, MyPanel*> panel_map;

    void set_panels(const std::vector<PanelInfo> panels);

    void draw(const DrawArgs& args) override;

    void panel_select_menu(Menu* menu, PngModule* module);

    json_t* config_to_json();
    void config_from_json(json_t* rootJ);


};



