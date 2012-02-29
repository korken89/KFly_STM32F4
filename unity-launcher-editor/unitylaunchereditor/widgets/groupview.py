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
from gtk.gdk import Pixbuf
import gobject
from pango import ELLIPSIZE_END
from re import sub
from unitylaunchereditor.core.translation import _


class GroupView(gtk.TreeView):
    (COLUMN_GROUP,
     COLUMN_NAME,
     COLUMN_EXEC,
     COLUMN_SHOWIN,
     COLUMN_VISIBLE) = range(5)
     
    __gsignals__ = {
                    "selection-changed" : (gobject.SIGNAL_RUN_LAST,
                                           gobject.TYPE_NONE,
                                           (gobject.TYPE_PYOBJECT,)),

                    }
    
    def __init__(self):
        super(GroupView, self).__init__()
        self.set_headers_visible(True)
        self._setup_model()
        self._setup_columns()
        self.get_selection().set_mode(gtk.SELECTION_SINGLE)
        self.set_reorderable(True)
        
        self.connect('cursor-changed', self._on_row_change)
    
    def _setup_columns(self):
        
        render = gtk.CellRendererText()
        
        column = gtk.TreeViewColumn(_('Group Name'))
        column.pack_start(render, expand=True)
        column.add_attribute(render, 'markup', self.COLUMN_GROUP)
        self.append_column(column)
        
        cell_renderer = gtk.CellRendererText();
        col = gtk.TreeViewColumn(_('Name'), cell_renderer, text=self.COLUMN_NAME);
        col.set_resizable(True)
        self.append_column(col)
        
        cell_renderer = gtk.CellRendererText();
        cell_renderer.set_property('ellipsize', ELLIPSIZE_END)
        col = gtk.TreeViewColumn(_('Command'));
        col.pack_start(cell_renderer, expand=True)
        col.add_attribute(cell_renderer, 'text', self.COLUMN_EXEC)
        col.set_resizable(True)
        self.append_column(col)
        
        render_check = gtk.CellRendererToggle()
        col = gtk.TreeViewColumn(_('Visible'), render_check, active=self.COLUMN_VISIBLE);
        col.set_alignment(0.1)
        #render_check.connect('toggled', self._sticky_toggled)
        self.append_column(col)
        
        cell = gtk.CellRendererCombo()
        #cell.set_property("editable", True)
        cell.set_property("has-entry", False)
        cell.set_property("model", self.unity_model)
        cell.set_property("text-column", 0)
        cell.connect('changed', self.action_class_changed)
        column = gtk.TreeViewColumn(_("Environment"))
        column.pack_start(cell, True)
        column.set_attributes(cell, text = self.COLUMN_SHOWIN)
        self.append_column(column)
    
    def _sticky_toggled(self, cell, path):
        model = self.get_model()
        treeiter = model.get_iter((int(path),))
        item = model.get_value(treeiter, self.COLUMN_VISIBLE)
        item = not item
        model.set(treeiter, self.COLUMN_VISIBLE, item)
        
    def action_class_changed(self, combo, path, it):
        m = combo.props.model
        ntype = m.get_value(it, 0)
        self.get_model()[path][self.COLUMN_SHOWIN] = ntype
            
    def _setup_model(self):
        model = gtk.ListStore(gobject.TYPE_STRING,
                              gobject.TYPE_STRING,
                              gobject.TYPE_STRING,
                              gobject.TYPE_STRING,
                              gobject.TYPE_BOOLEAN)
        
        old_model = self.get_model()
        
        if old_model:
            del old_model
        self.set_model(model)
        
        self.unity_model = gtk.ListStore(str)
        self.unity_model.append(["Unity"])
        self.unity_model.append(["Message Menu"])
        self.unity_model.append(["Unity;Message Menu"])
    def get_rows(self):
        groups = []
        for row_iter in self.get_model():
                groups.append({
                                 "GroupName": row_iter[self.COLUMN_GROUP],
                                 "Name": row_iter[self.COLUMN_NAME],
                                 "Exec": row_iter[self.COLUMN_EXEC],
                                 "TargetEnvironment":  row_iter[self.COLUMN_SHOWIN],
                                 "Visible":row_iter[self.COLUMN_VISIBLE]
                                 })
        
        return groups
    
    def update_current_row(self,values):
        self.set_value(values['Name'], self.COLUMN_NAME)
        self.set_value(values['Exec'], self.COLUMN_EXEC)
        self.set_value(values['TargetEnvironment'], self.COLUMN_SHOWIN)
        self.set_value(values['Visible'], self.COLUMN_VISIBLE)
    
    def set_value(self, value, column):
        selection = self.get_selection()
        model, row_iter = selection.get_selected()

        model[row_iter][column] = value
        
    def get_selected(self):
        selection = self.get_selection()
        gModel, titer = selection.get_selected()
        return titer
    
    def get_selected_row(self):
        titer = self.get_selected()
        return self.get_children_values(titer)
    
    def get_children_values(self, root_iter):
        model = self.get_model()
        text = model.get_value(root_iter, self.COLUMN_GROUP)
        text = sub('<.*?>', '', text)
        
        items = {'GroupName': text}
        items['Name'] = model.get_value(root_iter, self.COLUMN_NAME)
        items['Exec'] = model.get_value(root_iter, self.COLUMN_EXEC)
        items['TargetEnvironment']=model.get_value(root_iter, self.COLUMN_SHOWIN)
        items['Visible'] = model.get_value(root_iter, self.COLUMN_VISIBLE)
        return items
        
    def _on_row_change(self, data=None):
        try:
            root_iter = self.get_selected()
        except:
            root_iter = None
        
        self.emit("selection-changed", root_iter)
    
    def add_row(self, group_values):
        model = self.get_model()
        new_root = model.append(("<b>%s</b>" % group_values['GroupName'], group_values['Name'], group_values['Exec'], group_values['TargetEnvironment'], group_values['Visible']))
        return new_root
    
    def clear(self):
        self.get_model().clear()
    
    def remove_selected_row(self):
        try:
            self.get_model().remove(self.get_selected())
            return True
        except:
            return False
        
    def remove_row(self, titer):
        model = self.get_model()
        model.remove(titer)
        
