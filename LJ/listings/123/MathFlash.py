#!/usr/bin/env python
    
#----------------------------------------------------------------------
# MathFlash.py
# Dave Reed
# 02/12/2004
#----------------------------------------------------------------------

import sys

from random import randrange
from GladeWindow import *

#----------------------------------------------------------------------

def get_random_number(digits):

    num = 0
    for i in range(digits):
        d = randrange(10)
        num = num * 10 + d
    return num

#----------------------------------------------------------------------

class MathFlash(GladeWindow):

    #----------------------------------------------------------------------

    def __init__(self):

        ''' '''
        
        self.init()
        self.widgets['plus_check'].set_active(1)
        self.correct = 0
        self.total = 0
        self.show_results()
        
    #----------------------------------------------------------------------

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
        GladeWindow.__init__(self, filename, top_window, widget_list, handlers)

    #----------------------------------------------------------------------

    def show_results(self):

        if self.total > 0:
            pct = '%0.1f%%' % (100.0 * self.correct / float(self.total))
        else:
            pct = ''

        self.widgets['correct_entry'].set_text(str(self.correct))
        self.widgets['wrong_entry'].set_text(str(self.total - self.correct))
        self.widgets['pct_entry'].set_text(pct)

    #----------------------------------------------------------------------

    def on_operator_check_toggled(self, *args):

        self.operators = []
        if self.widgets['plus_check'].get_active():
            self.operators.append('+')
        if self.widgets['minus_check'].get_active():
            self.operators.append('-')
        if self.widgets['multiply_check'].get_active():
            self.operators.append('*')
        if self.widgets['divide_check'].get_active():
            self.operators.append('/')

    #----------------------------------------------------------------------

    def on_submit_button_clicked(self, *args):

        prob = self.widgets['problem_entry'].get_text()
        ans = eval(prob)
        user = int(self.widgets['answer_entry'].get_text())
        self.total += 1
        if ans == user:
            self.correct += 1
            self.widgets['result_entry'].set_text('Correct')
        else:
            self.widgets['result_entry'].set_text('Wrong, the answer is %d'
                                                  % ans)
        self.show_results()
        
    #----------------------------------------------------------------------

    def on_reset_button_clicked(self, *args):

        self.correct = 0
        self.total = 0
        self.show_results()
        
    #----------------------------------------------------------------------

    def on_new_button_clicked(self, *args):

        num_digits = int(self.widgets['digits_spin'].get_text())
        num_ops = int(self.widgets['operators_spin'].get_text())

        prob = []
        num = get_random_number(num_digits)
        prob.append(str(num))
        for i in range(num_ops):
            op = self.operators[randrange(len(self.operators))]
            prob.append(op)
            num = get_random_number(num_digits)
            prob.append(str(num))
        self.widgets['problem_entry'].set_text(''.join(prob))
        self.widgets['result_entry'].set_text('')
        self.widgets['answer_entry'].set_text('')
        
    #----------------------------------------------------------------------

    def on_exit_button_clicked(self, *args):

        gtk.main_quit()


#----------------------------------------------------------------------

def main(argv):

    w = MathFlash()
    w.show()
    gtk.main()

#----------------------------------------------------------------------

if __name__ == '__main__':
    main(sys.argv)
