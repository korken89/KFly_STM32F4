import gtk
from unitylaunchereditor.core.translation import _
class DialogType:
    """ Class to define constants for define Message class types"""
    (
        DIALOG_INPUT,
        DIALOG_MESSAGE,
        DIALOG_CONFIRMATION
    ) = range(3)

class FileChooser(gtk.FileChooserDialog):
    def __init__(self, title='Open...', filters=None):
        super(FileChooser, self).__init__(
                                    title,
                                    None,
                                    gtk.FILE_CHOOSER_ACTION_OPEN, (
                                   gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,
                                   gtk.STOCK_OPEN, gtk.RESPONSE_OK)
                                    )
        self.set_default_response(gtk.RESPONSE_OK)
        if filters:
            self.add_filter(filters)

    def run(self, destroy=True):
        """Returns True if yes was clicked, False otherwise."""
        resp = super(FileChooser, self).run()

        filename = self.get_filename()
        if destroy:
            self.destroy()
        if resp == gtk.RESPONSE_OK:
            return True, filename
        else:
            return False, None
        
class MessageBox(gtk.MessageDialog):
    def __init__(self, 
                 window_title="",
                 parent=None,
                 action_text="",
                 message="",
                 dlg_type=DialogType.DIALOG_MESSAGE,
                 label_text=None):
        
        text = "<b>%s</b>\n\n%s" % (action_text, message)
        
        self.dialog_type = dlg_type
        
        if dlg_type == DialogType.DIALOG_MESSAGE:
            icon = gtk.MESSAGE_WARNING
            buttons = gtk.BUTTONS_OK 
        elif dlg_type == DialogType.DIALOG_INPUT:
            icon = gtk.MESSAGE_QUESTION
            buttons = gtk.BUTTONS_CANCEL
        elif dlg_type == DialogType.DIALOG_CONFIRMATION:
            icon = gtk.MESSAGE_QUESTION
            buttons = gtk.BUTTONS_YES_NO
            
        super(MessageBox, self).__init__(parent,
                                         gtk.DIALOG_MODAL | gtk.DIALOG_DESTROY_WITH_PARENT,
                                         icon, buttons)
        self.set_title(window_title)
        self.set_markup(text)
        self.set_transient_for(parent)
        if dlg_type == DialogType.DIALOG_INPUT:
            
            # create a horizontal box to pack the entry and a label
            self.text = gtk.Entry()
            self.text.set_activates_default(gtk.TRUE)
            hbox = gtk.HBox()
            hbox.pack_start(gtk.Label(label_text), False, 5, 5)
            hbox.pack_end(self.text)
            self.vbox.pack_end(hbox, True, True, 0)
            self.ok_button = self.add_button(gtk.STOCK_OK, gtk.RESPONSE_OK)
            self.ok_button.grab_default()
           
            self.ok_button.set_sensitive(False)
            self.set_alternative_button_order([gtk.RESPONSE_CANCEL, gtk.RESPONSE_OK])
            self.set_default_response(gtk.RESPONSE_OK)
            self.text.connect('changed', lambda widget: self.ok_button.set_sensitive(widget.get_text_length() > 0))
            
        self.show_all()
    
    def run(self, destroy=True):
        """Returns True if yes was clicked, False otherwise."""
        resp = super(MessageBox, self).run()
        dialog_type = self.dialog_type
        text = None
        if self.dialog_type == DialogType.DIALOG_INPUT:
            text = self.text.get_text()
        if destroy:
            self.destroy()
        if resp == gtk.RESPONSE_YES or resp == gtk.RESPONSE_OK:
            if dialog_type == DialogType.DIALOG_INPUT:
                return True, text
            else:
                return True, text
        else:
            return False, text

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

