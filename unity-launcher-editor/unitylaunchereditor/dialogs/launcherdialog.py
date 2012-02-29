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
import gobject
import gtk
from pango import ELLIPSIZE_END
from unitylaunchereditor.core.translation import _
from unitylaunchereditor.core.constants import TITLES, DESCRIPTION
from unitylaunchereditor.addassistant import PageFromSystem, PageFromFile, PageFromInfo
from unitylaunchereditor.widgets.launcherinfo import LauncherInfo
from unitylaunchereditor.widgets.dialogheader import HeaderBoxButton, DialogHeader
from unitylaunchereditor.core.log import Logger
from unitylaunchereditor.core.utils import normalize_path, nullstring
from unitylaunchereditor.dialogs.dialogs import MessageBox
from unitylaunchereditor.core.translation import _, LOCALE

log = Logger()

class AddLauncherDialog(gtk.Window):

    def __init__(self, parent = None):
        super(AddLauncherDialog, self).__init__()
        self.parent_module = parent
        if parent:
            self.set_transient_for(parent)
            
        self.set_modal(True)
        self.set_type_hint(gtk.gdk.WINDOW_TYPE_HINT_DIALOG)
        self.set_resizable(False)
        self.set_default_size(650, 450)
        self.set_size_request(650, 450)
        
        self.set_title(_('Add Launcher...'))
        
        #header controls
        self.label_box = HeaderBoxButton(TITLES[0])
        self.label_box.set_description(DESCRIPTION[0])
        
        #this will hold pages for each option
        notebook= gtk.Notebook()
        notebook.set_border_width(0)
        notebook.set_show_border(False)
        notebook.set_show_tabs(False)
        
        fra=PageFromSystem()
        fra.connect('add-clicked', self.__on_add_clicked)
        notebook.append_page(fra)
        
        pfile = PageFromFile()
        pfile.connect('add-clicked', self.__on_newfile_clicked)
        notebook.append_page(pfile)
        
        pnew=PageFromInfo()
        pnew.connect('add-clicked', self.__on_new_frominfo_clicked)
        
        notebook.append_page(pnew)
        #----
        bottom_box = gtk.HButtonBox()
        bottom_box.set_layout(gtk.BUTTONBOX_END)
        bottom_box.set_border_width(6)
        close_button = gtk.Button(stock=gtk.STOCK_CLOSE)
        bottom_box.pack_start(close_button, fill=True, expand=False)
        
        self.label_box.add_start(notebook, True,True,0)
        self.label_box.add_start(gtk.HSeparator(), fill=True, expand=False)
        self.label_box.add_end(bottom_box, fill=True, expand=False)
        
        self.label_box.connect('button-clicked', self.change_page, notebook.set_current_page)
        close_button.connect('clicked', lambda w: self.hide())
        self.add(self.label_box)
        self.label_box.set_icon('gtk-execute')
        self.show_all()

    def __on_new_frominfo_clicked(self, widget, info):
        new_item =self.parent_module.new_from_info(info)
        if not new_item:
            MessageBox('Erro',self.parent_module,action_text=_("Erro inserting item."),message=_("Item already exists in the list.") ).run()
        
    def __on_newfile_clicked(self, widget,  filename):
        if filename:
            new_item =self.parent_module.new_from_file(normalize_path(filename),True)
            if not new_item:
                MessageBox('Erro',self.parent_module,action_text=_("Erro inserting item."),message=_("Item already exists in the list.") ).run()
        else:
            MessageBox('Erro',self.parent_module,action_text=_("No file selected."),message=_("You need to select a file in order to add a new Launcher") ).run()

    def __on_add_clicked(self, widget,  model, iter):
        app = model.get_value(iter, 2)
        new_item =self.parent_module.new_from_file(normalize_path(app.get_id()),True)
        if not new_item:
            MessageBox('Erro',self.parent_module,action_text=_("Erro inserting item."),message=_("Item already exists in the list.") ).run()
        
    def change_page(self, widget,id, show_page):
        #TODO: use our own icons
        icon_names={0:'gtk-execute', 1: 'gtk-file',2:'gtk-about'}
        
        show_page(id)
        
        self.label_box.set_title(TITLES[id])
        self.label_box.set_description(DESCRIPTION[id])
        self.label_box.set_icon(icon_names[id])

class EditLauncherDialog(gtk.Dialog):
    """XXX: add docs."""
    def __init__(self, title=_('Edit Launcher...'), parent=None):
        
        super(EditLauncherDialog, self).__init__(title,
            None, (gtk.DIALOG_MODAL | gtk.DIALOG_DESTROY_WITH_PARENT),
            (gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL))
        
        self.set_modal(True)
        self.set_type_hint(gtk.gdk.WINDOW_TYPE_HINT_DIALOG)
        self.set_resizable(False)

        if parent:
            self.set_transient_for(parent)
        
        self.ok_button = self.add_button(gtk.STOCK_OK, gtk.RESPONSE_OK)
        self.ok_button.grab_default()
        self.ok_button.set_sensitive(False)
        self.set_alternative_button_order([gtk.RESPONSE_CANCEL, gtk.RESPONSE_OK])
        #header controls
        self.label_box = DialogHeader(_('Edit Launcher'))
        self.label_box.set_description(_('Fill the fields below with your Launcher information.'))
        self.label_box.set_icon(gtk.STOCK_EDIT)
        self.label_box.add_start(gtk.HSeparator(), expand=False)
        
        self.launcher = LauncherInfo()
        self.label_box.add_start(self.launcher)
        self.launcher.connect('field-changed', self._on_change)
        self.label_box.add_start(gtk.HSeparator(), expand=False)
        self.vbox.pack_start(self.label_box, True, True, 0)
        self.vbox.show_all()

    def populate(self, desktop_obj):
        launcher_name = desktop_obj.get('Name', locale=LOCALE)
        launcher_command = desktop_obj.get('Exec', locale=LOCALE)
        launcher_comment = nullstring(desktop_obj.get('Comment', locale=LOCALE))
        launcher_run_terminal = desktop_obj.get('Terminal', locale=LOCALE)
        launcher_icon_name=desktop_obj.get('Icon', locale=LOCALE)
        
        self.launcher.launcher_name = launcher_name
        self.launcher.launcher_command = launcher_command
        self.launcher.launcher_comment = launcher_comment
        self.launcher.launcher_run_terminal = launcher_run_terminal
        self.launcher.launcher_icon_name = launcher_icon_name
        
        self.launcher.entry_name.set_text(self.launcher.launcher_name)
        self.launcher.entry_command.set_text(self.launcher.launcher_command)
        self.launcher.entry_comment.get_buffer().set_text(self.launcher.launcher_comment)
        self.launcher.chkTerminal.set_active(self.launcher.launcher_run_terminal)
        
        
    def _on_change(self,widget, text):
        if self.launcher.entry_name.get_text_length() > 0 and self.launcher.entry_command.get_text_length()>0:
            self.ok_button.set_sensitive(True)
        else:
            self.ok_button.set_sensitive(False)
        
    def run(self, destroy=True):
        """Returns True if yes was clicked, False otherwise."""
        resp = super(EditLauncherDialog, self).run()
        obj = None
        if resp == gtk.RESPONSE_OK:
            obj = {'Name': self.launcher.launcher_name}
            obj['Exec']= self.launcher.launcher_command
            obj['Comment'] = self.launcher.launcher_comment
            obj['Icon'] = self.launcher.launcher_icon_name
        if destroy:
            self.destroy()
        if resp == gtk.RESPONSE_OK:
            return True, obj
        else:
            return False, None