#include "rack.hpp"
#include <vector>

using namespace rack;

struct PngModule : Module {

    std::string panel_path;
    bool show_panel_labels = false;

};

struct PngModuleWidget : ModuleWidget {

    /* 1536000013 58% 17 90 2.1-768 */
    /* TODO:
        Add function to add menu options to select panels
            Put into a dropdown menu
            And another for global defaults
        save/load panel selection
    */

    int png_handle = 0;

    int show_svg = 0;
    SvgPanel* svg_panel = 0;
    std::string png_path;
    std::string svg_path;
    std::vector<std::pair<std::string, std::string>> panel_options;

    void set_panels(const std::string svg_path, 
            const std::vector<std::pair<std::string, std::string>> panels);

    void draw(const DrawArgs& args) override;

    void panel_select_menu(Menu* menu, PngModule* module);

    json_t* config_to_json();
    void config_from_json(json_t* rootJ);


};



