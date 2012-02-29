# -*- coding: utf-8 -*-
#####################################################################
#  Laudeci Oliveira <laudeci@ubuntu.com>
#  Ursula Junque <ursinha@ubuntu.com>
#
#  Copyright 2011 unity-launcher-editor-dev.
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published
#  by the Free Software Foundation; version 3 only.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#####################################################################

import gtk
from gobject import TYPE_STRING
from utils import FileChooser
from translation import  _

class GroupsDialog(gtk.Dialog):
    """XXX: add docs."""

    def __init__(self, title=_('Add Group...'), parent=None):
        super(GroupsDialog, self).__init__(title,
            None, (gtk.DIALOG_MODAL | gtk.DIALOG_DESTROY_WITH_PARENT | 
                gtk.DIALOG_NO_SEPARATOR),
            (gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL))
        
        
        self.set_resizable(False)
        
        if parent:
            self.set_transient_for(parent)
            
        self.add_group_panel = self.create_widgets()
        self.vbox.pack_start(self.add_group_panel, True, True, 0)
        self.vbox.show_all()
        
        self.ok_button = self.add_button(gtk.STOCK_OK, gtk.RESPONSE_OK)
        self.ok_button.grab_default()
        self.ok_button.set_sensitive(False)
        self.set_alternative_button_order(
            [gtk.RESPONSE_CANCEL, gtk.RESPONSE_OK])
        
        self.txtName.connect('changed', self.on_change)
        self.txtCommand.connect('changed', self.on_change)
        self.cmdGRun.connect('clicked', self.open_executable)

    def create_widgets(self):
        '''
            this will create input widgets for our dialog
            without depending on glade for that and
            solving the file not found error report.
        '''
        
        frame4 = gtk.Frame()
        frame4.set_shadow_type(gtk.SHADOW_NONE)
        
        align = gtk.Alignment()
        align.set_padding(0, 0, 12, 0)
        
        table = gtk.Table(rows=4, columns=2, homogeneous=False)
        table.set_col_spacings(5)
        table.set_row_spacings(5)
        
        label8 = gtk.Label(_('Group Name:'))
        label8.set_alignment(0, 0.5)
        eventbox8 = gtk.EventBox()
        eventbox8.add(label8)
        
        table.attach(eventbox8,
                     left_attach=0,
                     right_attach=1,
                     top_attach=0,
                     bottom_attach=1,
                     xoptions=gtk.FILL,
                     yoptions=gtk.FILL,
                     xpadding=0,
                     ypadding=0)
        
        
        label5 = gtk.Label(_('Command Name:'))
        label5.set_alignment(0, 0.5)
        eventbox5 = gtk.EventBox()
        eventbox5.add(label5)
        table.attach(eventbox5,
                     left_attach=0,
                     right_attach=1,
                     top_attach=1,
                     bottom_attach=2,
                     xoptions=gtk.FILL,
                     yoptions=gtk.FILL,
                     xpadding=0,
                     ypadding=0)
        
        
        label6 = gtk.Label(_('Command:'))
        label6.set_alignment(0, 0.5)
        eventbox6 = gtk.EventBox()
        eventbox6.add(label6)
        table.attach(eventbox6,
                     left_attach=0,
                     right_attach=1,
                     top_attach=2,
                     bottom_attach=3,
                     xoptions=gtk.FILL,
                     yoptions=gtk.FILL,
                     xpadding=0,
                     ypadding=0)
        
        label7 = gtk.Label(_('Target Environment:'))
        label7.set_alignment(0, 0.5)
        eventbox7 = gtk.EventBox()
        eventbox7.add(label7)
        table.attach(eventbox7,
                     left_attach=0,
                     right_attach=1,
                     top_attach=3,
                     bottom_attach=4,
                     xoptions=gtk.FILL,
                     yoptions=gtk.FILL,
                     xpadding=0,
                     ypadding=0)
        
        self.txtGroup = gtk.Entry()
        table.attach(self.txtGroup,
                     left_attach=1,
                     right_attach=2,
                     top_attach=0,
                     bottom_attach=1,
                     xoptions=gtk.FILL,
                     yoptions=gtk.FILL,
                     xpadding=0,
                     ypadding=0)
        
        self.txtName = gtk.Entry()
        self.txtName.set_name('Name')
        table.attach(self.txtName,
                     left_attach=1,
                     right_attach=2,
                     top_attach=1,
                     bottom_attach=2,
                     xoptions=gtk.EXPAND | gtk.FILL,
                     #yoptions=gtk.None, 
                     xpadding=0,
                     ypadding=0)
        
        hbox4 = gtk.HBox()
        self.txtCommand = gtk.Entry()
        self.txtCommand.set_name('Exec')
        self.cmdGRun = gtk.Button('...')
        
        hbox4.pack_start(self.txtCommand, expand=True, fill=True, padding=0)
        hbox4.pack_start(self.cmdGRun, expand=False, fill=True, padding=0)
        
        table.attach(hbox4,
                     left_attach=1,
                     right_attach=2,
                     top_attach=2,
                     bottom_attach=3,
                     xoptions=gtk.EXPAND | gtk.FILL,
                     yoptions=gtk.EXPAND | gtk.FILL,
                     xpadding=0,
                     ypadding=0)
        
        self.cmbTargetEnvironment = gtk.ComboBox()
        self.cmbTargetEnvironment.set_name('TargetEnvironment')
        
        self.combo_model = gtk.ListStore(TYPE_STRING)
        for target_key in ['Unity', 'Message Menu', 'Unity;Message Menu']:
            self.combo_model.append([target_key])

        celltitle = gtk.CellRendererText()

        self.cmbTargetEnvironment.pack_start(celltitle, False)
        self.cmbTargetEnvironment.add_attribute(celltitle, 'text', 0)
        self.cmbTargetEnvironment.set_model(self.combo_model)
        self.cmbTargetEnvironment.set_active(0)
        
        table.attach(self.cmbTargetEnvironment,
                     left_attach=1,
                     right_attach=2,
                     top_attach=3,
                     bottom_attach=4,
                     xoptions=gtk.EXPAND | gtk.FILL,
                     yoptions=gtk.EXPAND | gtk.FILL,
                     xpadding=0,
                     ypadding=0)
        
        align.add(table)
        frame4.add(align)
        
        return frame4
            
    def populate(self, values):
        try:
            self.txtGroup.set_text(values['GroupName'])
            self.txtGroup.set_sensitive(False)
            self.txtName.set_text(values['Name'])
            self.txtCommand.set_text(values['Exec'])
        except:
            pass
        try:
            for n in range(len(self.combo_model)):
                if self.combo_model[n][0] == values['TargetEnvironment']:
                    self.cmbTargetEnvironment.set_active(n)
        except:
            pass
        
    def run(self, destroy=True):
        """Returns True if yes was clicked, False otherwise."""
        resp = super(GroupsDialog, self).run()
        obj = None
        if resp == gtk.RESPONSE_OK:
            obj = {'GroupName': self.txtGroup.get_text(),
                   'Name': self.txtName.get_text(),
                   'Exec': self.txtCommand.get_text(),
                   'TargetEnvironment': (
                      self.cmbTargetEnvironment.get_active_text()),
            }
            print obj            
        if destroy:
            self.destroy()
        if resp == gtk.RESPONSE_OK:
            return True, obj
        else:
            return False, None

    def on_change(self, widget):
        if (self.txtName.get_text_length() == 0 or 
            self.txtCommand.get_text_length() == 0):
            self.ok_button.set_sensitive(False)
        else:
            self.ok_button.set_sensitive(True)

    def open_executable(self, widget):
        dialog = FileChooser()
        response, filename = dialog.run()
        if response:
            self.txtCommand.set_text(filename)

