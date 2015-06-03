#!/usr/bin/env python

import gtk

class Image:
    def __init__(self):
        window = gtk.Window()
        window.set_default_size(800, 600)
        vbox = gtk.VBox(False, 5)
        scrolledwindow = gtk.ScrolledWindow()
        self.image = gtk.Image()
        self.button = gtk.FileChooserButton("Select Image", None)
        window.connect("destroy", lambda w: gtk.main_quit())
        self.button.connect("file-set", self.load_image)
        window.add(vbox)
        vbox.pack_start(scrolledwindow, True, True, 0)
        vbox.pack_start(self.button, False, False, 0)
        scrolledwindow.add_with_viewport(self.image)
        window.show_all()

    def load_image(self, widget):
        self.image.set_from_file(self.button.get_filename())

Image()
gtk.main()
