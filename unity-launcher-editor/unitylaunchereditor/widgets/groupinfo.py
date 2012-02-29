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
import gobject
from gio import File,FileIcon,ThemedIcon
from unitylaunchereditor.core.iconmanager import IconManager
from unitylaunchereditor.core.constants import IMAGE_SIZE
from unitylaunchereditor.dialogs.dialogs import FileChooser, MessageBox
from unitylaunchereditor.core.translation import _
from unitylaunchereditor.core.log import Logger

log = Logger('GroupInfo')

class GroupInfo(gtk.HBox):
    __gsignals__ = { 'field-changed': (
                                       gobject.SIGNAL_RUN_LAST, 
                                       gobject.TYPE_NONE,
                                       (gobject.TYPE_STRING,),
                                        ),
        }
    
    def __init__(self,edit_group=False):
        super(GroupInfo,self).__init__()
        self.set_border_width(12)
        self.editing = edit_group
        self.__group_name =""
        self.__command=""
        self.__name=""
        self.__visible = False

        self.__create_text_frame()
        
    @property
    def quicklist_command(self):
        return self.__command
    @quicklist_command.setter
    def quicklist_command(self,value):
        self.__command = value

    @property
    def quicklist_name(self):
        return self.__name
    @quicklist_name.setter
    def quicklist_name(self,value):
            self.__name= value
    
    @property
    def quicklist_group_name(self):
        return self.__group_name
    @quicklist_group_name.setter   
    def quicklist_group_name(self,value):
        self.__group_name = value   
    
    @property
    def quicklist_visible(self):
        return self.__visible
    @quicklist_visible.setter
    def quicklist_visible(self,value):
            self.__visible = value
            
    def __create_text_frame(self):
        
        label_gname = gtk.Label(_('Group Name:'))
        label_gname.set_alignment(xalign=0, yalign=0)
        self.entry_gname = gtk.Entry()
        self.entry_gname.set_sensitive(not self.editing)
        self.entry_gname.connect('changed', self._on_change)
        self.entry_gname.connect('focus-out-event', self.__on_txt_focus_out, 'quicklist_group_name')
        
        
        label_name = gtk.Label(_('Launcher Name:'))
        label_name.set_alignment(xalign=0, yalign=0)
        self.entry_name = gtk.Entry()
        #entry_name.set_name()
        self.entry_name.connect('focus-out-event', self.__on_txt_focus_out, 'quicklist_name')
        self.entry_name.connect('changed', self._on_change)
        
        label_command = gtk.Label(_('Command:'))
        label_command.set_alignment(xalign=0, yalign=0)
        self.entry_command = gtk.Entry()
        self.entry_command.connect('focus-out-event', self.__on_txt_focus_out, 'quicklist_command')
        self.entry_command.connect('changed', self._on_change)
        
        button_command = gtk.Button('...')
        hbox = gtk.HBox()
        hbox.pack_start(self.entry_command)
        hbox.pack_start(button_command, expand=False, fill=True, padding=0)
        button_command.connect('clicked', self.__do_choose_run, self.entry_command)
        
        self.combo_label= gtk.Label('Environment:')
        self.combo_label.set_alignment(xalign=0, yalign=0)
        self.cmbTargetEnvironment = gtk.ComboBox()
        self.cmbTargetEnvironment.set_name('TargetEnvironment')
        
        self.combo_model = gtk.ListStore(gobject.TYPE_STRING)
        for target_key in ['Unity', 'Message Menu', 'Unity;Message Menu']:
            self.combo_model.append([target_key])

        celltitle = gtk.CellRendererText()

        self.cmbTargetEnvironment.pack_start(celltitle, False)
        self.cmbTargetEnvironment.add_attribute(celltitle, 'text', 0)
        self.cmbTargetEnvironment.set_model(self.combo_model)
        self.cmbTargetEnvironment.set_active(0)

        self.chkVisible = gtk.CheckButton()
        self.chkVisible.set_label('Visible')
        
        table = gtk.Table(rows=5, columns=2, homogeneous=False)
        table.set_col_spacings(5)
        table.set_row_spacings(5)
        
        table.attach(label_gname, 
                     left_attach=0, 
                     right_attach=1, 
                     top_attach=0, 
                     bottom_attach=1, 
                     xoptions=gtk.FILL, 
                     yoptions=gtk.FILL, 
                     xpadding=0, 
                     ypadding=0)

        table.attach(self.entry_gname, 
                     left_attach=1, 
                     right_attach=2, 
                     top_attach=0, 
                     bottom_attach=1, 
                     xoptions=gtk.FILL|gtk.EXPAND , 
                     yoptions=gtk.FILL, 
                     xpadding=0, 
                     ypadding=0
                     )
        
        table.attach(label_name, 
                     left_attach=0, 
                     right_attach=1, 
                     top_attach=1, 
                     bottom_attach=2, 
                     xoptions=gtk.FILL, 
                     yoptions=gtk.FILL, 
                     xpadding=0, 
                     ypadding=0)

        table.attach(self.entry_name, 
                     left_attach=1, 
                     right_attach=2, 
                     top_attach=1, 
                     bottom_attach=2, 
                     xoptions=gtk.FILL|gtk.EXPAND , 
                     yoptions=gtk.FILL, 
                     xpadding=0, 
                     ypadding=0
                     )
        
        table.attach(label_command, 
                     left_attach=0, 
                     right_attach=1, 
                     top_attach=2, 
                     bottom_attach=3, 
                     xoptions=gtk.FILL, 
                     yoptions=gtk.FILL, 
                     xpadding=0, 
                     ypadding=0)
        
        table.attach(hbox, 
                     left_attach=1, 
                     right_attach=2, 
                     top_attach=2, 
                     bottom_attach=3, 
                     xoptions=gtk.FILL|gtk.EXPAND , 
                     yoptions=gtk.FILL, 
                     xpadding=0, 
                     ypadding=0)
        
        table.attach(self.combo_label, 
                     left_attach=0, 
                     right_attach=1, 
                     top_attach=3, 
                     bottom_attach=4, 
                     xoptions=gtk.FILL, 
                     yoptions=gtk.FILL, 
                     xpadding=0, 
                     ypadding=0)
        
        table.attach(self.cmbTargetEnvironment, 
                     left_attach=1, 
                     right_attach=2, 
                     top_attach=3, 
                     bottom_attach=4, 
                     xoptions=gtk.FILL, 
                     yoptions=gtk.FILL, 
                     xpadding=0, 
                     ypadding=0)
        
        table.attach(gtk.Label(), 
                     left_attach=0, 
                     right_attach=1, 
                     top_attach=4, 
                     bottom_attach=5, 
                     xoptions=gtk.FILL, 
                     yoptions=gtk.FILL, 
                     xpadding=0, 
                     ypadding=0)
        
        table.attach(self.chkVisible, 
                     left_attach=1, 
                     right_attach=2, 
                     top_attach=4, 
                     bottom_attach=5, 
                     xoptions=gtk.FILL|gtk.EXPAND, 
                     yoptions=gtk.FILL, 
                     xpadding=0, 
                     ypadding=0)

        frame = gtk.Frame()
        frame.set_shadow_type(gtk.SHADOW_NONE)
        frame.set_label_align(xalign=0, yalign=0)
        frame.add(table)
        
        self.pack_start(frame, expand=True, fill=True, padding=0)

    def _on_change(self, widget):
        self.emit('field-changed',widget.get_text())
        
    def __on_txt_focus_out(self, widget, event, prop_name):
        ilength = 0
        if isinstance(widget, gtk.CheckButton):
            print getattr(self, prop_name)
            value = widget.get_active()
            ilength = 1
        else:
            ilength = widget.get_text_length() 
            value = widget.get_text()
        
        old = getattr(self, prop_name)
        
        if ilength > 0 and old != value:
            setattr(self, prop_name, value)
            log.info(prop_name)
            
    def __do_choose_run(self, widget, entry_widget):
        dialog = FileChooser()
        response, filename = dialog.run()
        if response:
            self.quicklist_command=filename
            if entry_widget:
                entry_widget.set_text(filename)