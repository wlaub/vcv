import xml.etree.ElementTree as et
import sys
import ast
from svgpathtools.parser import parse_path

class Control():
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
    def is_control(node):
        """
        Is a control if the label parameter is one of param, input, or output
        """
        for key,val in node.items():
            if Control.is_label(key):
                if val.lower() in ['param', 'input', 'output']:
                    return True
        return False

    def __init__(self, node):
        """
        self.kind from the path label describes the type of control
        self._id from the path id is the enum name excluding _PARAM/INPUT/etc
        self.widget from the path title is the widget class name
        self.config is a python variable in the path description (dict)
        self.pos is [x,y] location of the center of the control
        """
        for key, val in node.items():
            if Control.is_label(key):
                self.kind = val.lower()

        self._id = node.get('id').upper()
        d = node.get('d')

        self.path = parse_path(d)
       
        bbox = self.path.bbox()
        self.pos = [(bbox[i*2]+bbox[i*2+1])/2 for i in range(2)]

        self.config = {}
        for subnode in node.getchildren():
            if Control.is_title(subnode):
                self.widget = subnode.text
            elif Control.is_desc(subnode):
                self.config = ast.literal_eval(subnode.text)


class Panel():
    
    def __init__(self, infile):
        self.infile = infile
        self.tree = tree = et.parse(infile)
        self.controls = []
        self.cleantree = None

    @staticmethod
    def is_path(node):
        return node.tag[-4:].lower() == 'path'

    def process_all(self, root = None, first=False):
        """
        Find an process every path in the file
        """
        if first or root == None:
            self.controls = []
            self.cleantree = et.parse(self.infile)
            root = self.cleantree.getroot()
        
        for node in root.getchildren():
            if Panel.is_path(node): 
                res = self.process(node)
                if res:
                    root.remove(node)
            else:
                self.process_all(node)

    def process(self, node):
        """
        Check to see if a path node represents a control. If so, 
        populate the controls list and return True. Else return
        False
        """
        if Control.is_control(node):
            self.controls.append(Control(node))
            return True
        return False

if __name__ == '__main__':
    infile = sys.argv[1]
    panel = Panel(infile)
    panel.process_all()
    
