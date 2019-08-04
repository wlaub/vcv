#include "TechTechTechnologies.hpp"



void center(Widget* thing, int x, int y)
{
    float w = thing->box.size.x;
    float h = thing->box.size.y;
    thing->box.pos.x -= x*w/2;
    thing->box.pos.y -= y*h/2;
}

//Using 0-10V CV's per https://vcvrack.com/manual/VoltageStandards.html
int cv_to_depth(float cv, int max)
{
    return 1+round((max-1)*cv/10);
}

int cv_to_num(float cv, int depth)
{
    int max = pow(2,depth)-1;
    return round(max*cv/10);

}

float depth_to_cv(int depth, int max)
{
    return 10*float(depth-1)/(max-1);
}

float num_to_cv(int num, int depth)
{
    int max = pow(2,depth)-1;
    return 10*float(num)/max;
}

double db_to_num(float val, float unity)
{
/*
Convert a number representing Bels to a scaling factor so that
0 -> 1
1 -> 10^unity/10
-1 -> 10^-unity/10
i.e. unity is the dB value represented by a value of 1
*/

    return pow(10, val*unity/10);

}

double split_log(float val, float m, float n)
{
/*
A split log map where
0->.5 is 0->-m dB
.5 -> 1 is -m -> -n dB
*/
    if (val < .5)
    {
        return db_to_num(-2*val, m);
    }
    else
    {
        float k = 2*m/n-2;
        return db_to_num((k*val-k-1), n);
    }
 
}

void NumField::onSelectText(const event::SelectText &e) 
{
    if (e.codepoint < 128) {
                std::string newText(1, (char) e.codepoint);
                        insertText(newText);
                            }
    e.consume(this);

    if (text.size() > 0) 
    {
        try 
        {
            int num = std::stoi(text, 0,0);
            outNum = num;
        }
        catch (...) 
        {
            outNum -= 1;
        }
    }
    else
    {
        outNum = 0;
    }
}

TTTEncoder::TTTEncoder() {
    setSVG(SVG::load(assetPlugin(pluginInstance, "res/Components/TTTEncoder.svg")));
    minAngle=0;
    maxAngle=2*M_PI;
    snap=true;
    controller = 0;
}

void TTTEncoder::configureLights()
{
    float xpos = box.size.x/2;
    float ypos = box.size.y/2;
    float rad = 15;
    float angle = M_PI/7;
    if(flip) angle= -6*M_PI/7;
    for(int i = 0; i < 7; ++i)
    {
        lights[i] = createLightCentered<SmallLight<GreenLight>>(
            Vec(xpos+-rad*sin(2*M_PI*i/7+angle), ypos+rad*cos(2*M_PI*i/7+angle)), 
//            Vec(0, 0),
            paramQuantity->module, 0);
//        addChild(lights[i]);
    }
    lights_ready=true;
    controller->update(0);
}

void TTTEncoder::reset()
{
    controller->reset();
}

void TTTEncoder::draw(const DrawArgs &args)
{
    RoundBlackKnob::draw(args);
    
    if(lights_ready)
    {
        for (int i = 0; i < 7; ++i)
        {
            nvgSave(args.vg);
            nvgTranslate(args.vg, lights[i]->box.pos.x, lights[i]->box.pos.y);
            lights[i]->draw(args);
            nvgRestore(args.vg);
        }
    }
}

void TTTEncoder::dataFromJson(json_t *rootJ) {
    json_t *valueJ = json_object_get(rootJ, "value");
    if (valueJ)
        controller->values[controller->group][controller->index] = char(json_number_value(valueJ));
        setValue(json_number_value(valueJ));
}

void TTTEncoder::setValue(float v)
{
    value = v;
    if (lights_ready)
    {
        for(int i = 0; i < 7; ++i) lights[i]->color = nvgRGBAf(0,0,0,0);
        lights[char(value)]->color = color;
    }

//    EventChange e;
//    onChange(e);
}

void TTTEncoder::onHoverKey(const event::HoverKey &e) 
{
    if(e.action == GLFW_PRESS)
    {
        switch (e.key) 
        {
            case GLFW_KEY_PAGE_UP:
    //        case GLFW_KEY_UP:
                controller->update(1);
                e.consume(this);
                return;
            break;
            case GLFW_KEY_PAGE_DOWN:
    //        case GLFW_KEY_DOWN:
                controller->update(-1);
                e.consume(this);
                return;
            break;
        }
    }
    RoundBlackKnob::onHoverKey(e);  
}

void TTTEncoder::onDragMove(const event::DragMove &e) {
    if (e.button != GLFW_MOUSE_BUTTON_LEFT)
        return;

    if(paramQuantity)
    {
        float range;
        if (paramQuantity->isBounded()) {
            range = paramQuantity->getRange();
        }
        else {
            // Continuous encoders scale as if their limits are +/-1
            range = 2.f;
        }
        float delta = (horizontal ? e.mouseDelta.x : -e.mouseDelta.y);
        delta *= KNOB_SENSITIVITY;
        delta *= speed;
        delta *= range;

        int mods = APP->window->getMods();
        if ((mods & RACK_MOD_MASK) == RACK_MOD_CTRL) {
            delta /= 16.f;
        }
        // Drag even slower if mod+shift is held
        if ((mods & RACK_MOD_MASK) == (RACK_MOD_CTRL | GLFW_MOD_SHIFT)) {
            delta /= 256.f;
        }

        snapValue += delta;

        float maxValue = paramQuantity->getMaxValue();
        float minValue = paramQuantity->getMinValue();

        if(floor(snapValue) > floor(snapValue+delta))
        {
            controller->update(-1);
            snapValue = value;
        }
        if(floor(snapValue) < floor(snapValue+delta))
        {
            controller->update(1);
            snapValue = value;
        }

        //Looping effect
        if (snapValue > maxValue)      
        {
            snapValue += minValue - maxValue;
        }
        else if (snapValue < minValue)
        {
            snapValue += maxValue - minValue;
        }
    }
    ParamWidget::onDragMove(e);
}


void TTTEncoder::step() {
    // Re-transform TransformWidget if dirty
    if (spinning or dirty) {
        float angle;
        float maxValue = paramQuantity->getMaxValue();
        float minValue = paramQuantity->getMinValue();

        if (std::isfinite(minValue) && std::isfinite(maxValue)) {
            angle = rescale(value, minValue, maxValue, minAngle, maxAngle);
        }
        else {
            angle = rescale(value, -1.0, 1.0, minAngle, maxAngle);
            angle = fmodf(angle, 2*M_PI);
        }
        if(flip)
        {
            angle+=M_PI;
        }

        if(fabsf(angle-lastAngle) > M_PI) //Angle wrapping
        {
            if(angle > lastAngle)
            {
                lastAngle += 2*M_PI;
            }
            else if(angle < lastAngle)
            {
                lastAngle -= 2*M_PI;
            }
        }

        lastAngle = angle*.5+lastAngle*.5; //Knob slew
        spinning = true;

        if(fabsf(lastAngle-angle)<.0001)
        {
            lastAngle = angle;
            spinning = false;
        }
        else
        {
            dirty = true;
        }
        angle = lastAngle;

        tw->identity();
        // Rotate SVG
        Vec center = sw->box.getCenter();
        tw->translate(center);
        tw->rotate(lastAngle);
        tw->translate(center.neg());
    }
    Widget::step();
}


void EncoderController::update(int amount)
{ //TODO: Called by the widget to increment or decrement
    values[group][index] += amount;
    delta += amount;
    if(values[group][index] == 255) values[group][index] = 6;
    else if(values[group][index] == 7) values[group][index] = 0;

    widget->setValue(values[group][index]);
}

void EncoderController::setValues(unsigned char* v, unsigned char tgroup)
{ //
    if(tgroup >= ngroup)
    {
        tgroup = group;
    }
    for(int i = 0; i < 7; ++i)
    {
        values[tgroup][i] = v[i];
    }
    if(tgroup == group)
    {
        widget->setValue(values[group][index]);
    }
}

int EncoderController::process()
{   //Used to obtain state info from within the module
    //Returns the difference in encoder value since the last time process
    //was called.
    int result = delta;
    delta = 0;
    return result;
}

void EncoderController::clear()
{
    for(int i = 0; i < ngroup; ++i)
    {
        for(int j = 0; j < 7; ++j)
            values[i][j] = defaults[j];
    }
}


void EncoderController::reset()
{
    delta += defaults[index]-values[group][index];
    values[group][index] = defaults[index];
    update(0);   
}

int EncoderController::getValue()
{
    return values[group][index];
}
int EncoderController::getValue(int idx)
{
    return values[group][idx];
}

void EncoderController::setColor(EncoderController* src)
{
    widget->color = src->widget->color;
    update(0);
}

void EncoderController::setColor(float r, float g, float b)
{
    widget->color = nvgRGBAf(r,g,b,1);
    update(0);
}

void EncoderController::setIndex(unsigned char idx, unsigned char tgroup)
{ //Changes current index and sets the widget accordingly
    if(tgroup < ngroup)
    {
        group = tgroup;
    }
//        if(index == idx) return;
    index = idx;

    widget->setValue(values[group][idx]);
}



// The pluginInstance-wide instance of the Plugin class
Plugin *pluginInstance;

void init(rack::Plugin *p) {
    pluginInstance = p;
    // The "slug" is the unique identifier for your pluginInstance and must never change after release, so choose wisely.
    // It must only contain letters, numbers, and characters "-" and "_". No spaces.
    // To guarantee uniqueness, it is a good idea to prefix the slug by your name, alias, or company name if available, e.g. "MyCompany-MyPlugin".
    // The ZIP package must only contain one folder, with the name equal to the pluginInstance's slug.
//    p->website = "https://github.com/wlaub/vcv";
//    p->manual = "https://github.com/wlaub/vcv/blob/master/README.md";

    // For each module, specify the ModuleWidget subclass, manufacturer slug (for saving in patches), manufacturer human-readable name, module slug, and module name    

    p->addModel(modelDAC);

    p->addModel(modelmDAC);

    p->addModel(modelPrometheus);

    p->addModel(modelVulcan);

    p->addModel(modelSisyphus);

    p->addModel(modelPolyphemus);

    p->addModel(modelOuroboros);

    p->addModel(modelOdysseus);
    
    p->addModel(modelMneme);

    p->addModel(modelAchilles);

    p->addModel(modelQMod);

    p->addModel(modelConvo);

    p->addModel(modelPleiades);   

    // Any other pluginInstance initialization may go here.
    // As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}
