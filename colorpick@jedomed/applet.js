const Applet = imports.ui.applet;
const Lang = imports.lang;
const GLib = imports.gi.GLib;
const UUID = "colorpick@jedomed";
const Gettext = imports.gettext;

Gettext.bindtextdomain(UUID, GLib.get_home_dir() + "/.local/share/locale");

function _(str) {
	return Gettext.dgettext(UUID, str);
}

function ConfirmDialog(){
    this._init();
}

function MyApplet(metadata, orientation, panelHeight, instanceId) {
	this._init(metadata, orientation, panelHeight, instanceId);
}

MyApplet.prototype = {
	__proto__: Applet.IconApplet.prototype,

	_init: function(metadata, orientation, panelHeight, instanceId) {
		Applet.IconApplet.prototype._init.call(this, orientation, panelHeight, instanceId);

		try {
			this.set_applet_icon_symbolic_name("color-management");
			this.set_applet_tooltip(_("Open ColorPick"));

		}
		catch (e) {
			global.logError(e);
		}
	},

	on_applet_clicked: function(event) {
		// pick one, change paths
		GLib.spawn_command_line_async("/path/to/wherever/you/saved/it/./colorpick");
		//GLib.spawn_command_line_async("bash /path/to/wherever/you/saved/it/colorpick_on_top");
	},

};

function main(metadata, orientation, panelHeight, instanceId) {
	let myApplet = new MyApplet(metadata, orientation, panelHeight, instanceId);
	return myApplet;
}
