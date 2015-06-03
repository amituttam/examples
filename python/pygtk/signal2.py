#!/usr/bin/env python

import gtk

class Signal:
    def __init__(self):
        window = gtk.Window()
        
        table = gtk.Table(2, 3, True)
        self.label = gtk.Label()
        button_connect = gtk.Button("Connect")
        button_disconnect = gtk.Button("Disconnect")
        button_disconnect.set_sensitive(False)
        self.button_status = gtk.Button("Status")
        
        window.connect("destroy", lambda q: gtk.main_quit())
        button_connect.connect("clicked", self.signal_connected, button_disconnect)
        button_disconnect.connect("clicked", self.signal_disconnected, button_connect)

        window.add(table)
        table.attach(self.label, 0, 3, 0, 1)
        table.attach(button_connect, 0, 1, 1, 2)
        table.attach(button_disconnect, 1, 2, 1, 2)
        table.attach(self.button_status, 2, 3, 1, 2)
        window.show_all()
    
    def signal_connected(self, button_connect, button_disconnect):
        self.handler_id = self.button_status.connect("clicked", self.signal_status)
        self.label.set_text("Status Button connected with id of %s" % str(self.handler_id))
        button_connect.set_sensitive(False)
        button_disconnect.set_sensitive(True)
    
    def signal_disconnected(self, button_disconnect, button_connect):
        if self.button_status.handler_is_connected(self.handler_id):
            self.button_status.disconnect(self.handler_id)
            self.label.set_text("Status Button disconnected")
            button_connect.set_sensitive(True)
            button_disconnect.set_sensitive(False)
    
    def signal_status(self, button_status):
        print "Status Button connected with id of %s" % str(self.handler_id)

Signal()
gtk.main()
