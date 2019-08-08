#!/usr/bin/env python

#----------------------------------------------------------------------
# GladeGen.py
# Dave Reed
# 02/03/2004
#----------------------------------------------------------------------

import sys
import os.path
import time

import re
import inspect

import xml.dom.minidom

import GladeGenConfig

#----------------------------------------------------------------------

class GladeGen:

    #------------------------------------------------------------------

    def __init__(self, module, glade_file, class_name):

        '''__init__(self, module, glade_file, class_name):

        module: file name without .py exentsion
        glade_file: full name of Glade XML file
        class_name: name of class to create
        '''
        
        self.module = module
        self.glade_file = glade_file
        self.class_name = class_name
        self.filename = '%s.py' % module

        # dict of function objects in the file
        # maps name to function object
        self.functions = {}

        # dict of class objects in the file
        # maps name to class object
        self.classes = {}

        # list of class methods in the file
        self.methods = []

        # widgets to be able to acces - see GladeGenConfig.include_widget_types
        # for which types from the file will be included
        self.widgets = []

        # handlers specified in the XML file
        self.handlers = []

        # contains all lines in file
        self.lines = []

        # contains handler methods that need to be added to .py file
        self.missing = []

        # line number where init (not __init__) methods starts
        self.init_start = None        
        
        # create file if doesn't exist
        if not os.path.exists(self.filename):
            f = file(self.filename, 'w')
            f.write(self.create_file())
            f.close()

        # read existing file into self.lines
        f = file(self.filename, 'r')
        self.lines = f.readlines()
        f.close()
        
    #------------------------------------------------------------------

    def create_file(self):

        '''create_file(self):

        create empty python file for the class'''
        
        lines = []
        lines.append(self.create_header())
        lines.append(GladeGenConfig.class_header % self.class_name)
        lines.append(GladeGenConfig.constructor)
        lines.append(self.create_footer())
        return '\n'.join(lines)
        
    #------------------------------------------------------------------

    def get_members(self):

        '''get_members(self):

        get information on functions, classes, methods, handlers, etc.
        from the Python code'''

        self.functions = {}
        self.classes = {}
        self.methods = []
        self.module = __import__(self.module)

        # functions
        l = inspect.getmembers(self.module, inspect.isroutine)
        for name, obj in l:
            self.functions[name] = obj

        # classes
        l = inspect.getmembers(self.module, inspect.isclass)
        for name, obj in l:
            self.classes[name] = obj

            # methods in the class
            methods = inspect.getmembers(obj, inspect.ismethod)
            for m in methods:
                self.methods.append((name, m[0]))
                # find init method (not __init__) since we will be replacing
                # it with a new method each time
                if m[0] == 'init':
                    self.init_lines, self.init_start = \
                                     inspect.getsourcelines(m[1])

    #----------------------------------------------------------------------

    def get_xml(self):

        '''get_xml(self):

        parse the XML file to get widget and handler names'''

        self.widgets = []
        self.handlers = []
        self.top_window = None

        # load XML file
        doc = xml.dom.minidom.parse(self.glade_file)

        # look for widgets and get their widget type and name
        for node in doc.getElementsByTagName('widget'):
            widget = str(node.getAttribute('class'))
            name = str(node.getAttribute('id'))
            if self.top_window is None and widget == 'GtkWindow':
                self.top_window = name

            # if the widget type is in list of widgets user specified
            # in config file, include it in the list
            
            if widget in GladeGenConfig.include_widget_types:
                # (widget type, name)
                # ('GtkWindow', 'window1')
                self.widgets.append((widget, name))

        # get signal handler names
        for node in doc.getElementsByTagName('signal'):
            name = str(node.getAttribute('handler'))
            if name not in self.handlers:
                self.handlers.append(name)
            
    #----------------------------------------------------------------------

    def get_missing_methods(self):

        '''get_missing_methods(self):

        determine which handlers do not exist in the Python file'''
        
        self.missing = []
        method_names = map(lambda x: x[1], self.methods)
        for h in self.handlers:
            if h not in method_names:
                self.missing.append(h)
            
    #----------------------------------------------------------------------

    def get_init_method(self, indent):

        '''get_init_method(self, indent):

        create the init (not __init__) method'''

        indent1 = ' ' * indent
        indent2 = '  ' * indent
        indent3 = '   ' * indent
        lines = []

        # method name
        lines.append('%sdef init(self):' % indent1)
        lines.append('')

        # specify Glade XML file
        lines.append("%sfilename = '%s'" % (indent2, self.glade_file))
        lines.append('')

        # specify widgets the user wnats
        lines.append('%swidget_list = [' % indent2)
        for w in self.widgets:
            lines.append("%s'%s'," % (indent3, w[1]))
        lines.append('%s]' % indent3)
        lines.append('')

        # specify handlers
        lines.append('%shandlers = [' % indent2)
        for h in self.handlers:
            lines.append("%s'%s'," % (indent3, h))
        lines.append('%s]' % indent3)
        lines.append('')

        # specify  the top level window widget
        lines.append("%stop_window = '%s'" % (indent2, self.top_window))

        # call the subclass constructor
        lines.append('%sGladeWindow.__init__(self, %s, %s, %s, %s)' % (
            indent2, 'filename', 'top_window', 'widget_list', 'handlers'))

        lines.append('')
        return '\n'.join(lines)
        
    #----------------------------------------------------------------------

    def insert_missing_methods(self):

        '''insert_missing_methods(self):

        insert the missing handlers in the Python code stored in
        self.lines

        also updates the init (not __init__) method'''

        # get lines for the class object
        obj = self.classes[self.class_name]
        lines, start = inspect.getsourcelines(obj)
        end = start + len(lines)
        
        # determine indentation level for class methods
        for l in lines:
            if re.search('\sdef', l) is not None:
                indent = l.find('def')

        # remove existing init method
        s_pos = self.init_start - start
        e_pos = s_pos + len(self.init_lines)
        if self.init_start is not None:
            del lines[s_pos:e_pos]

        # insert new init (not __init__ method)
        lines.insert(s_pos, self.get_init_method(indent))

        # insert code for missing functions
        for m in self.missing:
            lines.append('%s#%s\n' % (indent * ' ', 70 * '-'))
            lines.append('\n')
            lines.append('%sdef %s(self, *args):\n' % (indent * ' ', m))
            lines.append('%spass\n' % (2 * indent * ' '))
            lines.append('\n')
        lines.append('\n')

        # delete class from self.lines
        del self.lines[start-1:end]

        # replace with updated class stored in lines
        self.lines.insert(start-1, ''.join(lines))

        return self.lines
                  
    #----------------------------------------------------------------------

    def print_info(self):

        '''print_info(self):

        print stored information - useful for debugging'''

        print 'funcs'
        for k in self.functions:
            print k, self.functions[k]
        print
        
        print 'classes'
        for k in self.classes:
            print k, self.classes[k]
        print
        
        print 'methods'
        for k in self.methods:
            print k
        print

        print 'widgets'
        for k in self.widgets:
            print k
        print

        print 'handlers'
        for k in self.handlers:
            print k
        print    

    #------------------------------------------------------------------
    #------------------------------------------------------------------

    def create_header(self):

        '''create_header(self):

        return header lines for Python file'''
        
        date = time.strftime(GladeGenConfig.date_format)
        header = '''#!/usr/bin/env python
    
#----------------------------------------------------------------------
# %s
# %s
# %s
#----------------------------------------------------------------------

import sys

from GladeWindow import *

#----------------------------------------------------------------------
''' % (self.filename, GladeGenConfig.author, date)

        return header
    
    #------------------------------------------------------------------

    def create_footer(self):

        '''return footer lines for Python file'''

        footer = """
#----------------------------------------------------------------------

def main(argv):

    w = %s()
    w.show()
    gtk.main()

#----------------------------------------------------------------------

if __name__ == '__main__':
    main(sys.argv)
""" % self.class_name

        return footer        
    
    #------------------------------------------------------------------

#----------------------------------------------------------------------

def main(argv):

    if len(argv) != 4:
        print 'usage: GladeGen.py glade_file module_name class_name'
        return
    
    glade_file = argv[1]
    module = argv[2]
    class_name = argv[3]

    pg = GladeGen(module, glade_file, class_name)
    pg.get_members()
    pg.get_xml()

    pg.print_info()
    pg.get_missing_methods()
    lines = ''.join(pg.insert_missing_methods())

    f = file('%s.py' % module, 'w')
    f.write(lines)
    f.close()    

#----------------------------------------------------------------------

if __name__ == '__main__':
    main(sys.argv)
