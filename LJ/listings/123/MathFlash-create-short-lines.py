#!/usr/bin/env python
    
#--------------------------------------------
# MathFlash.py
# Dave Reed
# 02/28/2004
#--------------------------------------------

import sys

from GladeWindow import *

#--------------------------------------------

class MathFlash(GladeWindow):

    #----------------------------------------

    def __init__(self):

        ''' '''
        
        self.init()

    #----------------------------------------

    def init(self):

        filename = 'mathflash.glade'

        widget_list = [
            'window1',
            'plus_check',
            'minus_check',
            'multiply_check',
            'divide_check',
            'digits_spin',
            'operators_spin',
            'correct_entry',
            'wrong_entry',
            'pct_entry',
            'problem_entry',
            'answer_entry',
            'submit_button',
            'reset_button',
            'exit_button',
            'new_button',
            'result_entry',
            ]

        handlers = [
            'on_operator_check_toggled',
            'on_submit_button_clicked',
            'on_reset_button_clicked',
            'on_exit_button_clicked',
            'on_new_button_clicked',
            ]

        top_window = 'window1'
        GladeWindow.__init__(self, filename,
                             top_window,
                             widget_list,
                             handlers)

    #----------------------------------------

    def on_operator_check_toggled(self, *args):
        pass

    #----------------------------------------

    def on_submit_button_clicked(self, *args):
        pass

    #----------------------------------------

    def on_reset_button_clicked(self, *args):
        pass

    #----------------------------------------

    def on_exit_button_clicked(self, *args):
        pass

    #----------------------------------------

    def on_new_button_clicked(self, *args):
        pass

#----------------------------------------

def main(argv):

    w = MathFlash()
    w.show()
    gtk.main()

#----------------------------------------

if __name__ == '__main__':
    main(sys.argv)
