#!/usr/bin/env python

# example buttonbox.py

import gtk

class ButtonBoxExample:
    # Create a Button Box with the specified parameters
    def create_bbox(self, horizontal, title, spacing,
                    child_w, child_h, layout):
        frame = gtk.Frame(title)

        if horizontal:
            bbox = gtk.HButtonBox()
        else:
            bbox = gtk.VButtonBox()

        bbox.set_border_width(5)
        frame.add(bbox)

        # Set the appearance of the Button Box
        bbox.set_layout(layout)
        bbox.set_spacing(spacing)
        bbox.set_child_size(child_w, child_h)

        button = gtk.Button("OK")
        bbox.add(button)

        button = gtk.Button("Cancel")
        bbox.add(button)

        button = gtk.Button("Help")
        bbox.add(button)

        return frame

    def __init__(self):
        window = gtk.Window(gtk.WINDOW_TOPLEVEL)
        window.set_title("Button Boxes")

        window.connect("destroy", gtk.mainquit)

        window.set_border_width(10)

        main_vbox = gtk.VBox(gtk.FALSE, 0)
        window.add(main_vbox)

        frame_horz = gtk.Frame("Horizontal Button Boxes")
        main_vbox.pack_start(frame_horz, gtk.TRUE, gtk.TRUE, 10)

        vbox = gtk.VBox(gtk.FALSE, 0)
        vbox.set_border_width(10)
        frame_horz.add(vbox)

        vbox.pack_start(self.create_bbox(gtk.TRUE, "Spread (spacing 40)",
                                         40, 85, 20, gtk.BUTTONBOX_SPREAD),
                        gtk.TRUE, gtk.TRUE, 0)

        vbox.pack_start(self.create_bbox(gtk.TRUE, "Edge (spacing 30)",
                                         30, 85, 20, gtk.BUTTONBOX_EDGE),
                        gtk.TRUE, gtk.TRUE, 5)

        vbox.pack_start(self.create_bbox(gtk.TRUE, "Start (spacing 20)",
                                         20, 85, 20, gtk.BUTTONBOX_START),
                        gtk.TRUE, gtk.TRUE, 5)

        vbox.pack_start(self.create_bbox(gtk.TRUE, "End (spacing 10)",
                                         10, 85, 20, gtk.BUTTONBOX_END),
                        gtk.TRUE, gtk.TRUE, 5)

        frame_vert = gtk.Frame("Vertical Button Boxes")
        main_vbox.pack_start(frame_vert, gtk.TRUE, gtk.TRUE, 10)

        hbox = gtk.HBox(gtk.FALSE, 0)
        hbox.set_border_width(10)
        frame_vert.add(hbox)

        hbox.pack_start(self.create_bbox(gtk.FALSE, "Spread (spacing 5)",
                                         5, 85, 20, gtk.BUTTONBOX_SPREAD),
                        gtk.TRUE, gtk.TRUE, 0)

        hbox.pack_start(self.create_bbox(gtk.FALSE, "Edge (spacing 30)",
                                         30, 85, 20, gtk.BUTTONBOX_EDGE),
                        gtk.TRUE, gtk.TRUE, 5)

        hbox.pack_start(self.create_bbox(gtk.FALSE, "Start (spacing 20)",
                                         20, 85, 20, gtk.BUTTONBOX_START),
                        gtk.TRUE, gtk.TRUE, 5)

        hbox.pack_start(self.create_bbox(gtk.FALSE, "End (spacing 20)",
                                         20, 85, 20, gtk.BUTTONBOX_END),
                        gtk.TRUE, gtk.TRUE, 5)

        window.show_all()

def main():
    # Enter the event loop
    gtk.mainloop()
    return 0

if __name__ == "__main__":
    ButtonBoxExample()
    main()
