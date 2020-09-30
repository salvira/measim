#include "trace_view.hh"
#include <fstream>
void TraceView::on_draw()
{
	double ww = get_width();
	double hh = get_height();

	glClearColor(1, 1, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  
        glColor3f(0, 0, 1);
	glBegin(GL_LINE_STRIP);
	for (size_t i = 0; i < num_rec; i++) {
		glVertex2f(i * ww / num_rec, hh-hh * x_val[(buf_start+i) % num_rec]);
	}
	glEnd();

	
        glColor3f(0, 1, 0);
	glBegin(GL_LINE_STRIP);
	for (size_t i = 0; i < num_rec; i++) {
		glVertex2f(i * ww / num_rec, hh-hh * y_val[(buf_start+i) % num_rec]*5);
	}
	glEnd();

        glColor3f(0, 0.4, 0.8);
	glBegin(GL_LINE_STRIP);
	for (size_t i = 0; i < num_rec; i++) {
		glVertex2f(i * ww / num_rec, hh-hh * z_val[(buf_start+i) % num_rec]);
	}
	glEnd();

        glColor3f(0, 0, 0);
	glBegin(GL_LINE_STRIP);
	for (size_t i = 0; i < num_rec; i++) {
		glVertex2f(i * ww / num_rec, hh-hh * a_val[(buf_start+i) % num_rec]);
	}
	glEnd();
}

void TraceView::on_button_event(int button, int state, int x, int y)
{
	std::cerr << "button=" << button << ", state=" << state << std::endl;
	if (button!=2 && state!=1) return;
	if (! fdlg) {
		using namespace gltk;
		fdlg = new Window;
		Box * bx = new Box(ORIENTATION_VERTICAL);
		fdlg->add(bx);
		FileBrowser * bsr = new FileBrowser;
		bx->pack_start(bsr);
		bsr->signal_selected().connect(sigc::mem_fun(this, & TraceView::file_select));
		Box * hb = new Box(ORIENTATION_HORIZONTAL);
		bx->pack_start(hb);
		hb->pack_start(new Label("Filename:"));
		hb->pack_start(fn_entry);
		Button * b = new Button("Cancel");
		bx->pack_start(b);
		b->signal_clicked().connect(sigc::mem_fun(fdlg, & Window::hide));
		b = new Button("OK");
		bx->pack_start(b);
		b->signal_clicked().connect(sigc::mem_fun(this, & TraceView::file_ok));
	}
	fdlg->show();
}

void TraceView::add_record()
{
	double age = vol->get_age();
	if (age < last_t+delta_t) return;
	double tty = 0;
	double ttx = 0;
        double ttz = 0;
	double tta = 0;
  
	size_t snum = vol->get_snum();
	for (size_t l=0; l < snum; l++) {
		MEAsim::Synapse const & sn = vol->get_synapse(l);
		tty += sn.Y;
		ttx += 1.0 - sn.A - sn.Y - sn.Z;
                ttz += sn.Z;
                tta += sn.A;
	}
	x_val[buf_start] = ttx / snum;
	y_val[buf_start] = tty / snum;
        z_val[buf_start] = ttz / snum;
	a_val[buf_start] = tta / snum;

	buf_start = (buf_start + 1) % num_rec;
	last_t = age;
}


void TraceView::file_select(std::string name)
{
	fn_entry.set_text(name);
}

void TraceView::file_ok()
{
	std::string fn = fn_entry.get_text();
	if (! fn.size()) return;
	std::ofstream f(fn.c_str());
	size_t sz = vol->get_nnum();
	double start = vol->get_age() - 300000; // default 10000=10sec
	f.precision(4);
	f << std::fixed;
	for (size_t i = 0; i<num_rec; i++) {
		size_t idx = (buf_start+i) % num_rec;
		f << start+i*delta_t << '\t' << x_val[idx] << '\t' << y_val[idx] <<'\t' << z_val[idx] <<'\t' << a_val[idx] <<'\n';
	}
	fdlg->hide();
}




TraceView::TraceView(MEAsim * vr) :
	vol(vr),
	num_rec(60000),
	buf_start(0),
        fdlg(0),
	last_t(0),
	delta_t(5)
{
	x_val = new double[num_rec];
	y_val = new double[num_rec];
        z_val = new double[num_rec];
        a_val = new double[num_rec];

	for (size_t i=0; i<num_rec; i++) {
		x_val[i] = 0;
		y_val[i] = 0;
		z_val[i] = 0;
                a_val[i] = 0;
	}
}

TraceView::~TraceView()
{
	delete [] x_val;
	delete [] y_val;
        delete [] z_val;
	delete [] a_val;

}

