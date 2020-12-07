#include <gtk/gtk.h>
#include <math.h>

GtkBuilder *builder; 
GtkWidget *window;

GtkEntry
	*textRgba,
	*textHex;

GtkSwitch *hsv_switch;
gboolean hsvOn;

GtkSpinButton
	*slider_r,
	*slider_g,
	*slider_b,
	*slider_a,
	*slider_h,
	*slider_s,
	*slider_v;

GtkColorChooser *picker;
GdkRGBA color;

//ref: https://www.tutorialspoint.com/c-program-to-change-rgb-color-model-to-hsv-color-model
float getMax(float a, float b, float c) {
	return ((a>b) ? (a>c ? a:c) : (b>c ? b:c));
}
float getMin(float a, float b, float c) {
	return ((a<b) ? (a<c ? a:c) : (b<c ? b:c));
}
void printRgbToHsv(float r, float g, float b) {
	float h, s, v;
	float max = getMax(r, g, b); 
	float min = getMin(r, g, b); 
	float diff = max-min;
	if (max == min) {
		h = 0;
	} else if (max == r) {
		h = fmod((60*((g-b)/diff)+360), 360.0);
	} else if (max == g) {
		h = fmod((60*((b-r)/diff)+120), 360.0);
	} else if (max == b) {
		h = fmod((60*((r-g)/diff)+240), 360.0);
	}
	
	if (max == 0) {
		s = 0;
	} else {
		s = (diff / max) * 100;
	}
	v = max * 100;
	
	gtk_spin_button_set_value(slider_h, h);
	gtk_spin_button_set_value(slider_s, s);
	gtk_spin_button_set_value(slider_v, v);
}

//convert hsv to rgb and update sliders
void printHsvToRgb(float h, float s, float v) {
	s /= 100;
	v /= 100;
	
	float c = v * s;
	float x;
	
	float m = v - c;
	float r, g, b;
	
	if(h >= 0 && h < 60) {
		x = c * (h/60);
		
		r = c;
		g = x;
		b = 0;
	} else if(h >= 60 && h < 120) {
		x = c * (1-((h/60)-1));
		
		r = x;
		g = c;
		b = 0;
	} else if(h >= 120 && h < 180) {
		x = c * ((h/60)-2);
		
		r = 0;
		g = c;
		b = x;
	} else if(h >= 180 && h < 240) {
		x = c * (1-((h/60)-3));
		
		r = 0;
		g = x;
		b = c;
	} else if(h >= 240 && h < 300) {
		x = c * ((h/60)-4);
		
		r = x;
		g = 0;
		b = c;
	} else {
		x = c * (1-((h/60)-5));
		
		r = c;
		g = 0;
		b = x;
	}
	
	//printf("%f - ", h);
	
	r = (r+m)*255;
	g = (g+m)*255;
	b = (b+m)*255;

	
	//FIXME: changing s/v changes hue... most likely to do with the "x" trainwreck
	//HSV on/off switch until it's fixed
	gtk_spin_button_set_value(slider_r, round(r));
	gtk_spin_button_set_value(slider_g, round(g));
	gtk_spin_button_set_value(slider_b, round(b));
}

//convert to hex and print to hex entry
void printRgbToHex(int r_int, int g_int, int b_int)
{
	char r[9];
	char g[9];
	char b[9];
	
	sprintf(r, "%.2x", r_int);
	sprintf(g, "%.2x", g_int);
	sprintf(b, "%.2x", b_int);
	
	char result[8]; //1 '#' +  6 hex chars + 1 null. but that throws a warning so w/e
	sprintf(result, "#%.2s%.2s%.2s", r, g, b); //.2 so it won't go over the 6 char limit
	
	gtk_entry_set_text(textHex, result);
}


//update color
void updateColor() {
	color.alpha = (int) (color.alpha*100); //round to 2 decimal places
	color.alpha = color.alpha/100;
	
	gtk_color_chooser_set_rgba(picker, &color); //set colorpicker
	gtk_entry_set_text(textRgba, gdk_rgba_to_string(&color)); //set rgba entry
	printRgbToHex(round(color.red*255), round(color.green*255), round(color.blue*255)); //set hex entry
	
	//sliders
	gtk_spin_button_set_value(slider_r, color.red*255);
	gtk_spin_button_set_value(slider_g, color.green*255);
	gtk_spin_button_set_value(slider_b, color.blue*255);
	gtk_spin_button_set_value(slider_a, color.alpha);
	if(hsvOn) printRgbToHsv(color.red, color.green, color.blue);
}
//update color from entry
void updateColorFromEntry(GtkEntry *entry)
{
	gdk_rgba_parse(&color, gtk_entry_get_text(entry)); //parse from entry
	updateColor();
	
}

//when colorpicker changed
void on_colorpicker_set()
{
	gtk_color_chooser_get_rgba(picker, &color); //get color
	updateColor();
}

//hsv switch toggled
void hsv_toggle()
{
	hsvOn = gtk_switch_get_active(hsv_switch);
	gtk_switch_set_active(hsv_switch, hsvOn);
	//sliders
	gtk_editable_set_editable(GTK_EDITABLE(slider_h), hsvOn);
	gtk_editable_set_editable(GTK_EDITABLE(slider_s), hsvOn);
	gtk_editable_set_editable(GTK_EDITABLE(slider_v), hsvOn);
}

//main
int main(int argc, char *argv[])
{
	gtk_init(&argc, &argv);

	builder = gtk_builder_new_from_resource("/window/window.glade");

	window = GTK_WIDGET(gtk_builder_get_object(builder, "window")); //get window
	gtk_builder_connect_signals(builder, NULL);
	
	picker = GTK_COLOR_CHOOSER(gtk_builder_get_object(builder, "colorpicker")); //get colorpicker
	
	textRgba = GTK_ENTRY(gtk_builder_get_object(builder, "text_rgba")); //get RGBA entry
	textHex = GTK_ENTRY(gtk_builder_get_object(builder, "text_hex")); //get HEX entry
	
	//HSV switch
	hsv_switch = GTK_SWITCH(gtk_builder_get_object(builder, "hsv_switch"));
	hsvOn = gtk_switch_get_state(hsv_switch);
	
	//this sucks ass
	//get slider
	slider_r = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "slider_r"));
	slider_g = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "slider_g")); 
	slider_b = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "slider_b"));
	slider_a = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "slider_a"));
	slider_h = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "slider_h")); 
	slider_s = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "slider_s"));
	slider_v = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "slider_v"));
	//set minmax
	gtk_spin_button_set_range(slider_r, 0, 255);
	gtk_spin_button_set_range(slider_g, 0, 255);
	gtk_spin_button_set_range(slider_b, 0, 255);
	gtk_spin_button_set_range(slider_a, 0, 1);
	gtk_spin_button_set_range(slider_h, 0, 359);
	gtk_spin_button_set_range(slider_s, 0, 100);
	gtk_spin_button_set_range(slider_v, 0, 100);
	//set steps
	gtk_spin_button_set_increments(slider_r, 1, 1);
	gtk_spin_button_set_increments(slider_g, 1, 1);
	gtk_spin_button_set_increments(slider_b, 1, 1);
	gtk_spin_button_set_increments(slider_a, 0.01, 0.01);
	gtk_spin_button_set_increments(slider_h, 1, 1);
	gtk_spin_button_set_increments(slider_s, 1, 1);
	gtk_spin_button_set_increments(slider_v, 1, 1);

	//activate on enter key
	g_signal_connect(textRgba, "activate", G_CALLBACK(updateColorFromEntry), textRgba);
	g_signal_connect(textHex, "activate", G_CALLBACK(updateColorFromEntry), textRgba);
	
	//get and set default values
	on_colorpicker_set();

	g_object_unref(builder);

	hsv_toggle(); //to disable the switches at start

	gtk_widget_show(window);				
	gtk_main();

	return 0;
}

//TODO: there must be a better way 
//change red
void change_r()
{
	color.red = ((double) gtk_spin_button_get_value_as_int(slider_r))/255;
	updateColor();
}

//change green
void change_g()
{
	color.green = ((double) gtk_spin_button_get_value_as_int(slider_g))/255;
	updateColor();
}

//change blue
void change_b()
{
	color.blue = ((double) gtk_spin_button_get_value_as_int(slider_b))/255;
	updateColor();
}

//change alpha
void change_a()
{
	color.alpha = gtk_spin_button_get_value(slider_a);
	updateColor();
}

//change hue, saturation, value
void change_hsv()
{
	if(hsvOn) {
		printHsvToRgb(gtk_spin_button_get_value(slider_h), gtk_spin_button_get_value(slider_s), gtk_spin_button_get_value(slider_v));
		updateColor();
	}
}

//exit
void on_window_destroy()
{
	gtk_main_quit();
}

