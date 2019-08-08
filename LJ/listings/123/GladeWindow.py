#!/usr/bin/env python

#----------------------------------------------------------------------
# GladeWindow.py
# Dave Reed
# 12/15/2002
#----------------------------------------------------------------------

import os
import gtk
import gtk.glade

#----------------------------------------------------------------------

def search_file(filename, search_path):

    """Given a search path, find file
    """

    file_found = 0
    paths = search_path.split(os.pathsep)

    for path in paths:
        if os.path.exists(os.path.join(path, filename)):
            file_found = 1
            break

    if file_found:
        return os.path.abspath(os.path.join(path, filename))
    else:
        return None

#----------------------------------------------------------------------

class GladeWindow(object):

    '''A base class for displaying a GUI developed with Glade; create
    a subclass and add any callbacks and other code; the derived class
    __init__ method needs to call GladeWindow.__init__; callbacks that
    start with on_ are automatically connected'''

    #----------------------------------------------------------------------

    def set_search_path(cls, path):

        '''set the search path for looking for the .glade files'''

        cls.search_path = path

    set_search_path = classmethod(set_search_path)

    #----------------------------------------------------------------------

    def __init__(self, filename, top_window, widget_list, handlers,
                 pull_down_dict=None):

        '''
        __init__(self, filename, top_window, widget_list, pull_down_dict=None):

        filename: filename of the .glade file
        top_window: the glade name of the top level widget (this will then
           be accessible as self.top_window)
        widget_list: a list of glade names; the dictionary self.widgets
           will be created that maps these name to the actual widget object
        pull_down_dict: a dictionary that maps combo names to a tuple of
            strings to put in the combo
        '''
        
        self.widget_list = widget_list

        try:
            search_path = GladeWindow.search_path
        except:
            search_path = './'

        fname = search_file(filename, search_path)
        self.xml = gtk.glade.XML(fname)

        # connect callbacks
        self.cb_dict = {}
        for f in handlers:
            self.cb_dict[f] = getattr(self, f)
        self.xml.signal_autoconnect(self.cb_dict)

        self.widgets = {}
        for w in self.widget_list:
            self.widgets[w] = self.xml.get_widget(w)

        if pull_down_dict is not None:
            for w, l in pull_down_dict.items():
                self.widgets[w].set_popdown_strings(l)
            
        # set attribute for top_window so it can be accessed as self.top_window
        self.top_window = self.xml.get_widget(top_window)

        # window to show when this one is hidden
        self.prev_window = None

        # initialize callback func
        self.cb_func = None

    #----------------------------------------------------------------------

    def set_top_window(self, top_window):

        '''set_top_window(self, top_window):

        notebook pages that are in containers need to be able to change
        their top window, especially so the dialog is set_transient_for
        the actual main window
        '''
        
        self.top_window = top_window
        
    #----------------------------------------------------------------------

    def set_callback_function(self, cb_func, *cb_args, **cb_kwargs):

        '''set_callback_function(cb_func, *cb_args, **cb_kwargs):

        stores the cb_func and its cb_args and cb_kwargs
        '''
        self.cb_func = cb_func
        self.cb_args = cb_args
        self.cb_kwargs = cb_kwargs
        
    
    #----------------------------------------------------------------------

    def show(self, center=1, prev_window=None, *args):

        '''show(self, center=1, prev_window=None, *args):

        display the top_window widget
        '''

        if prev_window is not None:
            self.prev_window = prev_window
        if center:
            self.top_window.set_position(gtk.WIN_POS_CENTER_ALWAYS)
        else:
            self.top_window.set_position(gtk.WIN_POS_NONE)
        self.top_window.show()

    #----------------------------------------------------------------------

    def hide(self):

        '''hide(self):

        hides the current window, shows self.prev_window
        if self.cb_func is not None, it is called with its cb_args
        and cb_kwargs
        '''

        self.top_window.hide()
        if self.prev_window is not None:
            self.prev_window.show()
        if self.cb_func is not None:
            self.cb_func(*self.cb_args, **self.cb_kwargs)
        if self.prev_window is None:
            gtk.main_quit()
            
#----------------------------------------------------------------------
