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
from gobject import TYPE_STRING
from os.path import basename, exists, expanduser, join
from pkg_resources import resource_filename
from gio import  FileIcon, ThemedIcon,File
from unitylaunchereditor.core.desktop import DesktopParser
from unitylaunchereditor.core.iconmanager import IconManager,IMAGE_SIZE
from unitylaunchereditor.core.translation import _, LOCALE
from unitylaunchereditor.dialogs.dialogs import DialogType
from unitylaunchereditor.dialogs.dialogs import FileChooser, MessageBox, GroupsDialog
from unitylaunchereditor.widgets.launcherview import LauncherView
from unitylaunchereditor.widgets.grouptree import GroupTree
from unitylaunchereditor.core.utils import nullstring
from unitylaunchereditor.core import iconmanager
from unitylaunchereditor.dialogs.launcheradd import HeaderBase


#constantes for toolbar buttons
(TOOLBUTTON_ADD,
TOOLBUTTON_EDIT,
TOOLBUTTON_REMOVE) = range(3)

class MainWindow(object):

    builder = gtk.Builder()

    def __init__(self, controller=None, glade_file="application.glade"):
        self.controller = controller
        glade_path = resource_filename(__name__, join("data", glade_file))
        try:
            self.builder.add_from_file(glade_path)
        except gobject.GError:
            if not exists(glade_path):
                print _("Critical: couldn't find glade file: %s") % glade_path
                raise
        except Exception, exc:
            print ("Something else went wrong: %s") % exc
            raise

        window_name = "window1"
        self.window = self.builder.get_object(window_name)
        
        header = gtk.EventBox()
        header.set_visible_window(True)
        header.modify_bg(gtk.STATE_NORMAL, gtk.gdk.Color('#3c3b37'))
        
        self.header = HeaderBase()
        header.add(self.header)
        self.header.set_title('Teessste')
        self.header.set_description('')
        img=gtk.Image()
        img.set_from_file(resource_filename(__name__, join("data", 'plugin-unityshell.png')))
        self.header.set_icon(img.get_pixbuf())
        self.builder.get_object('vbox1').pack_start(header,False,False)
        self.builder.get_object('vbox1').reorder_child(header,0)
        self.window.show_all()
        # launcher Objects
        self.list_container = self.builder.get_object('list_container')
        self.listview_launcher = LauncherView()
        self.list_container.add_with_viewport(self.listview_launcher)
        # groups objects
        self.group_container = self.builder.get_object('group_container')
        self.tvwGroups = GroupTree()
        self.group_container.add_with_viewport(self.tvwGroups)
        
        # info Widgets
        self.imgIcon = self.builder.get_object('imgIcon')
        self.txtName = self.builder.get_object('txtName')
        self.txtName.set_name('Name')
        self.txtComment = self.builder.get_object('txtComment')
        self.txtComment.set_name('Comment')
        self.txtCommand = self.builder.get_object('txtCommand')
        self.txtCommand.set_name('Exec')

        self.chkTerminal = self.builder.get_object('chkTerminal')
        self.chkTerminal.set_name('Terminal')

        # button widgets
        self.cmdIcon = self.builder.get_object('cmdIcon')
        self.cmdSave = self.builder.get_object('cmdSave')

        #toolbar widgets
        self.btnRemove = self.builder.get_object('btnRemove')
        self.btnEdit = self.builder.get_object('btnEdit')

        # set signals
        self.setup_signals()

    def setup_signals(self):
        # window close
        self.window.connect('delete-event', gtk.main_quit)

        #buttons event
        self.builder.get_object('cmdClose').connect('clicked', gtk.main_quit)
        self.builder.get_object('cmdAdd').connect('clicked', self.add_new_launcher)
        self.cmdSave.connect('clicked', self.save_launchers)
        self.builder.get_object('cmdRemove').connect('clicked', self.remove_from_launcher)
        self.cmdIcon.connect('clicked', self.load_icon_from_file)
        self.builder.get_object('cmdRun').connect('clicked', self.open_executable)
        self.chkTerminal.connect('toggled', self.chk_toggle)

        #editable events
        self.txtName.connect('focus-out-event', self.on_txt_focus_out)
        self.txtComment.connect('focus-out-event', self.on_txt_focus_out)
        self.txtCommand.connect('focus-out-event', self.on_txt_focus_out)

        #list group signals
        self.tvwGroups.connect('selection-changed', self.on_tvwgroups_row_change)

        #group signals
        self.builder.get_object('btnAdd').connect('clicked', self.add_remove_group, TOOLBUTTON_ADD)
        self.btnRemove.connect('clicked', self.add_remove_group, TOOLBUTTON_REMOVE)
        self.btnEdit.connect('clicked', self.add_remove_group, TOOLBUTTON_EDIT)

    def show(self):
        self.window.show_all()

    def normalize_path(self, str_path):
        file_name = basename(str_path)
        str_path = '%s/.local/share/applications/%s' % (expanduser('~'),
            file_name)
        return str_path

    def load_launcher_from_list(self, unity_items):
        self.listview_launcher.clear_list();
        for menu_item in unity_items:
            obj=DesktopParser(menu_item)
            sname = obj.get('Name')
            desc = obj.get('Comment')
            icon = obj.get('Icon')
            pix = IconManager().get_icon(ThemedIcon('image-missing'),IMAGE_SIZE)
            if icon:
                if icon.rfind('.') != -1:
                    pix = IconManager().get_icon(FileIcon(File(icon)),IMAGE_SIZE)
                else:
                    pix = IconManager().get_icon(ThemedIcon(icon),IMAGE_SIZE)
            data = (pix, '%s' % sname, obj, sname.upper(), menu_item)
            self.listview_launcher.add_row(data)

        self.listview_launcher.connect('selection-changed', self.listview_launcher_row_change)
        #make first row active
        self.listview_launcher.set_cursor((0,))
        
    def save_launchers(self, widget):
        dialog = MessageBox(self.window,
                                window_title=_('Save'),
                                action_text=_('Save changes'),
                                message=_('Do you want to save your changes?'),
                                dlg_type=DialogType.DIALOG_CONFIRMATION,
                                label_text=None
                                )
        result, text = dialog.run()
        if result:
            launchers = []
            for row in self.listview_launcher.get_rows():
                file_path = self.normalize_path(row["path"])
                fli = open(file_path, 'w')
                
                obj = row["obj"]
                obj.write(fli)
                fli.close()
                launchers.append(file_path)
                
            if self.controller:
                self.controller.save_unity_list(launchers)
                MessageBox(self.window,
                                window_title=_('Information'),
                                action_text=_('Changes saved'),
                                message=_('Your changes were successfuly saved.\n\n To apply the changes to your launcher you will need to <b>logout</b> and login again.'),
                                dlg_type=DialogType.DIALOG_MESSAGE,
                                label_text=None
                                ).run()
            else:
                MessageBox(self.window,
                                window_title=_('Error'),
                                action_text=_('Changes not saved'),
                                message=_("There's a problem with your application, \n controller not found."),
                                dlg_type=DialogType.DIALOG_MESSAGE,
                                label_text=None
                                ).run()
                

    def chk_toggle(self, widget):
        # only run when gaving the focus
        # this will prevent execution when setting the value by code.

        if widget.is_focus():
            objDesktop = self.listview_launcher.get_selected_desktop()
            value = widget.get_active()
            option = widget.get_name()
            objDesktop.set(option, value)

    def on_txt_focus_out(self, widget, event):
        ilength = 0
        if  isinstance(widget, gtk.TextView):
            buffer = widget.get_buffer()
            value = buffer.get_text(buffer.get_start_iter(),
                buffer.get_end_iter())
            ilength = len(value)
        else:
            ilength = widget.get_text_length() 
            value = widget.get_text()

        if ilength > 0:
            objDesktop = self.listview_launcher.get_selected_desktop()
            option = widget.get_name()
            objDesktop.set(option, value)
            if option == 'Name' or option == 'Comment':
                sname = objDesktop.get('Name')
                desc = objDesktop.get('Comment')
                self.listview_launcher.set_value('<b>%s</b>\n%s' % (sname, desc), self.listview_launcher.COLUMN_NAME)

    def add_remove_group(self, widget, operation):
        
        objDesktop = self.listview_launcher.get_selected_desktop()
        
        if operation == TOOLBUTTON_ADD:
            #TODO : update ayatana entry
            
            dialog = GroupsDialog(parent=self.window)
            result, group = dialog.run()
            if result:
                root = self.tvwGroups.add_root(group['GroupName'])

                shortcut_group_name = "%s Shortcut Group" % group['GroupName']
                self.tvwGroups.add_root_row(root, 'Name', group['Name'])
                self.tvwGroups.add_root_row(root, 'Exec', group['Exec'])
                self.tvwGroups.add_root_row(root, 'TargetEnvironment', group['TargetEnvironment'])

                items = objDesktop.get('X-Ayatana-Desktop-Shortcuts')
                if items is None:
                    items = []
                else:
                    items = [n for n in items if n != '']
                items.append(group['GroupName'])
                objDesktop.set('X-Ayatana-Desktop-Shortcuts', items)

                objDesktop.append_section(shortcut_group_name)
                objDesktop.set('Name', group['Name'] , None, shortcut_group_name)           
                objDesktop.set('Exec', group['Exec'] , None, shortcut_group_name)
                objDesktop.set('TargetEnvironment', group['TargetEnvironment'] , None, shortcut_group_name)
                self.tvwGroups.expand(root, True)
                
                
                
        elif operation == TOOLBUTTON_REMOVE:
            dialog = MessageBox(self.window,
                                window_title=_('Remove') ,
                                action_text=_('Remove Group'),
                                message=_('You are about to remove a Quick List Group, are you sure?'),
                                dlg_type=DialogType.DIALOG_CONFIRMATION,
                                label_text=None
                                )
            result = dialog.run()
                 
            if result[0]:
                root_iter = self.tvwGroups.get_selected_root()
                group = self.tvwGroups.get_root_text()
                shortcut_group_name = "%s Shortcut Group" % group
                
                self.tvwGroups.remove_root(root_iter)
                
                items = objDesktop.get('X-Ayatana-Desktop-Shortcuts')
                items.remove(group)
                groups = [x for x in items if x != '']
                objDesktop.remove_sections(shortcut_group_name)
                objDesktop.set('X-Ayatana-Desktop-Shortcuts', groups)
                
        elif operation == TOOLBUTTON_EDIT:               
            root_iter = self.tvwGroups.get_selected_root()
            
            root_text = self.tvwGroups.get_root_text()
            
            shortcut_group_name = "%s Shortcut Group" % root_text
            
            items = self.tvwGroups.get_children_values(root_iter)

            dialog = GroupsDialog(parent=self.window)
            dialog.populate(items)
            result, group = dialog.run()
            
            if result:
                self.tvwGroups.set_child_value(root_iter, 0, group['Name'])
                self.tvwGroups.set_child_value(root_iter, 1, group['Exec'])
                self.tvwGroups.set_child_value(root_iter, 2, group['TargetEnvironment'])
                
                objDesktop.set('Name', group['Name'] , LOCALE, shortcut_group_name)           
                objDesktop.set('Exec', group['Exec'] ,LOCALE, shortcut_group_name)
                objDesktop.set('TargetEnvironment', group['TargetEnvironment'] , LOCALE, shortcut_group_name)
            
            
            
    def remove_from_launcher(self, widget):
        """Signal handler for button to remoce an item from the Launcher List"""
        
        if self.listview_launcher.is_selected:
            dialog = MessageBox(window_title=_('Remove') ,
                                parent=self.window,
                                action_text=_('Remove Launcher'),
                                message=_('You are about to remove a Launcher item, are you sure?'),
                                dlg_type=DialogType.DIALOG_CONFIRMATION,
                                label_text=None
                                )
            result, text = dialog.run()                   
            if result:
                self.listview_launcher.remove_current_row()
                        
        else:
            MessageBox(self.window, window_title=_('Remove') ,
                       action_text=_('Remove Launcher'),
                       message=_('No launcher selected'),
                       dlg_type=DialogType.DIALOG_CONFIRMATION,
                       label_text=None
                       ).run()

    def add_new_launcher(self, widget):
        """Signal handler for button to add a new item into the Launcher List"""
        
        dialog = MessageBox(self.window,
                                window_title=_('Add Launcher') ,
                                action_text=_('Add a new launcher...') ,
                                message=_('Type a launcher name to create a new launcher.'),
                                dlg_type=DialogType.DIALOG_INPUT,
                                label_text=_('Name:')
                                )

        result, text = dialog.run()
        
        if result and len(text) > 0:
            desktop_parser = DesktopParser()
            desktop_parser.set('Name', text)
            desktop_parser.set('Terminal', False)
            desktop_parser.set('Icon', 'wft')
            path_name = join(expanduser('~/.local/share/applications/'),
                '%s.desktop' % text)
            row_iter = self.listview_launcher.add_row(path_name, True, desktop_parser)
        return row_iter

    def open_executable(self, widget):
        dialog = FileChooser()
        response, filename = dialog.run()
        if response:
            self.txtCommand.set_text(filename)
            objDesktop = self.listview_launcher.get_selected_desktop()
            objDesktop.set(self.txtCommand.get_name(), filename)


    def load_icon_from_file(self, widget):
        # XXX: Here maybe we could make a list with all mimetypes and patterns
        # somewhere else and just iterate over them here.
        filter = gtk.FileFilter()
        filter.set_name("Images")
        filter.add_mime_type("image/png")
        filter.add_mime_type("image/jpeg")
        filter.add_mime_type("image/gif")
        filter.add_mime_type("image/svg+xml")
        filter.add_pattern("*.png")
        filter.add_pattern("*.jpg")
        filter.add_pattern("*.gif")
        filter.add_pattern("*.tif")
        filter.add_pattern("*.xpm")
        filter.add_pattern("*.svg")

        dialog = FileChooser(filters=filter)
        response, filename = dialog.run()
        if response:
            pixbuf = get_icon(filename)
            self.imgIcon.set_from_pixbuf(pixbuf)
            # TODO: add code to write changes here
            # the best way is to put an apply button?
            objDesktop = self.listview_launcher.get_selected_desktop()
            objDesktop.set('Icon', filename)
            self.listview_launcher.set_value(pixbuf, self.listview_launcher.COLUMN_ICON)
    
    def listview_launcher_row_change(self, widget, model, row_iter, objDesktop):
        
        #disable groups toobar buttons
        self.btnEdit.set_sensitive(False)
        self.btnRemove.set_sensitive(False)
            
        # show properties
        self.imgIcon.set_from_pixbuf(widget.get_selected_icon())

        self.txtName.set_text(nullstring(objDesktop.get('Name', locale = LOCALE)))
        self.txtComment.get_buffer().set_text(
            nullstring(objDesktop.get('Comment', locale = LOCALE)))
        self.txtCommand.set_text(nullstring(objDesktop.get('Exec')))
        
        checked = objDesktop.get('Terminal')
        if not checked:
            checked = False
            
        self.chkTerminal.set_active(checked)
        items = [item.replace(' Shortcut Group','') for item in objDesktop._sections] 
        #items = objDesktop.get('X-Ayatana-Desktop-Shortcuts')
        items.remove('Desktop Entry')
        self.tvwGroups.clear()

        if items:
            if isinstance(items, str):
                items = [items]
                
            for group in items:
                if group != '':
                    root = self.tvwGroups.add_root(group)
                    for key in ('Name', 'Exec', 'TargetEnvironment'):
                        value = objDesktop.get(key, "%s Shortcut Group" % group, locale = LOCALE )
                        if value:
                            self.tvwGroups.add_root_row(root, key, value)
        self.tvwGroups.expand_all()

                        
    def on_tvwgroups_row_change(self, widget, root_iter):
        if root_iter:
            self.btnEdit.set_sensitive(True)
            self.btnRemove.set_sensitive(True)
        else:
            self.btnEdit.set_sensitive(True)
            self.btnRemove.set_sensitive(True)
            
