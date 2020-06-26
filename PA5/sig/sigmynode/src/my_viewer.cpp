# include "my_viewer.h"

# include "sn_mynode.h"

# include <sigogl/ui_button.h>
# include <sigogl/ui_radio_button.h>
# include <sig/sn_primitive.h>
# include <sig/sn_transform.h>
# include <sig/sn_manipulator.h>

# include <sigogl/gl_tools.h>
# include <sigogl/ws_run.h>


MyViewer::MyViewer(int x, int y, int w, int h, const char* l) : WsViewer(x, y, w, h, l)
{
	_nbut = 0;
	_animating = false;
	build_ui();
	build_scene();
}

//new sigapp one

void MyViewer::build_ui()
{
	UiPanel* p, * sp;
	UiManager* uim = WsWindow::uim();
	p = uim->add_panel("", UiPanel::HorizLeft);
	p->add(new UiButton("View", sp = new UiPanel()));
	{	UiPanel* p = sp;
	p->add(_nbut = new UiCheckButton("Normals", EvNormals));
	}
	p->add(new UiButton("Animate", EvAnimate));
	p->add(new UiButton("Exit", EvExit)); p->top()->separate();
}

void MyViewer::add_mynode(int n)
{

	SnMyNode* c;

	float r = 0.15f; // position range
	while (n-- > 0)
	{
		c = new SnMyNode;
		/*if (mcolorbut->value())
			c->multicolor = true;
		else*/
		c->color(GsColor::random());

		/*c->init.set(gs_random(-r, r), gs_random(-r, r), gs_random(-r, r));
		c->width = gs_random(0.001f, r);
		c->height = gs_random(0.001f, r * 2);*/

		// Example how to print/debug your generated data:
		// gsout<<n<<": "<<c->color()<<gsnl;

		rootg()->add(c);
	}
}

void MyViewer::add_model(SnShape* s, GsVec p)
{
	// This method demonstrates how to add some elements to our scene graph: lines,
	// and a shape, and all in a group under a SnManipulator.
	// Therefore we are also demonstrating the use of a manipulator to allow the user to
	// change the position of the object with the mouse. If you do not need mouse interaction,
	// you can just use a SnTransform to apply a transformation instead of a SnManipulator.
	// You would then add the transform as 1st element of the group, and set g->separator(true).
	// Your scene graph should always be carefully designed according to your application needs.

	SnManipulator* manip = new SnManipulator;
	GsMat m;
	m.translation(p);
	manip->initial_mat(m);

	SnGroup* g = new SnGroup;
	SnLines* l = new SnLines;
	l->color(GsColor::orange);
	g->add(s);
	g->add(l);
	manip->child(g);
	 manip->visible(false); // call this to turn off mouse interaction

	rootg()->add(manip);
}

void MyViewer::build_scene()
{
	SnPrimitive* p;

	SnMyNode* torus = new SnMyNode;
	add_mynode(1);
	rootg()->add(torus);

	p = new SnPrimitive(GsPrimitive::Capsule, 0.02f, 0.005f, 0.25f);
	p->prim().material.diffuse = GsColor::black;
	add_model(p, GsVec(0, 0.22f, 0));

	p = new SnPrimitive(GsPrimitive::Capsule, 0.02f, 0.005f, 0.125f);
	p->prim().material.diffuse = GsColor::red;
	add_model(p, GsVec(0, 0.1f, 0.05f));

}

// Below is an example of how to control the main loop of an animation:
void MyViewer::run_animation()
{
	if (_animating) return; // avoid recursive calls
	_animating = true;

	int ind = gs_random(0, rootg()->size() - 1); // pick one child
	SnManipulator* manip = rootg()->get<SnManipulator>(ind); // access one of the manipulators
	GsMat m = manip->mat();
	//GsMat m2 = manip->mat();

	double frdt = 1.0 / 30.0; // delta time to reach given number of frames per second
	double v = 4; // target velocity is 1 unit per second
	double t = 0, lt = 0, t0 = gs_time();
	do // run for a while:
	{
		//big hand rotates 6 degrees every second
		//small hand rotates 6 degrees every 1/60
		//double bigarrow = gs2pi / (60);
		//double smallarrow = gs2pi / (0.166f);

		while (t - lt < frdt) { ws_check(); t = gs_time() - t0; } // wait until it is time for next frame
		//call rotz from GsMat for both angles
		
		/*GsMat rotateZ;
		GsMat rotateZ;*/

		// push both GsMat __ = manip->mat();
		//GsMat bigarrow = manip->mat();
		//GsMat smallarrow = manip->mat();


		//double yinc = (t - lt) * v;
		//if (t > 2) yinc = -yinc; // after 2 secs: go down
		//lt = t;
		//m.e24 += (float)yinc;
		//if (m.e24 < 0) m.e24 = 0; // make sure it does not go below 0

		manip->initial_mat(m);
		render(); // notify it needs redraw
		ws_check(); // redraw now
	} while (m.e24 > 0);
	_animating = false;
}

void MyViewer::show_normals(bool view)
{
	// Note that primitives are only converted to meshes in GsModel
	// at the first draw call.
	GsArray<GsVec> fn;
	SnGroup* r = (SnGroup*)root();
	for (int k = 0; k < r->size(); k++)
	{
		SnManipulator* manip = r->get<SnManipulator>(k);
		SnShape* s = manip->child<SnGroup>()->get<SnShape>(0);
		SnLines* l = manip->child<SnGroup>()->get<SnLines>(1);
		if (!view) { l->visible(false); continue; }
		l->visible(true);
		if (!l->empty()) continue; // build only once
		l->init();
		if (s->instance_name() == SnPrimitive::class_name)
		{
			GsModel& m = *((SnModel*)s)->model();
			m.get_normals_per_face(fn);
			const GsVec* n = fn.pt();
			float f = 0.33f;
			for (int i = 0; i < m.F.size(); i++)
			{
				const GsVec& a = m.V[m.F[i].a]; l->push(a, a + (*n++) * f);
				const GsVec& b = m.V[m.F[i].b]; l->push(b, b + (*n++) * f);
				const GsVec& c = m.V[m.F[i].c]; l->push(c, c + (*n++) * f);
			}
		}
	}
}

int MyViewer::handle_keyboard(const GsEvent& e)
{
	int ret = WsViewer::handle_keyboard(e); // 1st let system check events
	if (ret) return ret;

	switch (e.key)
	{
	case GsEvent::KeyEsc: gs_exit(); return 1;
	case 'n': { bool b = !_nbut->value(); _nbut->value(b); show_normals(b); return 1; }
	default: gsout << "Key pressed: " << e.key << gsnl;
	}

	return 0;
}

int MyViewer::uievent(int e)
{
	switch (e)
	{
	case EvNormals: show_normals(_nbut->value()); return 1;
	case EvAnimate: run_animation(); return 1;
	case EvExit: gs_exit();
	}
	return WsViewer::uievent(e);
}