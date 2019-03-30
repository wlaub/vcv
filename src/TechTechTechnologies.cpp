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

void NumField::onTextChange() 
{
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


// The plugin-wide instance of the Plugin class
Plugin *plugin;

void init(rack::Plugin *p) {
	plugin = p;
	// The "slug" is the unique identifier for your plugin and must never change after release, so choose wisely.
	// It must only contain letters, numbers, and characters "-" and "_". No spaces.
	// To guarantee uniqueness, it is a good idea to prefix the slug by your name, alias, or company name if available, e.g. "MyCompany-MyPlugin".
	// The ZIP package must only contain one folder, with the name equal to the plugin's slug.
	p->slug = "TechTechTechnologies";
	p->version = TOSTRING(VERSION);
	p->website = "https://github.com/wlaub/vcv";
	p->manual = "https://github.com/wlaub/vcv/blob/master/README.md";

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

    p->addModel(modelAthena);

    p->addModel(modelAchilles);

    p->addModel(modelQMod);

    p->addModel(modelConvo);

	// Any other plugin initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}
