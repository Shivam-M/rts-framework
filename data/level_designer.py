import tkinter as tk
from tkinter import ttk, filedialog, colorchooser
from contextlib import suppress
import json


# Bespoke UI/Level JSON editor for rts_framework


DROPDOWN_OPTION_KEYS = {
    "type": ["PANEL", "MOVEABLE", "TEXT", "BUTTON"]
}


class UIPreviewCanvas(tk.Canvas):
    def __init__(self, master, json_data, **kwargs):
        super().__init__(master, **kwargs)
        self.json_data = json_data
        self.zoom_scale = 1.0
        self.zoom_factor = 1.1
        self.offset_location = [0.0, 0.0]
        self.offset_size = [1.0, 1.0]

        self.bind("<MouseWheel>", self.zoom)

    def zoom(self, event):
        if event.num == 4 or event.delta > 0:
            scale_factor = self.zoom_factor
        else:
            scale_factor = 1 / self.zoom_factor

        self.zoom_scale *= scale_factor

        x = self.canvasx(event.x)
        y = self.canvasy(event.y)

        self.scale("all", x, y, scale_factor, scale_factor)
        self.offset_size[0] *= scale_factor
        self.offset_size[1] *= scale_factor

        self.configure(scrollregion=self.bbox("all"))

        # self.redraw()

    def hex_to_rgba(self, hex_colour, alpha=1.0):
        hex_colour = hex_colour.lstrip('#')
        
        r = int(hex_colour[:2], 16)
        g = int(hex_colour[2:4], 16)
        b = int(hex_colour[4:], 16)
        
        if alpha < 1.0:
            r = int(r * alpha + 255 * (1 - alpha))
            g = int(g * alpha + 255 * (1 - alpha))
            b = int(b * alpha + 255 * (1 - alpha))
        
        return f'#{r:02x}{g:02x}{b:02x}'
        
    def redraw(self):
        self.delete("all")

        bg_colour = f"#{self.json_data.get('background', 'F0F0F0')}"
        if bg_colour == "#FFFFFF":
            bg_colour = "#F0F0F0"
        self.configure(bg=bg_colour)
        self.config(background=bg_colour)

        width = self.winfo_width()
        height = self.winfo_height()

        self.offset_location = [0.0, 0.0]
        self.offset_size = [1.0, 1.0]

        for obj in self.json_data.get("objects", []):
            object_type = obj["type"]
            base_colour = obj.get('colour', 'bdc3c7')

            if base_colour.startswith("&"):
                base_colour = self.json_data["variables"].get(base_colour[1:], 'bdc3c7')
            
            colour = f"#{base_colour}"

            x = (obj.get("x", 0.00) * self.offset_size[0] + self.offset_location[0]) * width
            y = (obj.get("y", 0.00) * self.offset_size[1] + self.offset_location[1]) * height
            w = obj.get("width", 1.00) * self.offset_size[0] * width
            h = obj.get("height", 1.00) * self.offset_size[1] * height

            colour = f"#{base_colour}"

            if object_type == "PANEL":
                self.create_rectangle(x, y, x + w, y + h, outline="blue", dash=(2, 2))
                self.offset_location = [obj.get("x", 0.00), obj.get("y", 0.00)]
                self.offset_size = [obj.get("width", 1.00), obj.get("height", 1.00)]
            elif object_type == "BUTTON":
                self.create_rectangle(x, y, x + w, y + h, fill=colour, outline="gray")
                text_size = max(int(obj.get("size", 20) * obj.get("scale", 1.0) * self.zoom_scale), 1) // 2
                self.create_text(x + w / 2, y + h / 2, text=obj.get("content", "Button"), fill="black", font=(obj.get("font", "TkDefaultFont"), text_size))  # use alt_colour
            elif object_type == "TEXT":
                text_size = max(int(obj.get("size", 20) * obj.get("scale", 1.0) * self.zoom_scale), 1) // 2

                self.create_text(x, y, text=obj.get("content", "Text"), fill=colour, font=(obj.get("font", "TkDefaultFont"), text_size))  # TODO: support new text alignment
            else:
                self.create_rectangle(x, y, x + w, y + h, fill=colour)


class UIJsonEditor:
    def __init__(self, root):
        self.root = root
        self.root.title("UI JSON Editor")
        self.root.geometry("1400x800+100+100")
        self.current_file = None
        self.data = self.create_default_json()
        self.clipboard = False

        self.setup_ui()
        
    def create_default_json(self):
        return {
            "alt_background": "FFFFFF",
            "alt_background_alpha": 0.00,
            "background": "FFFFFF",
            "background_alpha": 0.00,
            "foreground": "FFFFFF",
            "offset_positions": False,
            "image": "images/test.png",
            "level_name": "UI Level",
            "variables": {
                "name": "ui_panel",
                "font": "consolas",
                "colour": "bdc3c7"
            },
            "objects": []
        }
    
    def setup_ui(self):
        self.main_frame = ttk.PanedWindow(self.root, orient=tk.HORIZONTAL)
        self.main_frame.pack(fill=tk.BOTH, expand=True)
        
        self.left_panel = ttk.Frame(self.main_frame)
        self.center_panel = ttk.Frame(self.main_frame)
        self.right_panel = ttk.Frame(self.main_frame)
        
        self.main_frame.add(self.left_panel, weight=1)
        self.main_frame.add(self.center_panel, weight=2)
        self.main_frame.add(self.right_panel, weight=2)
        
        self.setup_variables_panel()
        self.setup_object_list()
        self.setup_properties_panel()
        self.setup_preview_panel()
        self.setup_menu()

    def setup_preview_panel(self):
        frame_preview = ttk.LabelFrame(self.left_panel, text="Preview")
        frame_preview.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        self.canvas_preview = UIPreviewCanvas(frame_preview, self.data, bg="white")
        self.canvas_preview.pack(fill=tk.BOTH, expand=True)
        
    def refresh_preview(self):
        self.canvas_preview.json_data = self.data
        self.canvas_preview.redraw()

    def setup_variables_panel(self):
        self.frame_variables_container = ttk.Frame(self.left_panel)
        self.frame_variables_container.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)

        self.frame_variables = ttk.LabelFrame(self.frame_variables_container, text="Variables")
        self.frame_variables.pack(fill=tk.BOTH, expand=True, padx=5, pady=(5, 0))

        frame_inner = ttk.Frame(self.frame_variables)
        frame_inner.pack(fill=tk.BOTH, expand=True)

        canvas = tk.Canvas(frame_inner)
        scrollbar = ttk.Scrollbar(frame_inner, orient="vertical", command=canvas.yview)

        self.frame_scroll_variables = ttk.Frame(canvas)
        self.frame_scroll_variables.bind("<Configure>", lambda e: canvas.configure(scrollregion=canvas.bbox("all")))

        canvas.create_window((0, 0), window=self.frame_scroll_variables, anchor="nw")
        canvas.configure(yscrollcommand=scrollbar.set)

        canvas.pack(side="left", fill="both", expand=True)
        scrollbar.pack(side="right", fill="y")

        self.frame_add_variable = ttk.Frame(self.frame_variables_container)
        self.frame_add_variable.pack(fill=tk.X, padx=5, pady=(5, 0), anchor="e")

        self.button_add_variable = ttk.Button(self.frame_add_variable, text="Add Variable", command=self.add_variable)
        self.button_add_variable.pack(side="bottom", anchor="e", pady=0, padx=0)

        self.refresh_variables()

    def add_variable(self):
        window_variable_add = tk.Toplevel(self.root)
        window_variable_add.title("Add Variable")
        window_variable_add.geometry("300x150+650+450")
        
        name_var = tk.StringVar()
        ttk.Label(window_variable_add, text="Name:").grid(row=0, column=0, padx=5, pady=5)
        ttk.Entry(window_variable_add, textvariable=name_var).grid(row=0, column=1, padx=5, pady=5)
        
        value_var = tk.StringVar()
        ttk.Label(window_variable_add, text="Value:").grid(row=1, column=0, padx=5, pady=5)
        ttk.Entry(window_variable_add, textvariable=value_var).grid(row=1, column=1, padx=5, pady=5)
        
        def add():
            if name := name_var.get():
                self.data["variables"][name] = value_var.get()
                self.refresh_variables()
                self.on_select_object(None)
                self.refresh_preview()
                window_variable_add.destroy()
                
        ttk.Button(window_variable_add, text="Add", command=add).grid(row=2, column=0, columnspan=2, pady=10)
        
    def refresh_variables(self):
        if not self.data.get("variables"):
            self.data["variables"] = {}

        for widget in self.frame_scroll_variables.winfo_children():
            widget.destroy()
            
        row = 0
        for key, value in self.data["variables"].items():
            ttk.Label(self.frame_scroll_variables, text=key).grid(row=row, column=0, padx=5, pady=2)
            
            if "colour" in key:
                frame = ttk.Frame(self.frame_scroll_variables)
                frame.grid(row=row, column=1, padx=5, pady=2, sticky="ew")
                
                tk.Canvas(frame, width=20, height=20, bg=f"#{value}").pack(side=tk.LEFT, padx=5)
                ttk.Button(frame, text=f"#{value.upper()}", command=lambda k=key, v=value: self.choose_variable_colour(k)).pack(side=tk.LEFT, fill=tk.X, expand=True)
            else:
                var = tk.StringVar(value=str(value))
                entry = ttk.Entry(self.frame_scroll_variables, textvariable=var, width=40)
                entry.grid(row=row, column=1, padx=5, pady=2, sticky="ew")
                entry.bind('<KeyRelease>', lambda e, k=key, v=var: self.update_variable(k, v.get()))
            
            ttk.Button(self.frame_scroll_variables, text="Delete", command=lambda k=key: self.delete_variable(k)).grid(row=row, column=2, padx=2, pady=2)
            row += 1
            
    def update_variable(self, key: str, value: str):
        self.data["variables"][key] = value
        self.on_select_object(None)
        self.refresh_preview()
        
    def delete_variable(self, key: str):
        if key in self.data["variables"]:
            del self.data["variables"][key]
            self.refresh_variables()
            self.refresh_preview()
    
    def setup_menu(self):
        menu = tk.Menu(self.root)
        
        menu_file = tk.Menu(menu, tearoff=0)
        menu_file.add_command(label="New", command=self.new_file)
        menu_file.add_command(label="Open", command=self.open_file)
        menu_file.add_command(label="Save", command=self.save_file)
        menu_file.add_command(label="Save As", command=self.save_file_as)
        menu.add_cascade(label="File", menu=menu_file)

        menu_edit = tk.Menu(menu, tearoff=0)
        menu_edit.add_command(label="Raw JSON", command=self.json_editor)
        menu.add_cascade(label="Edit", menu=menu_edit)
        
        menu_add = tk.Menu(menu, tearoff=0)
        menu_add.add_command(label="Panel", command=lambda: self.add_object("PANEL"))
        menu_add.add_command(label="Button", command=lambda: self.add_object("BUTTON"))
        menu_add.add_command(label="Text", command=lambda: self.add_object("TEXT"))
        menu_add.add_command(label="Moveable", command=lambda: self.add_object("MOVEABLE"))
        menu.add_cascade(label="Add Object", menu=menu_add)

        self.root.config(menu=menu)
    
    def setup_object_list(self):
        frame_list = ttk.LabelFrame(self.right_panel, text="Objects")
        frame_list.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        self.tree = ttk.Treeview(frame_list)
        self.tree.pack(fill=tk.BOTH, expand=True)
        self.tree.bind('<<TreeviewSelect>>', self.on_select_object)
        
        self.tree.bind("<Control-c>", self.copy_object)
        self.tree.bind("<Control-v>", self.paste_object)
        self.tree.bind("<Delete>", self.delete_selected)
        
    def setup_properties_panel(self):
        self.frame_properties_container = ttk.Frame(self.center_panel)
        self.frame_properties_container.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)

        self.frame_properties = ttk.LabelFrame(self.frame_properties_container, text="Properties")
        self.frame_properties.pack(fill=tk.BOTH, expand=True, padx=5, pady=(5, 0))

        canvas = tk.Canvas(self.frame_properties)
        scrollbar = ttk.Scrollbar(self.frame_properties, orient="vertical", command=canvas.yview)

        self.frame_scroll_properties = ttk.Frame(canvas)
        self.frame_scroll_properties.bind("<Configure>", lambda _: canvas.configure(scrollregion=canvas.bbox("all")))

        canvas.create_window((0, 0), window=self.frame_scroll_properties, anchor="nw")
        canvas.configure(yscrollcommand=scrollbar.set)
        canvas.pack(side="left", fill="both", expand=True)
        scrollbar.pack(side="right", fill="y")

        self.frame_add_property = ttk.Frame(self.frame_properties_container)
        self.frame_add_property.pack(fill=tk.X, padx=5, pady=(5, 0), anchor="e")

        self.button_properties_add = ttk.Button(self.frame_add_property, text="Add Property", command=self.add_property)
        self.button_properties_add.pack(side="bottom", anchor="e", pady=5, padx=5)
        self.button_properties_add.pack_forget()

    def choose_variable_colour(self, key: str):
        current_colour = f"#{self.data['variables'][key]}"
        if colour := colorchooser.askcolor(title="Choose colour", color=current_colour)[1]:
            colour = colour[1:]
            self.data["variables"][key] = colour
            self.refresh_variables()
            self.on_select_object(None)
            self.refresh_preview()

    def add_property(self):
        if not self.tree.selection():
            return

        window_property_add = tk.Toplevel(self.root)
        window_property_add.title("Add Property")
        window_property_add.geometry("300x200")

        name_var = tk.StringVar()
        ttk.Label(window_property_add, text="Name:").grid(row=0, column=0, padx=5, pady=5)
        ttk.Entry(window_property_add, textvariable=name_var).grid(row=0, column=1, padx=5, pady=5)

        type_var = tk.StringVar(value="string")
        combobox_type = ttk.Combobox(window_property_add, textvariable=type_var, values=["string", "int", "float", "bool", "colour"])  # TODO: colour
        combobox_type.grid(row=1, column=1, padx=5, pady=5)
        ttk.Label(window_property_add, text="Type:").grid(row=1, column=0, padx=5, pady=5)

        value_var = tk.StringVar()
        ttk.Label(window_property_add, text="Value:").grid(row=2, column=0, padx=5, pady=5)
        ttk.Entry(window_property_add, textvariable=value_var).grid(row=2, column=1, padx=5, pady=5)

        def add():
            property_type = type_var.get()
            property_value = value_var.get()

            if name := name_var.get():
                index = int(self.tree.item(self.tree.selection()[0])["values"][0])

                with suppress(ValueError):
                    if property_type == "int":
                        property_value = int(property_value)
                    elif property_type == "float":
                        property_value = float(property_value)
                    elif property_type == "bool":
                        property_value = property_value.lower() in ("true", "t", "1", "yes", "y")

                self.data["objects"][index][name] = property_value
                self.refresh_object_list()
                self.on_select_object(None)
                window_property_add.destroy()
                
        ttk.Button(window_property_add, text="Add", command=add).grid(row=3, column=0, columnspan=2, pady=10)

    def resolve_variable_value(self, value):
        if isinstance(value, str) and value.startswith("&"):
            var_name = value[1:]
            resolved_value = self.data["variables"].get(var_name, value)
            return f"{resolved_value.upper()} ({value})"
        return value
        
    def add_object(self, object_type: str):
        new_object = {
            "type": object_type,
            "x": 0.0,
            "y": 0.0,
            "width": 0.2,
            "height": 0.2,
            "parent": "&name"
        }
        
        if object_type == "TEXT":
            new_object.update({
                "font": "data/fonts/MedievalSharp-Book.ttf",
                "font_custom": True,
                "size": 20,
                "scale": 0.5,
                "colour": "bdc3c7",
                "alpha": 0.75,
                "content": "Sample Text"
            })
        elif object_type == "BUTTON":
            new_object.update({
                "action": 0,
                "content": "Button",
                "font": "data/fonts/MedievalSharp-Book.ttf",
                "font_custom": True,
                "size": 20,
                "scale": 0.5,
                "colour": "bdc3c7",
                "alpha": 0.75
            })
        elif object_type == "PANEL":
            new_object.update({
                "name": "&name",
            })
            del new_object["parent"]

        self.data["objects"].append(new_object)
        self.refresh_object_list()
        self.refresh_preview()

    def refresh_object_list(self):
        self.tree.delete(*self.tree.get_children())
        for i, obj in enumerate(self.data["objects"]):
            name = obj.get("name", f"{obj['type']}_{i}")
            self.tree.insert("", "end", text=name, values=(i,))

    def on_select_object(self, _):
        for widget in self.frame_scroll_properties.winfo_children():
            widget.destroy()

        if not (selection := self.tree.selection()):
            self.button_properties_add.pack_forget()
            return

        self.button_properties_add.pack(side="bottom", anchor="e", pady=5, padx=0)
 
        for widget in self.frame_scroll_properties.winfo_children():
            widget.destroy()

        index = int(self.tree.item(selection[0])["values"][0])
        obj = self.data["objects"][index]

        row = 0
        for key, value in sorted(obj.items()):
            label = ttk.Label(self.frame_scroll_properties, text=key)
            label.grid(row=row, column=0, padx=5, pady=2)

            if isinstance(value, bool):
                var = tk.BooleanVar(value=value)
                widget = ttk.Checkbutton(self.frame_scroll_properties, variable=var, command=lambda k=key, v=var: self.update_property(index, k, v.get()))
                widget.grid(row=row, column=1, columnspan=2, padx=5, pady=2, sticky="ew")
            elif isinstance(value, float):
                var = tk.DoubleVar(value=value)
                # widget = ttk.Entry(self.frame_scroll_properties, textvariable=var, width=40)
                widget = ttk.Spinbox(self.frame_scroll_properties, format="%.2f", from_=-999, to=999, increment=0.01, textvariable=var)
                widget.bind('<KeyRelease>', lambda e, k=key, v=var: self.update_numeric_property(index, k, v.get()))
                widget.bind('<ButtonPress>', lambda e, k=key, v=var: self.update_numeric_property(index, k, v.get()))
                widget.bind('<ButtonRelease>', lambda e, k=key, v=var: self.update_numeric_property(index, k, v.get()))
                widget.grid(row=row, column=1, columnspan=2, padx=5, pady=2, sticky="ew")
                # widget.set(value)
            elif isinstance(value, int):
                var = tk.IntVar(value=value)
                widget = ttk.Entry(self.frame_scroll_properties, textvariable=var, width=40)
                widget = ttk.Spinbox(self.frame_scroll_properties, from_=-999, to=999, increment=1, textvariable=var)
                widget.bind('<KeyRelease>', lambda e, k=key, v=var: self.update_numeric_property(index, k, v.get()))
                widget.bind('<ButtonPress>', lambda e, k=key, v=var: self.update_numeric_property(index, k, v.get()))
                widget.bind('<ButtonRelease>', lambda e, k=key, v=var: self.update_numeric_property(index, k, v.get()))
                widget.grid(row=row, column=1, columnspan=2, padx=5, pady=2, sticky="ew")
            else:
                var = tk.StringVar(value=str(value))
                if "colour" in key:
                    frame = ttk.Frame(self.frame_scroll_properties)
                    frame.grid(row=row, column=1, columnspan=2, padx=5, pady=2, sticky="ew")
                    
                    resolved_value = value if not value.startswith("&") else self.data["variables"].get(value[1:], value)
                    colour_preview = tk.Canvas(frame, width=20, height=20, bg=f"#{resolved_value}")
                    colour_preview.pack(side=tk.LEFT, padx=5)
                    
                    widget = ttk.Button(frame, text=f"#{self.resolve_variable_value(value)}", command=lambda k=key, current=value: self.choose_colour(index, k, current))
                    widget.pack(side=tk.LEFT, fill=tk.X, expand=True)
                elif key in ("x", "y", "width", "height"):
                    widget = ttk.Spinbox(self.frame_scroll_properties, from_=-1, to=2)
                    widget.grid(row=row, column=1, columnspan=2, padx=5, pady=2, sticky="ew")
                else:
                    widget = ttk.Entry(self.frame_scroll_properties, textvariable=var, width=40)
                    widget.bind('<KeyRelease>', lambda e, k=key, v=var: self.update_property(index, k, v.get()))
                    widget.grid(row=row, column=1, columnspan=2, padx=5, pady=2, sticky="ew")
                    
                    # if isinstance(value, str) and value.startswith("&"):
                    #    resolved_label = ttk.Label(self.scrollable_frame, text=f"[{self.data['variables'].get(value[1:], '')}]")
                    #    resolved_label.grid(row=row, column=3, padx=5, pady=2)
            
            if key not in ("type", "name"):  # loosen?
                button_delete = ttk.Button(self.frame_scroll_properties, text="Delete", command=lambda idx=index, k=key: self.delete_property(idx, k))
                button_delete.grid(row=row, column=3, padx=0, pady=2)
            
            row += 1
            
    def delete_property(self, object_index: int, key: str):
        if key in self.data["objects"][object_index]:
            del self.data["objects"][object_index][key]
            self.refresh_object_list()
            if current_selection := self.tree.selection():
                self.tree.selection_set(current_selection)
                self.on_select_object(None)
            self.refresh_preview()

    def update_property(self, object_index, key, value):
        self.data["objects"][object_index][key] = value
        self.refresh_preview()

    def update_numeric_property(self, object_index: int, key: str, value):
        with suppress(ValueError):
            if '.' in str(value):
                self.data["objects"][object_index][key] = float(value)
            else:
                self.data["objects"][object_index][key] = int(value)
            self.refresh_preview()

    def choose_colour(self, object_index: int, key: str, current_colour: str):
        if current_colour.startswith("&"):
            current_colour = self.data["variables"].get(current_colour[1:], "FFFFFF")
        if colour := colorchooser.askcolor(title="Choose colour", color=f"#{current_colour}")[1]:
            colour = colour[1:]
            self.data["objects"][object_index][key] = colour
            self.on_select_object(None)
            self.refresh_preview()

    def delete_selected(self, _):
        if not (selection := self.tree.selection()):
            return

        index = int(self.tree.item(selection[0])["values"][0])
        self.data["objects"].pop(index)
        self.refresh_object_list()
        self.refresh_preview()
        self.button_properties_add.pack_forget()

    def copy_object(self, _):
        if not (selection := self.tree.selection()):
            return

        index = int(self.tree.item(selection[0])["values"][0])  # copy all selected instead
        self.clipboard = self.data["objects"][index].copy()

    def paste_object(self, _):
        if self.clipboard:
            new_object = self.clipboard.copy()
            self.data["objects"].append(new_object)
            self.refresh_object_list()
            self.refresh_preview()
    
    def json_editor(self):
        window_json_editor = tk.Toplevel(self.root)
        window_json_editor.title("Raw JSON Editor")
        window_json_editor.geometry("500x600+550+225")

        text = tk.Text(window_json_editor)
        text.insert(tk.END, json.dumps(self.data, indent=4))
        text.pack(padx=0, pady=0)

        button_set = None
        button_discard = None

    def new_file(self):
        self.current_file = None
        self.data = self.create_default_json()
        self.refresh_object_list()
        self.refresh_variables()
        self.refresh_preview()

    def open_file(self):
        if filename := filedialog.askopenfilename(filetypes=[("JSON files", "*.json"), ("All files", "*.*")]):
            with open(filename, 'r') as f:
                self.data = json.load(f)
            self.current_file = filename
            self.refresh_object_list()
            self.refresh_variables()
            self.refresh_preview()

    def save_file(self):
        if not self.current_file:
            self.save_file_as()
        else:
            with open(self.current_file, 'w') as file:
                json.dump(self.data, file, indent=2)

    def save_file_as(self):
        if filename := filedialog.asksaveasfilename(defaultextension=".json", filetypes=[("JSON files", "*.json"), ("All files", "*.*")]):
            self.current_file = filename
            with open(filename, 'w') as file:
                json.dump(self.data, file, indent=2)


if __name__ == "__main__":
    root = tk.Tk()
    app = UIJsonEditor(root)
    root.mainloop()
