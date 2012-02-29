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
from pango import ELLIPSIZE_END,WRAP_WORD

from unitylaunchereditor.core.constants import IMAGE_SIZE
from unitylaunchereditor.core.translation import LOCALE
from unitylaunchereditor.core.log import Logger

log = Logger()

class LauncherView(gtk.TreeView):
    __gsignals__ = {
                    "selection-changed" : (gobject.SIGNAL_RUN_LAST,
                                           gobject.TYPE_NONE,
                                           (gobject.TYPE_PYOBJECT,
                                            gobject.TYPE_PYOBJECT,
                                            gobject.TYPE_PYOBJECT),),

                    }
    # Constants for column mapping
    (COLUMN_ICON, COLUMN_NAME, COLUMN_DESKTOP, COLUMN_NAME_UPPER, COLUMN_PATH) = range(5)
    
    def __init__(self):
        super(LauncherView, self).__init__()
        log.loggername = __name__
        self.set_headers_visible(False)
        self.set_reorderable(True)
        #self.set_rules_hint(True)
        self.get_selection().set_mode(gtk.SELECTION_SINGLE)
        self.set_headers_visible(False)
       
        self.listmodel = None
        self._setup_columns()
        self._setup_model()
        
        # assign the tooltip
        #self.set_has_tooltip(True)
        #self.connect("query-tooltip", self._ViewTooltip)
        #self.set_events(gtk.gdk.POINTER_MOTION_MASK)

        self.connect('cursor-changed', self._on_row_change)
    
    def _setup_columns(self):
        #this renderer will show application icon
        render_pixbuf = gtk.CellRendererPixbuf()
        render_pixbuf.set_property('stock-size', gtk.ICON_SIZE_LARGE_TOOLBAR)
        render_pixbuf.set_property('yalign',0)
        render_pixbuf.set_property('ypad',2)
        
        #this will show application text
        #using 2 lines, one for application name and other for description
        #or command
        render_text = gtk.CellRendererText()
        render_text.set_property("ellipsize", ELLIPSIZE_END)
        render_text.set_property('wrap-mode',WRAP_WORD)
        render_text.set_property('wrap-width',500)
        
        
        column = gtk.TreeViewColumn()
        column.pack_start(render_pixbuf, expand=False)
        column.add_attribute(render_pixbuf, 'pixbuf', self.COLUMN_ICON)
        column.pack_start(render_text, expand=True)
        column.add_attribute(render_text, 'markup', self.COLUMN_NAME)
        self.append_column(column)
     
    def _setup_model(self):
        if self.listmodel:
            old_model = self.get_model()
        
            if old_model:
                del old_model
        
        self.listmodel = gtk.ListStore(Pixbuf,
                              gobject.TYPE_STRING,
                              gobject.TYPE_PYOBJECT,
                              gobject.TYPE_STRING,
                              gobject.TYPE_STRING
                              )
        self.set_model(self.listmodel)
        

    def refilter(self):
        self.filter.refilter()
         
    def enable_filter(self,visible_func):
        self.filter = self.get_model().filter_new()
        self.filter.set_visible_func(visible_func)
        self.sorter = gtk.TreeModelSort(self.filter)
        self.set_model(self.sorter)
        self.listmodel.set_sort_column_id(self.COLUMN_NAME_UPPER, gtk.SORT_ASCENDING)
        self.set_reorderable(False)
        
    def _ViewTooltip(self, widget, x, y, keyboard_mode, tooltip):
        try:
            (path, col, x, y) = widget.get_path_at_pos(int(x), int(y)) 
            it = widget.get_model().get_iter(path)
            value = widget.get_model().get_value(it, self.COLUMN_NAME)
            value += "\n%s" % widget.get_model().get_value(it, self.COLUMN_PATH)
            pix = widget.get_model().get_value(it, self.COLUMN_ICON)
            tooltip.set_icon(pix)
            tooltip.set_markup(value)
        except Exception, e:
            tooltip.set_text(str(e))
            
        
        return True
            
    def _on_row_change(self, data=None):
        
        selection = self.get_selection()
        model, row_iter = selection.get_selected()
        
        if len(model)>0:
            objDesktop = model[row_iter][self.COLUMN_DESKTOP]
            self.emit("selection-changed", model, row_iter, objDesktop)
    
    def clear_list(self):
        model = self.get_model()
        if model:
            model.clear()
            
    def get_selected_row(self):
        selection = self.get_selection()
        return selection.get_selected()
    
    def get_rows(self):
        desktops = []
        for row_iter in self.get_model():
                desktops.append({
                                 "icon": row_iter[self.COLUMN_ICON],
                                 "path": row_iter[self.COLUMN_PATH],
                                 "objDesktop":  row_iter[self.COLUMN_DESKTOP],
                                 "name_upper":row_iter[self.COLUMN_NAME_UPPER]
                                 })
        
        return desktops
    
    def get_selected_desktop(self):
        selection = self.get_selection()
        model, row_iter = selection.get_selected()
        
        return model[row_iter][self.COLUMN_DESKTOP]
    
    def get_selected_icon(self):
        selection = self.get_selection()
        model, row_iter = selection.get_selected()
        
        return model[row_iter][self.COLUMN_ICON]
    
    def set_value(self, value, column):
        selection = self.get_selection()
        model, row_iter = selection.get_selected()
        
        model[row_iter][column] = value

    @property
    def is_selected(self):
        selection = self.get_selection()
        model, row_iter = selection.get_selected()
        
        return (not row_iter is None)
    
    def remove_current_row(self):
        selection = self.get_selection()
        model, row_iter = selection.get_selected()
        
        indx = model.get_path(row_iter)[0]
        model.remove(row_iter)
        
        icount = len(model)
        
        if indx == icount and icount > 0:
            self.set_cursor((indx - 1,))
        elif indx < icount and icount > 0:
            self.set_cursor((indx,))
        else:
            self.set_cursor((indx + 1,))
            
    def set_selected_iter(self, row):
        self.set_cursor(self.get_model().get_path(row))
           
    def add_row(self, row_data, selected=False):
        new_iter = None
        if row_data:
            in_data =[]
            for n in self.get_model():
                in_data.append(n[self.COLUMN_PATH])
                               
            if row_data[self.COLUMN_PATH] not in in_data:
                new_iter = self.listmodel.append(row_data)
                if selected:
                    self.set_cursor(self.get_model().get_path(new_iter))
                
            return new_iter
