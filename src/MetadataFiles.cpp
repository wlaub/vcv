#include "TechTechTechnologies.hpp"
#include <osdialog.h>
#include <stb_image_write.h>
#include "settings.hpp"

// Get the GLFW API.
#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>

struct FileTypeItem : ui::MenuItem {
    enum FileTypes {
        IMAGE,
        FILE, 
        GIT,
        NUM_TYPES
    };

    int* target;
    int value;
 

    static const char* getTypeName(int value)
    {
        if(value == IMAGE) return "Image";
        if(value == FILE)   return "File";
        if(value == GIT)    return "Git Repository";
    }

    static const char* getTypeKey(int value)
    {
        if(value == IMAGE) return "image";
        if(value == FILE)   return "file";
        if(value == GIT)    return "git";
    }

    void onAction(const event::Action& e) override {
        *target = this->value;
    }

};

struct FileTypeChoice : LedDisplayChoice {

    ui::Menu* menu;
    int value=0;

    FileTypeChoice(){
        
    }

    const char* getTypeKey()
    {
        return FileTypeItem::getTypeKey(this->value);
    }

    void addEntry(ui::Menu* menu, int value)
    {
        FileTypeItem* item = new FileTypeItem;
        item->value = value; 
        item->target = &this->value;
        item->text = item->getTypeName(value);
        if(!strcmp(this->text.c_str(), item->text.c_str()))
        {
            item->rightText = CHECKMARK(true);
        }
        menu->addChild(item);
        
    }

    void onAction(const event::Action& e) override {
        menu = createMenu();
        menu -> addChild(createMenuLabel("File Type"));
        for(int i = 0; i < FileTypeItem::NUM_TYPES; ++i)
        {
            addEntry(menu, i);
        }
 
    }

    void step() override {
        text = FileTypeItem::getTypeName(value);
    }

    void draw(const DrawArgs& args) override {
        int rad = 2;
        bndOutlineBox(args.vg, 0,0, box.size.x, box.size.y, rad, rad, rad, rad, nvgRGBA(0xff,0xff,0xff,0xff));
        bndLabel(args.vg, 0, -1, box.size.x, box.size.y, -1, text.c_str());

    }
    void drawLayer(const DrawArgs& args, int layer) override {
    }

};

struct NewlineTextField : ui::TextField {
    void draw(const DrawArgs& args) override {
        //I thought I was going to modify the actual text before showing it in order to show newlines explicitly, but that did not turn out to work...
        ui::TextField::draw(args);
    }

};


struct MetadataFiles : Module {
    enum ParamIds {
        SCREEN_BUTTON_PARAM,
        ADD_FILE_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        SCREEN_IN_INPUT,
        NUM_INPUTS
    };
    enum OutputIds {
        NUM_OUTPUTS
    };
    enum LightIds {
        SCREEN_LIGHT,
        NUM_LIGHTS
    };

    int screen_request = 0;
    int add_file_request = 0;

    dsp::BooleanTrigger screen_button, add_file_button;
    dsp::SchmittTrigger screen_gate;

    dsp::PulseGenerator screen_pulse, screen_holdoff;

    MetadataFiles() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(SCREEN_BUTTON_PARAM, 0.f, 1.f, 0.f, "Save a screenshot of the current patch.");
    }

    void process(const ProcessArgs& args) override {

        bool screen_enable = !screen_holdoff.process(args.sampleTime);

        if( (   screen_button.process(params[SCREEN_BUTTON_PARAM].getValue() > 0.f) 
            || screen_gate.process(inputs[SCREEN_IN_INPUT].getVoltage()) )
            && screen_enable
            ) 
        {
            screen_request = 1;

            screen_holdoff.trigger(10);
            screen_pulse.trigger(1e-1);

        }

        lights[SCREEN_LIGHT].setBrightness(0);
        if(screen_pulse.process(args.sampleTime))
        {
            lights[SCREEN_LIGHT].setBrightness(1);
        }

        if (add_file_button.process(params[ADD_FILE_PARAM].getValue() > 0))
        {
            add_file_request = 1;
        }


    }

    bool fields_ready = false;

    NewlineTextField* files_field;
    FileTypeChoice * type_choice;
    std::string dir;

    json_t* dataToJson() override {
        json_t* rootJ = json_object();
        if(!fields_ready) return rootJ;
        json_object_set_new(rootJ, "files", json_string(files_field->text.c_str()));
        json_object_set_new(rootJ, "dir", json_string(dir.c_str()));

        json_object_set_new(rootJ, "file_type", json_string(type_choice->getTypeKey()));

        json_object_set_new(rootJ, "file_type_id", json_integer(type_choice->value));

        return rootJ;
    }


    bool load_json = false;
    json_t* widget_json;
    void dataFromJson(json_t* rootJ) override
    {
        widget_json = json_copy(rootJ);
        load_json = true;
    }



};


struct MetadataFilesWidget : ModuleWidget {
    NewlineTextField* files_field;
    FileTypeChoice * type_choice;
    std::string dir;

    MetadataFilesWidget(MetadataFiles* module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/MetadataFiles.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        float shift = 63.5+6.35/2;
        float y = 25.4-3.125;
        addParam(createParamCentered<LEDBezel>(mm2px(Vec(shift+6.35, y)), module, MetadataFiles::SCREEN_BUTTON_PARAM));
        addChild(createLightCentered<LEDBezelLight<GreenLight>>(mm2px(Vec(shift+6.35, y)), module, MetadataFiles::SCREEN_LIGHT));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(shift+19.05, y)), module, MetadataFiles::SCREEN_IN_INPUT));

        addParam(createParamCentered<LEDBezel>(
            Vec(box.size.x-mm2px(6.35*1.5), mm2px(y)), module, MetadataFiles::ADD_FILE_PARAM));
 

        type_choice = createWidget<FileTypeChoice>(mm2px(Vec(3.175, y-2.425)));
        type_choice->box.size = mm2px(Vec(63.5, 6.35));
        addChild(type_choice);

        files_field = createWidget<NewlineTextField>(mm2px(Vec(3.175, y+10.275 /*48.375-12.7*/)));
        files_field->box.size = Vec(box.size.x-mm2px(6.35), mm2px(118.225-y-10.275));
        files_field->multiline = true;
        addChild(files_field);

        dir = asset::user("patches");

    }

    void add_file(const char* path)
    {
            if(path && files_field->text.find(path) >= files_field->text.length())
            {
                if(!files_field->text.empty() && files_field->text[files_field->text.length()-1]!='\n')
                {
                    files_field->text += "\n";
                }
                files_field->text += path;

//                dir = string::directory(path);
                char* tpath = strdup(path);
                dir = dirname(tpath);
                free(tpath);
                if(module)
                {
                    MetadataFiles* mod = ((MetadataFiles*) module);
                    mod->dir=dir; 
                }
            }
 
    }

    void step() override {
        ModuleWidget::step();
        if(!module) return;

        MetadataFiles* mod = ((MetadataFiles*) module);
       
        if(!mod->fields_ready)
        {
            mod->files_field = files_field;
            mod->type_choice = type_choice;
            mod->dir = dir;
            mod->fields_ready = true;
        }

        if(mod->load_json)
        {
            loadJson(mod->widget_json);
            mod->load_json = false;
            free(mod->widget_json);
        }
 
 
        if(mod->screen_request == 1)
        {
            mod->screen_request = 0;
            if( strcmp(type_choice->getTypeKey(), "image") )
            { //Only take screenshots for image files
            }
            else
            {
                /*
                The following screenshot code is derived from existing Rack code, and inherits GPLv3 license
                Based on RackWidget rendering code in src/app/RackWidget.cpp
                https://github.com/VCVRack/Rack/blob/v1/src/app/RackWidget.cpp            
                and plugin screenshot code in src/window.cpp
                https://github.com/VCVRack/Rack/blob/v1/src/window.cpp#L420
                */

                math::Rect bb = APP->scene->rack->getModuleContainer()->getChildrenBoundingBox();
    //            printf("moduleContainer bb:\n%f, %f\n%f, %f\n", bb.pos.x, bb.pos.y, bb.size.x, bb.size.y);
                
                // Draw scene

                //APP->scene->box.size = bb.size;
                APP->scene->box.size.x *= 2;
                APP->scene->box = bb;
                bb = APP->scene->box;

                float pixelRatio = 1;

                //Set up the frame buffer to be rendered to
                int fbWidth = bb.size.x;
                int fbHeight = bb.size.y;
                NVGLUframebuffer* fb = nvgluCreateFramebuffer(APP->window->vg, fbWidth, fbHeight, 0);
                nvgluBindFramebuffer(fb);

                //Begin rendering
                nvgBeginFrame(APP->window->vg, fbWidth, fbHeight, pixelRatio);
                nvgScale(APP->window->vg, pixelRatio, pixelRatio);

                //Setup the draw args w/ the target framebuffer
                widget::Widget::DrawArgs args;
                args.vg = APP->window->vg;
                args.clipBox = bb.zeroPos();
                args.fb = fb;

                //Draw the rack background          
                APP->scene->rack->draw(args);

    //            args.clipBox = bb.zeroPos();
    //            args.clipBox.pos.x = 200;

                nvgTranslate(args.vg, -bb.pos.x, -bb.pos.y);

                //Draw modules
                Widget* mods = APP->scene->rack->getModuleContainer();

                for (widget::Widget* child : mods->children) {

                    ModuleWidget* w = dynamic_cast<ModuleWidget*>(child);
                    assert(w);

                    float xpos = child->box.pos.x;
                    float ypos = child->box.pos.y;

                    nvgSave(args.vg);
                    nvgTranslate(args.vg, xpos, ypos);
                    w->draw(args);
                    w->drawLayer(args, 1);
                    nvgRestore(args.vg);
                    nvgluBindFramebuffer(fb); //The module might bind its own framebuffer in its draw function

    //                printf("Drawing module %s at %f, %f\n", xpos, ypos, w->module->model->name.c_str());

                }


                // Draw cables
                float active_opacity = 1;
                float inactive_opacity = 0.33;
                float old_opacity = settings::cableOpacity;
                settings::cableOpacity = active_opacity;
                for (widget::Widget* w : APP->scene->rack->getCableContainer()->children) {
                    CableWidget* cw = dynamic_cast<CableWidget*>(w);
                    assert(cw);

                    Module* inMod = cw->cable->inputModule;
                    Module* outMod = cw->cable->outputModule;
                    settings::cableOpacity = active_opacity;
                    if((inMod && inMod->isBypassed()) || (outMod && outMod->isBypassed()))
                    {   //i.e. the cable is connected to a disabled module
                        settings::cableOpacity = inactive_opacity;
                    }

                   cw->draw(args);

//                    cw->drawLayer(args, 1);
                    cw->drawLayer(args, 2);
                    cw->drawLayer(args, 3);
//                    cw->drawPlugs(args);
    //                printf("Cable at %f, %f \n", cw->getOutputPos().x, cw->getOutputPos().y);
                }
                settings::cableOpacity = old_opacity;

                //Done
                glViewport(0, 0, fbWidth, fbHeight);
                glClearColor(0.0, 0.0, 0.0, 1.0);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

                nvgEndFrame(APP->window->vg);

                //Extract the pixel data
                uint8_t* data = new uint8_t[fbHeight * fbWidth * 4];
                glReadPixels(0, 0, fbWidth, fbHeight, GL_RGBA, GL_UNSIGNED_BYTE, data);
    //            printf("Size: %i, %i\n", fbWidth, fbHeight);

                //Flip the image vertically
                for (int y = 0; y < fbHeight / 2; y++) {
                    int flipY = fbHeight - y - 1;
                    uint8_t tmp[fbWidth * 4];
                    memcpy(tmp, &data[y * fbWidth * 4], fbWidth * 4);
                    memcpy(&data[y * fbWidth * 4], &data[flipY * fbWidth * 4], fbWidth * 4);
                    memcpy(&data[flipY * fbWidth * 4], tmp, fbWidth * 4);
                }

                time_t rawtime;
                time(&rawtime);
                char timestamp[256];
                strftime(timestamp, 256, "%Y%m%d-%H%M%S", localtime(&rawtime));
                std::string filename = asset::user("screenshots");
                system::createDirectory(filename);
                filename += "/";
                filename += timestamp;
                filename += ".png";


                stbi_write_png(filename.c_str(), fbWidth, fbHeight, 4, data, fbWidth * 4);
                printf("Wrote screenshot to %s\n", filename.c_str());

                add_file(filename.c_str());

                delete[] data;
                nvgluBindFramebuffer(NULL);

            /*
            End of screenshot code
            */            
            }
        }

        if(mod->add_file_request == 1)
        {
            mod->add_file_request = 0;

            char* path = osdialog_file(OSDIALOG_OPEN, dir.c_str(), NULL, NULL);
            add_file(path);
        }


    }


    void loadJson(json_t* rootJ) {

        MetadataFiles* mod = (MetadataFiles*) module;
        
        json_t* textJ;

        textJ = json_object_get(rootJ, "files");
        if(textJ) files_field->text = json_string_value(textJ);

        textJ = json_object_get(rootJ, "dir");
        if(textJ){
            dir=json_string_value(textJ);
                if(module)
                {
                    MetadataFiles* mod = ((MetadataFiles*) module);
                    mod->dir=dir; 
                }
 
        }

        json_t* tint = json_object_get(rootJ, "file_type_id");
        if(tint) type_choice->value = json_integer_value(tint);
        
       

    }





};


Model* modelMetadataFiles = createModel<MetadataFiles, MetadataFilesWidget>("MetadataFiles");
