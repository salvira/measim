#pragma once

#include "measim_run.hh"
#include <s_entry.hh>
#include <gltk.hh>

class TraceView :
	public virtual gltk::Widget
{
	MEAsim const * vol;
	void on_draw();
	void on_button_event(int button, int state, int x, int y);
	size_t num_rec;
	size_t buf_start;
	double * x_val;
	double * y_val;
        double * z_val;
        double * a_val;
	gltk::Window * fdlg; // file dialog
	gltk::Entry fn_entry;
	void file_select(std::string name);
	void file_ok();
public:
	void add_record();
	double last_t;
	double delta_t;

	TraceView(MEAsim * vol);
	~TraceView();
};
