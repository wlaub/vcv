import xml.etree.ElementTree as et
import sys
import ast
from svgpathtools.parser import parse_path
import panel_config, module_template
import os
import copy

class Metadata(panel_config.MetadataConfig):

    def __init__(self):
        self.config = {}

    def add_node(self, node, layer):
        """
        Extract metadata from nodes on the appropriate layer
        """
        if layer == None or layer.lower() != 'metadata': return False
        if 'text' not in node.tag.lower(): return False
        src = '\n'.join([line.text for line in node.getchildren() if line.text != None]).strip()
        if src[0] != '{':
            src = f'{{{src}}}'
        try:
            new_configs = ast.literal_eval(src)
        except:
            print(src)
            raise

        self.config.update(new_configs) 

    def apply_class(self, ctrl):
        """
        Apply class to the given control as needed
        """
        if not 'class' in ctrl.config.keys(): return False
        classes = self.config.get('classes', {})
        ctrl_class = ctrl.config['class']
        try:
            new_data = copy.deepcopy(classes[ctrl_class])
        except:
            print(f'Failed to get class {ctrl_class} for {ctrl._id}')
            raise
        del ctrl.config['class']
        new_data.update(ctrl.config)
        ctrl.config=new_data

    def is_keep(self, layer):
        """
        Return True if the layer should be kept in the final output svg or
        False if it should not
        """
        try:
            keep = self.config['layers']
            if layer in keep: return True
            return False
        except KeyError: return True


    def get_tags(self):
        """
        Return a string representing tags defined in metadata
        """
        tags = self.config.get('tags', None)
        if tags == None: return ''
        #validate against vcv src?
        return ', '+', '.join(tags)

class Control(panel_config.ControlConfig):
    """
    Represents a control derived from path metadata
    """

    @staticmethod
    def is_title(node):
        return node.tag[-5:].lower() == 'title'

    @staticmethod
    def is_desc(node):
        return node.tag[-4:].lower() == 'desc'


    @staticmethod
    def is_label(key):
        return key[-5:].lower()=='label'

    @staticmethod
    def is_control(node, layer):
        """
        Is a control if the label parameter is one of param, input, or output
        """
        if layer == None or layer.lower() not in Control.kind_map.keys():
            return False
        return True
        #just using layers for now
        for key,val in node.items():
            if Control.is_label(key):
                if val.lower() in ['param', 'input', 'output']:
                    return True
        return False

    def __init__(self, node, layer):
        """
        self.kind from the path label describes the type of control
        self._id from the path id is the enum name excluding _PARAM/INPUT/etc
        self.widget from the path title is the widget class name
        self.config is a python variable in the path description (dict)
        self.pos is [x,y] location of the center of the control
        """
        self.kind = self.enum_kind_map[self.kind_map[layer.lower()]]

        self._id = node.get('id').upper()
        d = node.get('d')

        self.enum_base = None
        self.enum_idx = None
        self.enum_count = None

        self.path = parse_path(d)
       
        bbox = self.path.bbox()
        self.pos = [(bbox[i*2]+bbox[i*2+1])/2 for i in range(2)]

        self.widget=None
        self.config = {}
        for subnode in node.getchildren():
            if Control.is_title(subnode):
                self.widget = subnode.text
            elif Control.is_desc(subnode):
                try:
                    self.config = ast.literal_eval(subnode.text)
                except:
                    print(f'Failed to load for {self._id} from {subnode.text}')
                    raise

    def get_index(self):
        """
        Return the index of this control or None if it is not indexed.
        """
        if not '_' in self._id: return None
        if self.enum_idx != None: return self.enum_idx
        try:
            self.enum_idx= int(self._id.split('_')[-1])
            return self.enum_idx
        except:
            return None

    def get_base_name(self):
        """
        Get the base id/name without any indexing suffix
        """
        idx = self.get_index()
        name = self._id
        if idx != None:
            name = '_'.join(name.split('_')[:-1])
        return name

    def get_enum_base(self):
        """
        Get the control enum base name of the form
        KIND_NAME
        from self.kind and self._id respectively, stripping any _#
        """
        if self.enum_base != None: return self.enum_base
        name = self.get_base_name()
        self.enum_base=f'{self.kind.upper()}_{name.upper()}' 
        return self.enum_base

    def get_enum_expr(self):
        """
        Return the expression to reference this control by enum and index
        e.g. enum_base   (not indexed)
             enum_base   (indexed)
             enub_base+1 (indexed)
        """
        result = self.get_enum_base()
        idx = self.get_index()
        if idx != None and idx != 0:
            result = f'{result}+{idx}'
        return result

    def get_enum(self, controls):
        """
        Compute the enumerator rendering string for this control or None if
        this control is indexed and not the first index. 
        Return [enum_index, enum_string]
        """
        idx = self.get_index()
        if idx != None and idx > 0: return None, None

        enum_base = self.get_enum_base()
        count=0
        if idx == 0:
            if self.enum_count == None:
                for ctrl in controls:
                    if ctrl != self and ctrl.get_enum_base() == enum_base:
                        try:
                            count = max(count, ctrl.get_index())
                        except TypeError:
                            raise KeyError(f'Un-indexed instance of {enum_base}')
                self.enum_count = count+1
            count = self.enum_count
        else:
            count = 1
        return self.kind, self.create_enum.format(
                name = enum_base, count= count
                )

    def get_auto(self, kind):
        """
        Return the _id of the param that relates to this port by kind
        name[_#] -> name_kind[#]
        e.g. env_1 -> env_atv_1
        """
        idx = self.get_index()
        base = self.get_base_name()
        if idx == None: return f'{base}_{kind}'
        return f'{base}_{kind}_{idx}'

    def get_auto_gain(self): return self.get_auto('ATV')
    def get_auto_offset(self): return self.get_auto('OFFSET')

    def get_instantiation(self, modname):
        """
        Return the panel instantiation string for this control
        """
        try:
            result = self.create_strings[self.kind].format(
                name=modname, widget=self.widget,
                id=self.get_enum_expr(), xpos = self.pos[0], ypos = self.pos[1],
                **self.config
                )
        except KeyError as e:
            result = f'FAILED to instantiate {self._id} due to missing param {e}'

        return result

    def get_variable_name(self):
        """
        name of the variable this value will be stored in
        """
        return self.get_enum_base().lower()

    def get_variable_access(self):
        """
        string to get the indexed variable name for this control
        """
        base = self.get_variable_name()
        idx = self.get_index()
        if idx == None: return base
        return f'{base}[{idx}]'

    def get_vcv_access(self):
        """
        string to access the vcv object e.g. inputs[INPUT_NAME+2]
        """
        index = self.get_enum_expr()
        return f'{self.kind}s[{index}]'

    def get_declaration_string(self):
        """
        Get the string that declares the variable for this value
        indexed [0]:  'float[N] variable_name;'
        indexed [>0]: None
        single:       'float variable_name;'
        """
        idx = self.get_index()
        if idx != None and idx > 0: return None
        type_str = self.config.get('type', 'float')
        var_name = self.get_variable_name()
        if idx == None: return f'{type_str} {var_name};'
        return f'{type_str} {var_name}[{self.enum_count}];'

    def get_param_string(self, controls):
        """
        Get the string that reads the param value into the base variable
        Needs a list of all controls to look up other refs
        """
        if self.kind != 'param': return
        return f'{self.get_variable_access()} = {self.get_vcv_access()}.value;'

    def get_input_string(self, controls):
        """
        Get the string that reads the value into the base variable
        Needs a list of all controls to look up other refs
        """
        if self.kind != 'input' : return
        result = []
        result.append( f'{self.get_variable_access()} = {self.get_vcv_access()}.value;')

        result.append(self.get_norm_string())
        result.append(self.get_gain_string(controls))
        result.append(self.get_offset_string(controls))
        result.append(self.get_clip_string())

        result = list(filter(lambda x: x != None, result))

        return '\n'.join(result)

    def get_output_string(self, controls):
        """
        Get the string that updates a variable and loads it into the relevant
        output
        """
        if self.kind in ['input', 'param']: return
        result = []
        result.append(self.get_gain_string(controls))
        result.append(self.get_offset_string(controls))
        result.append(self.get_clip_string())

        result.append( f'{self.get_vcv_access()}.value = {self.get_variable_access()};')

        result = list(filter(lambda x: x != None, result))

        return '\n'.join(result)


    def get_norm_string(self):
        try: #norm unused inputs
            norm = self.config['norm']
        except KeyError: return None
        else:
            return f"""if (!{self.get_vcv_access()}.active) 
{{
    {self.get_variable_access()} = {norm};
}}"""

    def get_gain_string(self, controls):
        """
        Get the gain apply string if there is one for this control
        """
        try: #apply gain from params 
            gain = self.config['gain']
        except KeyError: return None
        else:
            try:
                gain = float(gain)
            except ValueError:
                if gain == 'auto':
                    gain_src = self.get_auto_gain()
                else:
                    gain_src = gain
                ctrl_src = None
                for ctrl in controls:
                    if ctrl._id == gain_src: 
                        ctrl_src = ctrl
                        break
                if ctrl_src == None:
                    raise KeyError(f'No matching gain source {gain_src} for {self._id}')
                gain = ctrl_src.get_variable_access()
            return f"{self.get_variable_access()} *= {gain};"

    def get_offset_string(self, controls):
        """
        Get the offset apply string if there is one for this control
        """
        try: #apply offset
            offset = self.config['offset']
        except KeyError: pass
        else:
            off_off = offset.get('offset', 0)
            off_scale = offset.get('scale', 1)
            off_src = offset.get('src', None)
            off_val = 0;
            if off_src == None: pass
            if off_src == 'auto':
                off_src = self.get_auto_offset()

            if off_src != None:
                ctrl_src = None
                for ctrl in controls:
                    if ctrl._id == off_src: 
                        ctrl_src = ctrl
                        break
                if ctrl_src == None:
                    raise KeyError(f'No matching offset source {off_src} for {self._id}')
                off_val = ctrl_src.get_variable_access()
            return f"{self.get_variable_access()} += {off_off}+{off_scale}*{off_val};"

    def get_clip_string(self):
        """
        Get the clipping apply string if there is one for this control
        """
        try: #apply clipping on inputs
            clipping = self.config['clip']
            clipping = list(map(lambda x: str(x) if x!= None else x, clipping))
            clipping = list(map(lambda x: x if (x == None or '.' in x) else f'{x}.0', clipping))
        except KeyError: pass
        else:
            if not None in clipping:
                return f"{self.get_variable_access()} = clamp({self.get_variable_access()}, {clipping[0]}f, {clipping[1]}f);"
            elif clipping[0] == None:
                return f"{self.get_variable_access()} = min({self.get_variable_access()}, {clipping[1]}f);"
            elif clipping[1] == None:
                return f"{self.get_variable_access()} = max({self.get_variable_access()}, {clipping[0]}f);"

class Panel():
    
    def __init__(self, infile):
        self.infile = infile
        self.tree = tree = et.parse(infile)
        self.controls = []
        self.cleantree = None

        self.metadata = Metadata()

        self.modname = infile[:-4]
        self.outfile = 'clean_'+infile

        if self.infile[:6] == 'proto_':
            self.outfile = self.infile[6:]
            self.modname = self.modname[6:]


    @staticmethod
    def is_path(node):
        return node.tag[-4:].lower() == 'path'

    @staticmethod
    def get_layer(node, old_layer):
        if node.tag[-1] != 'g': return old_layer
        prefix = '{http://www.inkscape.org/namespaces/inkscape}'
        if node.get(f'{prefix}groupmode', None) != 'layer': 
            return old_layer
        try:
            return node.get(f'{prefix}label')
        except:
            print(f"Couldn't get layer name from {node.items()}")
            return old_layer 


    def process_all(self, root = None, first=False, layer=None):
        """
        Find and process every path in the file
        """
        if first or root == None:
            self.controls = []
            self.cleantree = et.parse(self.infile)
            root = self.cleantree.getroot()
            self.width=float(root.get('width'))
            ns = root.tag[1:root.tag.index('}')]
            nsn = root.tag[root.tag.index('}')+1:]
            et.register_namespace('', ns)

            node_layer = Panel.get_layer(root, layer)
            self._process_node(None, root, layer=node_layer)
            self.post_process(root)
            return

    def _delete_layer(self, root, node = None, layer=None):
        """
        The actual recursion function for processing nodes
        """
        node_layer = Panel.get_layer(node, layer)
        if root!= None and node_layer!=None and not self.metadata.is_keep(node_layer):
            print(f'  {node_layer}')
            root.remove(node)
            return

        for subnode in node.getchildren():
            self._delete_layer(node, subnode)



    def _process_node(self, root, node, layer):
        """
        The actual recursion function for processing nodes
        """
        node_layer = Panel.get_layer(node, layer)
        self.metadata.add_node(node, node_layer)
        if Panel.is_path(node): 
            res = self.process(node, node_layer)
            if res and root != None:
                root.remove(node)

        for subnode in node.getchildren():
            self._process_node(node, subnode, layer=node_layer)


    def post_process(self, root):
        """
        Anything that needs to be done after loading everything
        """
        print('Deleting layers:')
        self._delete_layer(None, root)
        for ctrl in self.controls:
            self.metadata.apply_class(ctrl)

    def process(self, node, layer):
        """
        Check to see if a path node represents a control. If so, 
        populate the controls list and return True. Else return
        False
        """
        if Control.is_control(node, layer):
            self.controls.append(Control(node, layer))
            return True
        return False

    def get_instantiation_block(self):
        """
        Return the entire panel instantiation block for this panel
        """
        result = [Control.create_declarations]
        for ctrl in self.controls:
            result.append(ctrl.get_instantiation(self.modname))

        return '\n'.join(result)

    def get_enum_block(self):
        """
        Get the enuemrator block:
 	enum ParamIds {
            ENUMS(NAME, #),
            NUM_PARAMS
	};
        ...
        """

        lines = {k: [] for k in Control.enum_kind_map.values()}
        for ctrl in self.controls:
            key, text = ctrl.get_enum(self.controls)
            if key == None: continue
            lines[key].append(' '*8+text)

        for key, val in lines.items():
            if len(val) > 0:
                lines[key] = ',\n'.join(sorted(val))+','
            else:
                lines[key] = ''

        return Control.enums_template.format(**lines)

    def get_module_instance(self):
        """
        Create the module instantiation line
        """
        return module_template.instance.format(
            plugin = 'TechTechTechnologies',
            modname = self.modname,
            modname_full = self.modname,
            width = self.width/15.,
            tags = self.metadata.get_tags()
            )

    def get_module_src(self):
        """
        Use module_template to generate the base source file for the module.
        This only needs to happen the first time as any autogenerated code will
        be included from separate files
        """
        return module_template.module.format(
            plugin = 'TechTechTechnologies',
            modname = self.modname,
            modname_full = self.modname,
            width = self.width/15.,
            tags = self.metadata.get_tags()
            )

    def get_input_block(self):
        result = ['/* Declare vars */']
        for ctrl in self.controls:
            line = ctrl.get_declaration_string()
            if line != None: result.append(line)

        result.extend(['','/* Read params */',''])
        
        for ctrl in self.controls:
            line = ctrl.get_param_string(self.controls)
            if line != None: result.append(line)


        result.extend(['','/* Read inputs */',''])
        for ctrl in self.controls:
            line = ctrl.get_input_string(self.controls)
            if line != None: result.append(line)

        return '\n'.join(result)

    def get_output_block(self):

        result = ['','/* Write outputs */','']
        for ctrl in self.controls:
            line = ctrl.get_output_string(self.controls)
            if line != None: result.append(line)

        return '\n'.join(result)


    def write_headers(self, src_dir = '../src'):
        """
        write all the auxilliary header files for this module and the template
        if it doesn't exist
        """
        try:
            with open(os.path.join(src_dir,f'{self.modname}_panel.hpp'), 'w') as f:
                f.write(self.get_instantiation_block())
        except Exception as e:
            print(f'Failed to generate instantiation block\n{e}')

        try:
            with open(os.path.join(src_dir,f'{self.modname}_enums.hpp'), 'w') as f:
                f.write(self.get_enum_block())
        except Exception as e:
            print(f'Failed to generate enum block\n{e}')

        try:
            with open(os.path.join(src_dir,f'{self.modname}_vars.hpp'), 'w') as f:
                f.write(self.get_vars_block())
        except Exception as e:
            print(f'Failed to generate vars block\n{e}')

        try:
            with open(os.path.join(src_dir,f'{self.modname}_inputs.hpp'), 'w') as f:
                f.write(self.get_input_block())
        except Exception as e:
            print(f'Failed to generate input block\n{e}')

        try:
            with open(os.path.join(src_dir,f'{self.modname}_outputs.hpp'), 'w') as f:
                f.write(self.get_output_block())
        except Exception as e:
            print(f'Failed to generate output block\n{e}')

        try:
            with open(os.path.join(src_dir,f'{self.modname}_instance.hpp'), 'w') as f:
                f.write(self.get_module_instance())
        except Exception as e:
            print(f'Failed to generate instance block\n{e}')



        filename = os.path.join(src_dir, f'{self.modname}.cpp')
        if not os.path.exists(filename):
            with open(filename, 'w') as f:
                f.write(self.get_module_src())




    def write_clean_file(self, outfile = None):
        if outfile == None:
            outfile = self.outfile
        assert outfile != infile

        self.cleantree.write(outfile)

    def __repr__(self):
        return f'Panel for {self.modname} - {len(self.controls)} controls'

if __name__ == '__main__':
    infile = sys.argv[1]
    panel = Panel(infile)
    panel.process_all()
    print(panel)
    panel.write_clean_file()
    panel.write_headers()

