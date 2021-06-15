#include "TechTechTechnologies.hpp"
#include <osdialog.h>
#include <stb_image_write.h>
#include "settings.hpp"

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

    void step() override {
        ModuleWidget::step();
        if(!module) return;

        MetadataFiles* mod = ((MetadataFiles*) module);
        
        if(mod->screen_request == 1)
        {
            mod->screen_request = 0;

            /*
            Based on Rack window rendering and plugin screenshot code
            https://github.com/VCVRack/Rack/blob/v1/src/window.cpp#L421
            */

            int fbWidth, fbHeight;
            glfwGetFramebufferSize(APP->window->win, &fbWidth, &fbHeight);
            int winWidth, winHeight;
            glfwGetWindowSize(APP->window->win, &winWidth, &winHeight);
            
            printf("Sizes: %i, %i\n%i, %i\n", fbWidth, fbHeight, winWidth, winHeight);

            math::Rect bb = APP->scene->rack->moduleContainer->getChildrenBoundingBox();
            printf("moduleContainer bb:\n%f, %f\n%f, %f\n", bb.pos.x, bb.pos.y, bb.size.x, bb.size.y);

            math::Rect old_bb = APP->scene->box;
            printf("scene bb:\n%f, %f\n%f, %f\n", old_bb.pos.x, old_bb.pos.y, old_bb.size.x, old_bb.size.y);

            
            // Draw scene

            //APP->scene->box.size = bb.size;
            APP->scene->box.size.x *= 2;
            APP->scene->box = bb;
            bb = APP->scene->box;


            old_bb = APP->scene->box;
            printf("new scene bb:\n%f, %f\n%f, %f\n", old_bb.pos.x, old_bb.pos.y, old_bb.size.x, old_bb.size.y);

            //Save the old zoom and offset
            math::Vec old_offset = APP->scene->rackScroll->offset;
            float old_zoom = rack::settings::zoom;

            //Change the zoom factor to 1
            rack::settings::zoom = 0.5;
            APP->scene->rackScroll->step();

            //Moves to the upper left of the modules bounding box
            Vec target = bb.pos;
            target = target.mult(APP->scene->rackScroll->zoomWidget->zoom);
            APP->scene->rackScroll->offset = target;

            float pixelRatio = 1;

            //Update scene
            APP->scene->step();

            //Render
            fbWidth = bb.size.x;
            fbHeight = bb.size.y;
            NVGLUframebuffer* fb = nvgluCreateFramebuffer(APP->window->vg, fbWidth, fbHeight, 0);
            nvgluBindFramebuffer(fb);
//            float pixelRatio = 1;
            nvgBeginFrame(APP->window->vg, fbWidth, fbHeight, pixelRatio);
            nvgScale(APP->window->vg, pixelRatio, pixelRatio);


            widget::Widget::DrawArgs args;
            args.vg = APP->window->vg;
            args.clipBox = APP->scene->box.zeroPos();

            APP->scene->draw(args);

            old_bb = args.clipBox;
            printf("clipbox bb:\n%f, %f\n%f, %f\n", old_bb.pos.x, old_bb.pos.y, old_bb.size.x, old_bb.size.y);

            glViewport(0, 0, fbWidth, fbHeight);
            glClearColor(0.0, 0.0, 0.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            nvgEndFrame(APP->window->vg);
//glfwSwapBuffers(APP->window->win);

            int width, height;
            width = fbWidth;
            height = fbHeight;
            uint8_t* data = new uint8_t[height * width * 4];
            glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
            printf("Size: %i, %i\n", width, height);


            for (int y = 0; y < height / 2; y++) {
                int flipY = height - y - 1;
                uint8_t tmp[width * 4];
                memcpy(tmp, &data[y * width * 4], width * 4);
                memcpy(&data[y * width * 4], &data[flipY * width * 4], width * 4);
                memcpy(&data[flipY * width * 4], tmp, width * 4);
            }


            std::string filename = asset::user("screenshots") + "/test.png";
            stbi_write_png(filename.c_str(), width, height, 4, data, width * 4);

            printf("Wrote screenshot to %s\n", filename.c_str());

            //Restore original view
            //Change the zoom factor to 1
            rack::settings::zoom = old_zoom;
            APP->scene->rackScroll->zoomPos = APP->scene->rackScroll->box.size.mult(0.5f);
            APP->scene->rackScroll->step();

            //Moves to the upper left of the modules bounding box
            target = old_offset;
            //target = target.mult(APP->scene->rackScroll->zoomWidget->zoom);
            APP->scene->rackScroll->offset = target;



//            APP->scene->rackScroll->offset = old_offset;
//            rack::settings::zoom = old_zoom;
            

            delete[] data;
            //Screenshot here
            //APP->patch->saveDialog();
        }

        if(mod->add_file_request == 1)
        {
            mod->add_file_request = 0;

            char* path = osdialog_file(OSDIALOG_OPEN, dir.c_str(), NULL, NULL);
            if(path)
            {
                if(!files_field->text.empty() && files_field->text[files_field->text.length()-1]!='\n')
                {
                    files_field->text += "\n";
                }
                files_field->text += path;

                dir = string::directory(path);
            }
        }


    }



    json_t* toJson() override {
        json_t* rootJ = ModuleWidget::toJson();

        MetadataFiles* mod = (MetadataFiles*) module;

        json_object_set_new(rootJ, "files", json_string(files_field->text.c_str()));
        json_object_set_new(rootJ, "dir", json_string(dir.c_str()));

        json_object_set_new(rootJ, "file_type", json_string(type_choice->getTypeKey()));

        json_object_set_new(rootJ, "file_type_id", json_integer(type_choice->value));



        return rootJ;

    }

    void fromJson(json_t* rootJ) override {
        ModuleWidget::fromJson(rootJ);

        MetadataFiles* mod = (MetadataFiles*) module;
        
        json_t* textJ;

        textJ = json_object_get(rootJ, "files");
        if(textJ) files_field->text = json_string_value(textJ);

        textJ = json_object_get(rootJ, "dir");
        if(textJ) dir=json_string_value(textJ);

        json_t* tint = json_object_get(rootJ, "file_type_id");
        if(tint) type_choice->value = json_integer_value(tint);
        
       

    }





};


Model* modelMetadataFiles = createModel<MetadataFiles, MetadataFilesWidget>("MetadataFiles");
