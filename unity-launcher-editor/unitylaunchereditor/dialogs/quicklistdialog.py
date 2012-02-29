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
from unitylaunchereditor.widgets.groupinfo import GroupInfo
from unitylaunchereditor.widgets.dialogheader import HeaderBoxButton, DialogHeader
from unitylaunchereditor.core.log import Logger
from unitylaunchereditor.core.utils import nullstring
from unitylaunchereditor.dialogs.dialogs import MessageBox
from unitylaunchereditor.core.translation import _, LOCALE

log = Logger()

class QuicklistDialog(gtk.Dialog):
    """XXX: add docs."""
    def __init__(self, title=_('Edit Launcher...'), parent=None):
        
        super(QuicklistDialog, self).__init__(title,
            None, (gtk.DIALOG_MODAL | gtk.DIALOG_DESTROY_WITH_PARENT),
            (gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL))
        
        self.set_modal(True)
        self.set_resizable(False)
        self.set_default_size(600, 400)
        self.set_size_request(600, 400)
        
        if parent:
            self.set_transient_for(parent)
        
        self.ok_button = self.add_button(gtk.STOCK_OK, gtk.RESPONSE_OK)
        self.ok_button.grab_default()
        self.ok_button.set_sensitive(False)
        self.set_alternative_button_order([gtk.RESPONSE_CANCEL, gtk.RESPONSE_OK])
        #header controls
        self.label_box = DialogHeader(_('Add quicklist group'))
        self.label_box.set_description(_('Fill the fields below with your quicklist group information.'))
        self.label_box.set_icon(gtk.STOCK_EDIT)
        self.label_box.add_start(gtk.HSeparator(), expand=False)
        
        self.groupinfo = GroupInfo()
        self.label_box.add_start(self.groupinfo)
        self.groupinfo.connect('field-changed', self._on_change)
        self.label_box.add_start(gtk.HSeparator(), expand=False)
        self.vbox.pack_start(self.label_box, True, True, 0)
        self.vbox.show_all()

    def _on_change(self,widget, text):
        if self.groupinfo.entry_name.get_text_length() > 0 and self.groupinfo.entry_command.get_text_length()>0 and self.groupinfo.entry_gname.get_text_length()>0:
            self.ok_button.set_sensitive(True)
        else:
            self.ok_button.set_sensitive(False)
    
    def populate(self, values):
        try:
            self.groupinfo.quicklist_group_name = values['GroupName']
            self.groupinfo.quicklist_name =values['Name']
            self.groupinfo.quicklist_command = values['Exec']
            self.groupinfo.quicklist_visible = values['Visible']

            self.groupinfo.entry_gname.set_text(values['GroupName'])
            self.groupinfo.entry_gname.set_sensitive(False)
            self.groupinfo.entry_name.set_text(values['Name'])
            self.groupinfo.entry_command.set_text(values['Exec'])
            self.groupinfo.chkVisible.set_active(values['Visible'])
        except:
            pass
        try:
            for n in range(len(self.groupinfo.combo_model)):
                if self.groupinfo.combo_model[n][0] == values['TargetEnvironment']:
                    self.groupinfo.cmbTargetEnvironment.set_active(n)
        except:
            pass
        
    def run(self, destroy=True):
        """Returns True if yes was clicked, False otherwise."""
        resp = super(QuicklistDialog, self).run()
        obj = None
        if resp == gtk.RESPONSE_OK:
            obj = {'GroupName':self.groupinfo.quicklist_group_name}
            obj['Name']= self.groupinfo.quicklist_name
            obj['Exec']= self.groupinfo.quicklist_command
            obj['TargetEnvironment'] = self.groupinfo.cmbTargetEnvironment.get_active_text()
            obj['Visible'] = self.groupinfo.chkVisible.get_active()
        if destroy:
            self.destroy()
        if resp == gtk.RESPONSE_OK:
            return True, obj
        else:
            return False, None