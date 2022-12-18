#include "rack.hpp"
#include <vector>

using namespace rack;

enum class ImageType {SVG, PNG};

typedef std::tuple<std::string, std::string> PanelInfo;

struct MyPanel {
    std::string path;
    std::string label;
    ImageType type;

    int png_handle = 0;
    NSVGimage* svg_handle = 0;

    float width = 12.f*RACK_GRID_WIDTH;

    MyPanel(PanelInfo config);

    void draw(const ModuleWidget::DrawArgs& args, float w, float h);

    float get_width();

};

struct MyPanelCache {

    float width = 0;
    MyPanel* default_panel = 0;
    MyPanel* label_panel = 0;
    bool show_labels = false;

    std::vector<struct MyPanel*> panel_options;
    std::map<std::string, MyPanel*> panel_map;

    void set_panels(const std::vector<PanelInfo> panels);
    void add_panel(PanelInfo config);
    void find_default_panel(const char* default_label=0);
    void set_label_panel(const char* path = 0);

};

struct PngModule : Module {

    MyPanelCache* panel_cache = 0;
    std::string current_panel_label;
    MyPanel* current_panel = 0;
    bool show_panel_labels = false;

    void save_panel(json_t* rootJ);
    void load_panel(json_t* rootJ);
    json_t* dataToJson() override;
    void dataFromJson(json_t* rootJ) override;

};

typedef std::map<std::string, MyPanelCache*> PanelCacheMap;

struct PngModuleWidget : ModuleWidget {

    /* 1536000013 58% 17 90 2.1-768 */

    MyPanel* current_panel = 0;

    std::string slug = "";

    static PanelCacheMap panel_cache_map;
    MyPanelCache* panel_cache = 0;

    std::string get_panel_definitions_path();
    std::string get_panel_settings_path();
    json_t* load_panel_settings();
    void save_panel_settings();
    void load_panels_from_json();
    void init_panels(std::string slug);
    void _init_instance_panels();

    void draw(const DrawArgs& args) override;

    void panel_select_menu(Menu* menu, PngModule* module);

    json_t* config_to_json();
    void config_from_json(json_t* rootJ);


};



