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

from gio import ThemedIcon, FileIcon, File
from os.path import join
from pkg_resources import resource_filename
from unitylaunchereditor.core.utils import normalize_path, remove_html_tags
from unitylaunchereditor.core.desktop import DesktopParser
from unitylaunchereditor.core.iconmanager import IconManager
from unitylaunchereditor.core.constants import IMAGE_SIZE
from unitylaunchereditor.core.translation import LOCALE, _
from unitylaunchereditor.widgets.dialogheader import DialogHeader
from unitylaunchereditor.widgets.launcherview import LauncherView
from unitylaunchereditor.widgets.launcherinfo import LauncherInfo
from unitylaunchereditor.widgets.groupview import GroupView, GroupView
from unitylaunchereditor.dialogs.launcherdialog import AddLauncherDialog,EditLauncherDialog
from unitylaunchereditor.dialogs.quicklistdialog import QuicklistDialog
from unitylaunchereditor.dialogs.dialogs import DialogType,MessageBox
from unitylaunchereditor.core.log import Logger
from unitylaunchereditor.core.translation import LOCALE

log = Logger(__name__)

#constantes for toolbar buttons
(TOOLBUTTON_ADD,
TOOLBUTTON_EDIT,
TOOLBUTTON_REMOVE) = range(3)

class MainWindow(gtk.Window):
    def __init__(self, gsettings):
        super(MainWindow, self).__init__()
        log.info('Application started.')
        self.set_title("Unity Launcher Editor")
        self.gsettings = gsettings
        
        self.set_default_size(700, 500)
        self.set_size_request(700, 500)
        self.set_position(gtk.WIN_POS_CENTER)
        
        main_box = gtk.VBox()
        self.add(main_box)
        
        #header labels and imagem
        self.__create_header(main_box)
        
        main_panel = gtk.HPaned()
        main_box.pack_start(main_panel,fill=True, expand=True)
        
        #laucher list
        self.__create_launcher(main_panel)
        
        #group lists
        self.__create_group_view(main_panel)
        
        #bottom window buttons
        self.__create_bottom_box(main_box)
        
        self.__populate()
        self.connect('delete-event', lambda x,y:gtk.main_quit())
        
    def __create_header(self, container):
        self.header_title = DialogHeader(_('Unity Launcher Editor'))
        self.header_title.set_description(_('Create unity launcher and quicklist easely.'))
        
        #TODO: Set bacground color using themed colors
        #color = getattr(self.get_style(), 'fg', None)[gtk.STATE_PRELIGHT]
        #header_box.modify_bg(gtk.STATE_NORMAL, gtk.gdk.Color(str(color)))
        
        #header_box.add(self.header_title)
        #header image icon
        img_path = resource_filename(__name__, join("../data", 'plugin-unityshell.png'))
        self.header_title.set_icon(img_path)
        self.header_title.add_start(gtk.HSeparator())
        container.pack_start(self.header_title, expand=False)
    
    def __create_launcher(self, container):
        vbox = gtk.VBox()
        toolbar_view = gtk.Toolbar()
        toolbar_view.set_style(gtk.TOOLBAR_ICONS)
        
        add_buttom = gtk.ToolButton()
        add_buttom.set_icon_name(gtk.STOCK_NEW)
        add_buttom.set_use_action_appearance(False)
        add_buttom.set_tooltip_text(_('Create a new Launcher item.'))
        add_buttom.connect('clicked', self.__on_laucher_button_click, TOOLBUTTON_ADD)
        
        edit_buttom = gtk.ToolButton()
        edit_buttom.set_icon_name(gtk.STOCK_PROPERTIES)
        edit_buttom.set_use_action_appearance(False)
        edit_buttom.set_tooltip_text(_('Edit select Launcher information.'))
        edit_buttom.connect('clicked', self.__on_laucher_button_click, TOOLBUTTON_EDIT)
        
        remove_buttom = gtk.ToolButton()
        remove_buttom.set_icon_name(gtk.STOCK_DELETE)
        remove_buttom.set_use_action_appearance(False)
        remove_buttom.set_tooltip_text(_('Remove selected Launcher.'))
        remove_buttom.connect('clicked', self.__on_laucher_button_click, TOOLBUTTON_REMOVE)
        
        toolbar_view.insert(add_buttom, TOOLBUTTON_ADD)
        toolbar_view.insert(edit_buttom, TOOLBUTTON_EDIT)
        toolbar_view.insert(remove_buttom, TOOLBUTTON_REMOVE)
        
        self.launcher_view = LauncherView()
        hscroll = gtk.ScrolledWindow()
        hscroll.set_shadow_type(gtk.SHADOW_ETCHED_IN)
        hscroll.set_border_width(5)
        hscroll.set_size_request(200,200)
        hscroll.add(self.launcher_view)
        
        
        vbox.pack_start(toolbar_view, expand = False)
        vbox.pack_start(hscroll, expand = True)
        container.set_position(100)
        container.pack1(vbox, resize=False, shrink=False)
    
    def __create_group_view(self, container):
        vbox = gtk.VBox()
        toolbar_view = gtk.Toolbar()
        toolbar_view.set_style(gtk.TOOLBAR_ICONS)
        
        add_button = gtk.ToolButton()
        add_button.set_icon_name(gtk.STOCK_ADD)
        add_button.set_use_action_appearance(False)
        add_button.set_tooltip_text(_('Add a new quicklist group.'))
        add_button.connect('clicked', self.__on_quicklist_button_click, TOOLBUTTON_ADD)
        
        self.edit_button = gtk.ToolButton()
        self.edit_button.set_icon_name(gtk.STOCK_EDIT)
        self.edit_button.set_use_action_appearance(False)
        self.edit_button.set_tooltip_text(_('Change select quicklist group information.'))
        self.edit_button.connect('clicked', self.__on_quicklist_button_click, TOOLBUTTON_EDIT)
        
        self.remove_button = gtk.ToolButton()
        self.remove_button.set_icon_name(gtk.STOCK_REMOVE)
        self.remove_button.set_use_action_appearance(False)
        self.remove_button.set_tooltip_text(_('Remove selected quicklist group.'))
        self.remove_button.connect('clicked', self.__on_quicklist_button_click, TOOLBUTTON_REMOVE)
        
        toolbar_view.insert(add_button, TOOLBUTTON_ADD)
        toolbar_view.insert(self.edit_button, TOOLBUTTON_EDIT)
        toolbar_view.insert(self.remove_button, TOOLBUTTON_REMOVE)
        
        self.group_view = GroupView()
        hscroll = gtk.ScrolledWindow()
        hscroll.set_shadow_type(gtk.SHADOW_ETCHED_IN)
        hscroll.set_border_width(5)
        hscroll.set_size_request(200,200)
        hscroll.add(self.group_view)
        
        vbox.pack_start(toolbar_view, expand = False)
        vbox.pack_start(hscroll, expand = True)
        
        container.pack2(vbox, resize=False, shrink=False)
        #list group signals
        self.group_view.connect('selection-changed', self.__group_view_row_change)
            
    def __create_bottom_box(self, container):
        #Bottom button list for save and close
        button_close = gtk.Button(stock=gtk.STOCK_CLOSE)
        button_save = gtk.Button(stock=gtk.STOCK_SAVE)
        bottom_box = gtk.HButtonBox()
        bottom_box.set_layout(gtk.BUTTONBOX_END)
        bottom_box.set_border_width(12)
        bottom_box.pack_end(button_save)
        bottom_box.pack_end(button_close)
        button_close.connect('clicked', lambda x: gtk.main_quit())
        button_save.connect('clicked', self.save_launchers)
        
        container.pack_start(gtk.HSeparator(), False,True,0)
        container.pack_start(bottom_box, expand=False)
        
    def new_from_info(self, info):
        file_path = normalize_path('%s.desktop' % info['Name'])
        
        obj = DesktopParser()
        
        obj.set('Name',info['Name'])
        obj.set('Name',info['Comment'])
        obj.set('Name',info['Icon'])
        obj.set('Name',info['Exec'])
        
        sname = info['Name']
        desc = info['Comment']
        icon = info['Icon']
        pix = IconManager().get_icon(ThemedIcon('image-missing'),32)
        if icon:
            if icon.rfind('.') != -1:
                pix = IconManager().get_icon(FileIcon(File(icon)),32)
            else:
                pix = IconManager().get_icon(ThemedIcon(icon),32)
        data = (pix, '%s' % sname, obj, sname.upper(), file_path)
        
        return self.launcher_view.add_row(data,True)
        
    def new_from_file(self, filename, selected=True):
        try:
            file_path = normalize_path(filename, True)
            obj = DesktopParser(file_path)
            sname = obj.get('Name',locale=LOCALE)
            desc = obj.get('Comment',locale=LOCALE)
            icon = obj.get('Icon')
            pix = IconManager().get_icon(ThemedIcon('image-missing'),32)
            if icon:
                if icon.rfind('.') != -1:
                    pix = IconManager().get_icon(FileIcon(File(icon)),32)
                else:
                    pix = IconManager().get_icon(ThemedIcon(icon),32)
            
                data = (pix, '%s' % sname, obj, sname.upper(), file_path)
            return self.launcher_view.add_row(data,selected)
        except:
            return None
        
    def __populate(self):
        self.launcher_view.clear_list();
        self.unity = self.gsettings.get_value('favorites')
        log.info(self.unity)
        for menu_item in self.unity:
            self.new_from_file(menu_item, False)
        self.launcher_view.connect('selection-changed', self.__launcher_view_row_change)
        #make first row active
        self.launcher_view.set_cursor((0,))
            
        return
    def __on_quicklist_button_click(self, widget, button_type):
        if button_type == TOOLBUTTON_ADD:
            dialog = QuicklistDialog(parent=self)
            result, group_info = dialog.run()
            if result:
                objDesktop = self.launcher_view.get_selected_desktop()
                shortcut_group_name = "%s Shortcut Group" % group_info['GroupName']
                items = [item.replace(' Shortcut Group','') for item in objDesktop._sections]
                if group_info['Visible']:
                    
                    items.append(group_info['GroupName'])
                    objDesktop.set('X-Ayatana-Desktop-Shortcuts', items)
                
                objDesktop.append_section(shortcut_group_name)
                objDesktop.set('Name', group_info['Name'] , None, shortcut_group_name)           
                objDesktop.set('Exec', group_info['Exec'] , None, shortcut_group_name)
                objDesktop.set('TargetEnvironment', group_info['TargetEnvironment'] , None, shortcut_group_name)
                self.group_view.add_row(group_info)
            
        elif button_type == TOOLBUTTON_EDIT:
            values = self.group_view.get_selected_row()
            dialog = QuicklistDialog(parent=self)
            dialog.populate(values)
            result, group_info = dialog.run()
            if result:
                objDesktop = self.launcher_view.get_selected_desktop()
                shortcut_group_name = "%s Shortcut Group" % group_info['GroupName']
                items = [item for item in objDesktop.get('X-Ayatana-Desktop-Shortcuts', locale=LOCALE)]
                if group_info['Visible']:
                    if group_info['GroupName'] not in items:
                        items.append(group_info['GroupName'])
                        objDesktop.set('X-Ayatana-Desktop-Shortcuts', items)
                else:
                    if group_info['GroupName'] in items:
                        items.remove(group_info['GroupName'])
                        objDesktop.set('X-Ayatana-Desktop-Shortcuts', items)

                objDesktop.set('Name', group_info['Name'] , LOCALE, shortcut_group_name)           
                objDesktop.set('Exec', group_info['Exec'] , None, shortcut_group_name)
                objDesktop.set('TargetEnvironment', group_info['TargetEnvironment'] , None, shortcut_group_name)
                self.group_view.update_current_row(group_info)
                
                
        elif button_type == TOOLBUTTON_REMOVE:
            dialog = MessageBox(parent = self,
                                window_title=_('Remove') ,
                                action_text=_('Remove Group'),
                                message=_('You are about to remove a Quick List Group, are you sure?'),
                                dlg_type=DialogType.DIALOG_CONFIRMATION,
                                )
            result, text = dialog.run()
                 
            if result:
                objDesktop = self.launcher_view.get_selected_desktop()
                
                group_info = self.group_view.get_selected_row()
                group = group_info['GroupName']
                
                shortcut_group_name = "%s Shortcut Group" % group
                
                if self.group_view.remove_selected_row():
                    items = objDesktop.get('X-Ayatana-Desktop-Shortcuts')
                    if group in items:
                        items.remove(group)
                    groups = [x for x in items if x != '']
                    objDesktop.remove_sections(shortcut_group_name)
                    objDesktop.set('X-Ayatana-Desktop-Shortcuts', groups)
                    
    def __on_laucher_button_click(self, widget, button_type):
        
        if button_type == TOOLBUTTON_ADD:
            dialog = AddLauncherDialog(self)
            dialog.set_position(gtk.WIN_POS_CENTER_ALWAYS)
            dialog.show_all()
        if button_type == TOOLBUTTON_EDIT:
            if self.launcher_view.is_selected:
                win = EditLauncherDialog()
                desktop_obj = self.launcher_view.get_selected_desktop()
                win.populate(desktop_obj)
                result, obj = win.run()
                if result:
                    for key, value in obj.iteritems():
                        desktop_obj.set(key,value, locale=LOCALE)
                        
                    self.launcher_view.set_value(obj['Name'], self.launcher_view.COLUMN_NAME)
                    self.launcher_view.set_value(obj['Name'].upper(), self.launcher_view.COLUMN_NAME_UPPER)
                    icon = obj['Icon']
                    pix = IconManager().get_icon(ThemedIcon('image-missing'),32)
                    if icon:
                        if icon.rfind('.') != -1:
                            pix = IconManager().get_icon(FileIcon(File(icon)),32)
                        else:
                            pix = IconManager().get_icon(ThemedIcon(icon),32)
                        self.launcher_view.set_value(pix, self.launcher_view.COLUMN_ICON)
                        
        if button_type == TOOLBUTTON_REMOVE:
            if self.launcher_view.is_selected:
                dialog = MessageBox(window_title=_('Remove') ,
                                    parent=self,
                                    action_text=_('Remove Launcher'),
                                    message=_('You are about to remove a Launcher item, are you sure?'),
                                    dlg_type=DialogType.DIALOG_MESSAGE
                                    )
                result, text = dialog.run()
                if result:
                    self.launcher_view.remove_current_row()
                            
            else:
                MessageBox(parent=self, window_title=_('Remove') ,
                           action_text=_('Remove Launcher'),
                           message=_('No launcher selected'),
                           dlg_type=DialogType.DIALOG_MESSAGE
                           ).run()
            
    def __launcher_view_row_change(self, widget, model, row_iter, objDesktop):
        
        #disable groups toobar buttons
        self.edit_button.set_sensitive(False)
        self.remove_button.set_sensitive(False)

        items = [item.replace(' Shortcut Group','') for item in objDesktop._sections] 
        items_visible = objDesktop.get('X-Ayatana-Desktop-Shortcuts')
        items.remove('Desktop Entry')
        self.group_view.clear()

        if items:
            if isinstance(items, str):
                items = [items]
                
            for group in items:
                if group != '':
                    root = {'GroupName':group}
                    root['Name']= objDesktop.get('Name', "%s Shortcut Group" % group, locale=LOCALE )
                    root['Exec']= objDesktop.get('Exec', "%s Shortcut Group" % group)
                    value = objDesktop.get('TargetEnvironment', "%s Shortcut Group" % group)
                    if isinstance(value, list):
                        root['TargetEnvironment'] = ";".join(value)
                    else:
                        root['TargetEnvironment']= value
                    root['Visible']= (group in items_visible)
                    self.group_view.add_row(root)
                        
    def __group_view_row_change(self, widget, root_iter):
        if root_iter:
            self.edit_button.set_sensitive(True)
            self.remove_button.set_sensitive(True)
        else:
            self.edit_button.set_sensitive(True)
            self.remove_button.set_sensitive(True)

    def __save_unity_list(self, menu_items):
        try:
            print str(menu_items)
            self.gsettings.set_strv('favorites', menu_items)
            self.__populate()
            return True
        except gobject.GError, e:
            return False
        #return_code = os.system('unity --reset') #subprocess.call(['unity','--reset'])
        #if return_code != 0:
        #    print "Settings fail to transition to new unity compiz favorites"
        
    def save_launchers(self, widget):
        dialog = MessageBox(parent=self,
                                window_title=_('Save'),
                                action_text=_('Save changes'),
                                message=_('Do you want to save your changes?'),
                                dlg_type=DialogType.DIALOG_CONFIRMATION
                                )
        result, text = dialog.run()
        if result:
            launchers = []
            group_list = []
            for row in self.launcher_view.get_rows():
                file_path = normalize_path(row["path"])
                fli = open(file_path, 'w')
                obj = row["objDesktop"]
                #set the ayatana list to current order and visibility
                for group_item in self.group_view.get_rows():
                    if group_item['Visible']:
                        group_name = remove_html_tags(group_item['GroupName'])
                        group_list.append( group_name) 
                if len(group_list)>0:
                    obj.set('X-Ayatana-Desktop-Shortcuts', group_list)
                obj.write(fli)
                fli.close()
                launchers.append(file_path)
                
            if self.__save_unity_list(launchers):
                MessageBox(parent=self,
                                window_title=_('Information'),
                                action_text=_('Changes saved'),
                                message=_('Your changes were successfuly saved.\n\n To apply the changes to your launcher you will need to <b>logout</b> and login again.'),
                                dlg_type=DialogType.DIALOG_MESSAGE
                                ).run()
            else:
                MessageBox(parent = self,
                                window_title=_('Error'),
                                action_text=_('Changes not saved'),
                                message=_("There's a problem with your application, \n controller not found."),
                                dlg_type=DialogType.DIALOG_MESSAGE
                                ).run()
                                    