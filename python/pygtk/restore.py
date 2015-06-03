#!/usr/bin/env python

import pty
import os
import gtk
import glib
import pango
import subprocess
import logging

# Logging
log = logging.getLogger('restore')
log.setLevel(logging.DEBUG)
formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s', "%a, %d %b %Y %H:%M:%S")

# File Hnadler
handler = logging.FileHandler('/tmp/restore.log')
handler.setLevel(logging.DEBUG)
handler.setFormatter(formatter)

# Console Handler
ch = logging.StreamHandler()
ch.setLevel(logging.INFO)
ch.setFormatter(formatter)

# Add the handlers defined above
log.addHandler(handler)
log.addHandler(ch)

class CommandTextView(gtk.TextView):
    '''TextView that reads output of a command synchronously. From:
    https://pygabriel.wordpress.com/2009/07/27/redirecting-the-stdout-on-a-gtk-textview/
    '''
    def __init__(self, command):
        super(CommandTextView, self).__init__()
        self.command = command

    def run(self):
        master, slave = pty.openpty()
        log.info("Running %s" % self.command)
        proc = subprocess.Popen(self.command, stdout=slave, stderr=slave, shell=True)
        glib.io_add_watch(os.fdopen(master), glib.IO_IN, self.write_to_buffer)

    def write_to_buffer(self, fd, condition):
        if condition == glib.IO_IN:
            char = fd.readline()
            buf = self.get_buffer()
            buf.insert_at_cursor(char)
            return True
        else:
            return False

class Restore:
    def __init__(self):
        log.info("Initializing Restore Class")

        # Main Window
        self.window = gtk.Window()
        self.window.set_default_size(800, 480)
        self.window.set_title("Logicube System Restore")
        self.window.set_border_width(10)

        ## Vertical Box container
        vbox = gtk.VBox()

        ## Buttons
        bbox = gtk.HButtonBox()
        bbox.set_layout(gtk.BUTTONBOX_SPREAD)

        ### Restore Button
        restore_image = gtk.Image()
        restore_image.set_from_stock(gtk.STOCK_REFRESH,
                gtk.ICON_SIZE_BUTTON)
        restore = gtk.Button()
        restore.add(restore_image)

        ### Poweroff Button
        poweroff_image = gtk.Image()
        poweroff_image.set_from_stock(gtk.STOCK_CANCEL,
                gtk.ICON_SIZE_BUTTON)
        poweroff = gtk.Button()
        poweroff.add(poweroff_image)

        bbox.add(restore)
        bbox.add(poweroff)

        ## Horizontal Box container
        hbox = gtk.HBox()

        # Scrolled Window
        scrolledwindow = gtk.ScrolledWindow()
        scrolledwindow.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
        vadj = scrolledwindow.get_vadjustment()
        vadj.connect('changed', self.rescroll, scrolledwindow)

        ## Toolbar
        toolbar = gtk.Toolbar()
        settings = gtk.ToolButton(gtk.STOCK_PREFERENCES)
        #restore.set_label("Reflash")
        #poweroff = gtk.ToolButton(gtk.STOCK_QUIT)
        #poweroff.set_label("Power Off")
        title = gtk.Label()
        title.set_markup("<b>Logicube System Restore</b>")

        # Text View
        view = CommandTextView("ls -ltrRh /etc")
        view.set_editable(False)
        view.set_cursor_visible(False)

        # Events
        self.window.connect("destroy", lambda q: gtk.main_quit())
        restore.connect("clicked", self.restore_system, view, None)
        #poweroff.connect("clicked", self.poweroff_system, view, None)

        # Draw
        self.window.add(vbox)
        scrolledwindow.add(view)
        vbox.pack_start(toolbar, False, False, 10)
        vbox.pack_start(title, False, False, 10)
        vbox.pack_start(bbox, False, False, 10)
        toolbar.insert(settings, 0)
        vbox.pack_start(scrolledwindow)

        # Done
        self.window.show_all()
        log.info("Done with initialization")

    def rescroll(self, adj, scroll):
        adj.set_value(adj.upper-adj.page_size)
        scroll.set_vadjustment(adj)

    def restore_system(self, widget, view, event):
        dialog = gtk.MessageDialog(self.window, gtk.DIALOG_MODAL,
                gtk.MESSAGE_INFO, gtk.BUTTONS_YES_NO, "Are you sure you "
                "want to reflash the system? All Settings and Profiles "
                "will be lost!")
        dialog.set_title("Restore")

        response = dialog.run()
        dialog.destroy()
        if response == gtk.RESPONSE_YES:
            log.info("User chose YES...Starting system restore")

            #  Disable restore button
            widget.set_sensitive(False)

            # Font
            fontdesc = pango.FontDescription("monospace 11")
            view.modify_font(fontdesc)

            # Output
            view.run()
            return False
        else:
            return True

    def poweroff_system(self, widget, event, data):
        dialog = gtk.MessageDialog(self.window, gtk.DIALOG_MODAL,
                gtk.MESSAGE_INFO, gtk.BUTTONS_YES_NO, "Are you sure you "
                "want to poweroff the unit?")
        dialog.set_title("Power Off")

        response = dialog.run()
        dialog.destroy()
        if response == gtk.RESPONSE_YES:
            log.info("User chose YES...Powering off system")
            subprocess.call("sudo poweroff", shell=True)
            return False
        else:
            return True

if __name__ == '__main__':
    Restore()
    gtk.main()

